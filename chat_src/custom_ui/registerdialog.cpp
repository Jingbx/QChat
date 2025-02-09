#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "../network_manager/http_manager.h"
#include "../custom_ui_wid/clicked_btn.h"
#include "../custom_ui_wid/clicked_label.h"
RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent), ui(new Ui::RegisterDialog) {
    ui->setupUi(this);
    // 为pwd设置lineEdit不可见模式
    ui->pwd_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);
    // pwd & confirm state链接 更新图标
    ui->pwd_visible->setState("unvisible", "unvisible_hover", "",
                              "visible", "visible_hover", "");
    ui->confirm_visible->setState("unvisible", "unvisible_hover", "",
                              "visible", "visible_hover", "");
    // pwd & confirm press链接 更新密码Echo
    connect(ui->pwd_visible, &ClickedLabel::clicked, [this](){
        if (ClickLabelState::NORMAL == ui->pwd_visible->getCurrState()) {
            ui->pwd_edit->setEchoMode(QLineEdit::Password);
        } else {
            ui->pwd_edit->setEchoMode(QLineEdit::Normal);
        }
    });
    connect(ui->confirm_visible, &ClickedLabel::clicked, [this](){
        if (ClickLabelState::NORMAL == ui->confirm_visible->getCurrState()) {
            ui->confirm_edit->setEchoMode(QLineEdit::Password);
        } else {
            ui->confirm_edit->setEchoMode(QLineEdit::Normal);
        }
    });
    // err_tip 初始样式属性
    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);
    // 验证码请求发送槽连接 ⚠️直接获取单例 需要裸指针
    connect(HttpManager::GetInstance().get(), &HttpManager::signal_register_mod_finished,
            this, &RegisterDialog::slot_register_mod_finished);
    // 初始化Http Req处理表
    _initHttpHandlers();

    // 创建并链接定时器计时
    _countdown_timer = new QTimer(this);
    connect(_countdown_timer, &QTimer::timeout, [this](){
        if (_countdown == 0) {
            _countdown_timer->stop();
            // ⚠️: 自动跳转主界面
            emit signal_switch_login();
            return;
        }
        _countdown--;
        auto str = QString("Return to login after %1s ").arg(_countdown);
        ui->tip_lb2->setText(str);
    });
}

RegisterDialog::~RegisterDialog() {
    delete ui;
}

// functions
void RegisterDialog::_initHttpHandlers() {
    // 闭包(伪闭包)
    // handler 1: ID_GET_VERICODE
    _handlers.insert(ReqId::ID_GET_VERICODE, [this] (QJsonObject jsonObj) {
        int err = jsonObj["error"].toInt();
        // Enhanced: 检查频繁发送✅
        if (BackendErrorCodes::BE_ERR_TOOMANYREQ == err) {
            _showErrTip(tr("Vericode sending interval is 15s"), false);
            return;
        }
        if (err != ErrorCodes::SUCCESS) {
            _showErrTip(tr("JSON Params Err"), false);
            return;
        }

        auto email = jsonObj["email"].toString();
        qDebug() << "email is " << email ;
        _showErrTip(tr("VeriCode has been sent!"), true);
    });

    // handler 2: ID_REGISTER_USER
    _handlers.insert(ReqId::ID_REGISTER_USER, [this] (QJsonObject jsonObj) {
        int err = jsonObj["error"].toInt();

        // Enhanced: 验证码超时✅
        if (BackendErrorCodes::BE_ERR_VERIFY_EXPIRED == err) {
            qDebug() << "error: " << err;
            _showErrTip(tr("VeriCode is expired or not exited, reget!"), false);
            return;
        }
        // Enhanced: 验证码错误✅
        if (BackendErrorCodes::BE_ERR_VERIFY_ERR  == err) {
            qDebug() << "error: " << err;
            _showErrTip(tr("VeriCode is error!"), false);
            return;
        }
        // Enhanced: 用户/邮件已存在MySQL
        if (BackendErrorCodes::BE_ERR_USER_OR_EMAIL_EXISTED == err) {
            qDebug() << "error: " << err;
            _showErrTip(tr("User or email is exist!"), false);
            return;
        }
        if (err != ErrorCodes::SUCCESS) {
            _showErrTip(tr("Register backend error, sorry!"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        qDebug() << "email is " << email ;

        auto username = jsonObj["user"].toString();
        qDebug() << "username is " << username ;

        auto uid = jsonObj["uid"].toInt();
        qDebug() << "uid is " << uid ;

        // TODO: 切换界面
        _showErrTip(tr("Register success!"), true);
        _changeTipPage();
    });
}

void RegisterDialog::_showErrTip(QString str, bool is_right) {
    ui->err_tip->setText(str);
    if (is_right) {
        ui->err_tip->setProperty("state", "normal");
    } else {
        ui->err_tip->setProperty("state", "err");
    }
    repolish(ui->err_tip);
}


// btn slots
/**
 * @brief 验证码槽
 *        正则表达式验证邮箱正确性 -> 发送http
 *                             -> err_tip报错
 */
void RegisterDialog::on_getvericode_btn_clicked() {
    const QString email = ui->email_edit->text();
    // 正则表达式验证邮箱格式
    bool is_match = _checkEmailValid();

    if (is_match) {
        // http发送验证码-构造HTTP POST请求
        QJsonObject req_json;
        req_json["email"] = email;
        HttpManager::GetInstance()->postHttpReq(QUrl(gate_url_prefix + "/get_vericode"),
                                                req_json,
                                                ReqId::ID_GET_VERICODE,
                                                Modules::REGISTERMOD);

        _showErrTip(tr("VeriCode send success!"), true);
    } else {
        return;
    }
}

/**
 * @brief 注册槽
 *        检查验证码是否正确 是否过期不应该再前端中，需要在GateServer中检查Redis缓存
 */
void RegisterDialog::on_sure_btn_clicked() {
    bool valid = _checkUserValid();
    if(!valid){
        return;
    }

    valid = _checkEmailValid();
    if(!valid){
        return;
    }

    valid = _checkPassValid();
    if(!valid){
        return;
    }

    valid = _checkConfirmValid();
    if(!valid){
        return;
    }

    valid = _checkVerifyValid();
    if(!valid){
        return;
    }

    // 发送注册请求
    // 传入未序列化json数据对象， HttpManager发送post请求
    QJsonObject register_req;
    register_req["user"] = ui->user_edit->text();
    register_req["email"] = ui->email_edit->text();
    register_req["password"] = xorString(ui->pwd_edit->text());
    register_req["confirm"] = xorString(ui->confirm_edit->text());
    register_req["verifycode"] = ui->verify_edit->text();

    HttpManager::GetInstance()->postHttpReq(gate_url_prefix + "/user_register",
                                            register_req,
                                            ReqId::ID_REGISTER_USER,
                                            Modules::REGISTERMOD);
    _showErrTip(tr("Registration request sent!"), true);

}

void RegisterDialog::on_return_login_clicked() {
    _countdown_timer->stop();
    emit signal_switch_login();
}



// my slots
void RegisterDialog::slot_register_mod_finished(QString resp, ErrorCodes err_code, ReqId req_id) {
    // 错误: 判断错误原因，更新err_tip； TODO: 写入日志
    // 1. reqID : 获取验证码
    if (ReqId::ID_GET_VERICODE == req_id) {
        if (err_code != ErrorCodes::SUCCESS) {
            _showErrTip(tr("Http Request Error"), false);
            return;
        }
        // 正确: Json字符串流反序列化为正常Json data, 更新err_tip;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(resp.toUtf8());
        if (jsonDoc.isNull()) {
            _showErrTip(tr("Response Deserialization Error0"), false);
            return;
        }
        else if (!jsonDoc.isObject()) {
            _showErrTip(tr("Response Deserialization Error1"), false);
            return;
        }

        // _handlers Map处理器集合(类似rote action表)，根据ReqId索引对应的可调用对象
        // ⚠️jsonDos.object()是反序列化后的resp json对象
        _handlers[req_id](jsonDoc.object());
    }
    // 2. reqID : 用户注册
    if (ReqId::ID_REGISTER_USER == req_id) {
        if (err_code != ErrorCodes::SUCCESS) {
            _showErrTip(tr("Http Request Error"), false);
            return;
        }
        QJsonDocument jsonDoc = QJsonDocument::fromJson(resp.toUtf8());
        if (jsonDoc.isNull()) {
            _showErrTip(tr("Response Deserialization Error0"), false);
            return;
        }
        else if (!jsonDoc.isObject()) {
            _showErrTip(tr("Response Deserialization Error1"), false);
            return;
        }
        _handlers[req_id](jsonDoc.object());
    }
}




/**
 * @brief Utils
 * @return
 */
bool RegisterDialog::_checkUserValid()
{
    if(ui->user_edit->text() == ""){
        _addTipErr(TipErrorCode::TIP_ERR_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    _delTipErr(TipErrorCode::TIP_ERR_USER_ERR);
    return true;
}

bool RegisterDialog::_checkEmailValid()
{
    static const QRegularExpression emailRegex(RegexPatterns::EMAIL);
    auto email = ui->email_edit->text();

    if (!emailRegex.match(email).hasMatch()) {
        _addTipErr(TipErrorCode::TIP_ERR_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    _delTipErr(TipErrorCode::TIP_ERR_EMAIL_ERR);
    return true;
}

bool RegisterDialog::_checkPassValid()
{
    static const QRegularExpression passRegex(RegexPatterns::PASSWORD);
    auto pass = ui->pwd_edit->text();
    auto confirm = ui->confirm_edit->text();

    if (pass.length() < 6 || pass.length() > 15) {
        _addTipErr(TipErrorCode::TIP_ERR_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    if (!passRegex.match(pass).hasMatch()) {
        _addTipErr(TipErrorCode::TIP_ERR_PWD_ERR, tr("不能包含非法字符"));
        return false;
    }

    _delTipErr(TipErrorCode::TIP_ERR_PWD_ERR);

    if (pass != confirm) {
        _addTipErr(TipErrorCode::TIP_ERR_PWD_CONFIRM, tr("密码和确认密码不匹配"));
        return false;
    } else {
        _delTipErr(TipErrorCode::TIP_ERR_PWD_CONFIRM);
    }
    return true;
}

bool RegisterDialog::_checkConfirmValid()
{
    static const QRegularExpression confirmRegex(RegexPatterns::PASSWORD);
    auto pass = ui->pwd_edit->text();
    auto confirm = ui->confirm_edit->text();

    if (confirm.length() < 6 || confirm.length() > 15) {
        _addTipErr(TipErrorCode::TIP_ERR_CONFIRM_ERR, tr("密码长度应为6~15"));
        return false;
    }

    if (!confirmRegex.match(confirm).hasMatch()) {
        _addTipErr(TipErrorCode::TIP_ERR_CONFIRM_ERR, tr("不能包含非法字符"));
        return false;
    }

    _delTipErr(TipErrorCode::TIP_ERR_CONFIRM_ERR);

    if (pass != confirm) {
        _addTipErr(TipErrorCode::TIP_ERR_PWD_CONFIRM, tr("确认密码和密码不匹配"));
        return false;
    } else {
        _delTipErr(TipErrorCode::TIP_ERR_PWD_CONFIRM);
    }
    return true;
}

bool RegisterDialog::_checkVerifyValid()
{
    auto pass = ui->verify_edit->text();
    if(pass.isEmpty()){
        _addTipErr(TipErrorCode::TIP_ERR_VERIFY_ERR , tr("验证码不能为空"));
        return false;
    }

    _delTipErr(TipErrorCode::TIP_ERR_VERIFY_ERR );
    return true;
}

void RegisterDialog::_addTipErr(TipErrorCode te, QString tips)
{
    _tip_errs[te] = tips;
    _showErrTip(tips, false);
}

void RegisterDialog::_delTipErr(TipErrorCode te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
      ui->err_tip->clear();
      return;
    }

    _showErrTip(_tip_errs.first(), false);
}

void RegisterDialog::_changeTipPage() {
    // 重启定时器，计时单位1s
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    _countdown_timer->start(1000);
}



void RegisterDialog::on_cancel_btn_clicked() {
    emit signal_switch_login();
}

