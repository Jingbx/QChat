#include "resetdialog.h"
#include "ui_resetdialog.h"
#include <QDebug>
#include <QRegularExpression>
#include "../base/global.h"
#include "../network_manager/http_manager.h"
#include "../custom_ui_wid/clicked_btn.h"
#include "../custom_ui_wid/clicked_label.h"
ResetDialog::ResetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResetDialog)
{
    ui->setupUi(this);
    // err_tip 初始样式属性
    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);

    connect(ui->user_edit,&QLineEdit::editingFinished,this,[this](){
        _checkUserValid();
    });

    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        _checkEmailValid();
    });

    connect(ui->pwd_edit, &QLineEdit::editingFinished, this, [this](){
        _checkPassValid();
    });


    connect(ui->verify_edit, &QLineEdit::editingFinished, this, [this](){
         _checkVerifyValid();
    });

    //连接reset相关信号和注册处理回调
    connect(HttpManager::GetInstance().get(), &HttpManager::signal_reset_mod_finished, this,
            &ResetDialog::slot_reset_mod_finish);
    _initHandlers();
}


ResetDialog::~ResetDialog()
{
    delete ui;
}

// TODO: 没连接到jlkdjaskdkjaskljdkasjkldjsakljdakljk
void ResetDialog::slot_reset_mod_finish(QString resp, ErrorCodes err, ReqId id)
{
    qDebug() << "ajfklasklfj";
    if(err != ErrorCodes::SUCCESS){
        _showErrTip(tr("NetWork Error"),false);
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(resp.toUtf8());
    //json解析错误
    if(jsonDoc.isNull()){
        _showErrTip(tr("Json Error"),false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        _showErrTip(tr("Json Error"),false);
        return;
    }


    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}


void ResetDialog::_initHandlers()
{
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


    //注册注册用户回包逻辑
    _handlers.insert(ReqId::ID_RESET_PWD, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        qDebug() << "======0";
        // TODO: User Email不匹配✅
        if (BackendErrorCodes::BE_ERR_NO_MATCHED_EMAIL == error) {
            _showErrTip(tr("User and Email is not matched!"),false);
            return;
        }
        // TODO: Update password失败
        if (BackendErrorCodes::BE_ERR_UPDATE_PWD == error) {
            _showErrTip(tr("Update password error!"),false);
            return;
        }
        if(error != ErrorCodes::SUCCESS){
            _showErrTip(tr("Backend Error!"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        qDebug() << "======";
        _showErrTip(tr("Reset password success!"), true);
        qDebug()<< "email is " << email ;
        // qDebug()<< "user uid is " <<  jsonObj["uid"].toInt();
    });
}

void ResetDialog::_showErrTip(QString str, bool is_right) {
    ui->err_tip->setText(str);
    if (is_right) {
        ui->err_tip->setProperty("state", "normal");
    } else {
        ui->err_tip->setProperty("state", "err");
    }
    repolish(ui->err_tip);
}



/**
 * @brief Utils
 * @return
 */
bool ResetDialog::_checkUserValid()
{
    if(ui->user_edit->text() == ""){
        _addTipErr(TipErrorCode::TIP_ERR_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    _delTipErr(TipErrorCode::TIP_ERR_USER_ERR);
    return true;
}

bool ResetDialog::_checkEmailValid()
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

bool ResetDialog::_checkPassValid()
{
    static const QRegularExpression passRegex(RegexPatterns::PASSWORD);
    auto pass = ui->pwd_edit->text();

    if (pass.length() < 6 || pass.length() > 15) {
        _addTipErr(TipErrorCode::TIP_ERR_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    if (!passRegex.match(pass).hasMatch()) {
        _addTipErr(TipErrorCode::TIP_ERR_PWD_ERR, tr("不能包含非法字符"));
        return false;
    }

    _delTipErr(TipErrorCode::TIP_ERR_PWD_ERR);
    return true;
}


bool ResetDialog::_checkVerifyValid()
{
    auto pass = ui->verify_edit->text();
    if(pass.isEmpty()){
        _addTipErr(TipErrorCode::TIP_ERR_VERIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    _delTipErr(TipErrorCode::TIP_ERR_VERIFY_ERR);
    return true;
}

void ResetDialog::_addTipErr(TipErrorCode te, QString tips)
{
    _tip_errs[te] = tips;
    _showErrTip(tips, false);
}

void ResetDialog::_delTipErr(TipErrorCode te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }

    _showErrTip(_tip_errs.first(), false);
}


// slots
void ResetDialog::on_fp_return_btn_clicked() {
    emit signal_switch_login2();
}

void ResetDialog::on_fp_verify_btn_clicked()
{

    auto email = ui->email_edit->text();
    auto bcheck = _checkEmailValid();
    if(!bcheck){
        return;
    }

    //发送http请求获取验证码
    QJsonObject json_obj;
    json_obj["email"] = email;
    HttpManager::GetInstance()->postHttpReq(QUrl(gate_url_prefix+"/get_vericode"),
                                            json_obj, ReqId::ID_GET_VERICODE, Modules::RESETMOD);
    _showErrTip(tr("VeriCode send success!"), true);
}

void ResetDialog::on_fp_sure_btn_clicked()
{
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

    valid = _checkVerifyValid();
    if(!valid){
        return;
    }

    //发送http重置用户请求
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["new_password"] = xorString(ui->pwd_edit->text());
    json_obj["verifycode"] = ui->verify_edit->text();
    HttpManager::GetInstance()->postHttpReq(QUrl(gate_url_prefix+"/reset_pwd"),
                 json_obj, ReqId::ID_RESET_PWD,Modules::RESETMOD);
    _showErrTip(tr("Reset request has been sent!"), true);
}






