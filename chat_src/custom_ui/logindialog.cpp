#include "logindialog.h"
#include "ui_logindialog.h"
#include "../network_manager/http_manager.h"
#include "../network_manager/tcp_manager.h"
#include <QMovie>
#include <QPainter>
#include "../custom_ui_wid/clicked_btn.h"
#include "../custom_ui_wid/clicked_label.h"
LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);
    // 设置动图
    setMovLabel(ui->logo_label, ":resources/plane2.gif");

    // 设置密码不可见
    ui->pwd_edit->setEchoMode(QLineEdit::Password);

    // 设置forgetlabel 并链接忘记密码resp槽信号
    ui->forget_pwd_label->setState("normal","hover","press","selected","selected_hover","selected_press");
    connect(ui->forget_pwd_label, &ClickedLabel::clicked, this, &LoginDialog::slot_forget_pwd);
    // 连接登录resp槽信号
    connect(HttpManager::GetInstance().get(), &HttpManager::signal_login_mod_finished,
            this, &LoginDialog::slot_login);
    // 连接TcpManager
    connect(this, &LoginDialog::signal_connect_tcp,
            TcpManager::GetInstance().get(), &TcpManager::slot_connect_tcp);
    connect(TcpManager::GetInstance().get(), &TcpManager::signal_conn_success,
            this, &LoginDialog::slot_conn_success);
    connect(TcpManager::GetInstance().get(), &TcpManager::signal_login_failed,
            this, &LoginDialog::slot_login_failed);
    // initHttp
    _initHttpHandlers();
}

LoginDialog::~LoginDialog() {
    delete ui;
}

// Handlers
void LoginDialog::_initHttpHandlers() {
    // 1.handler login
    _handlers.insert(ReqId::ID_USER_LOGIN, [this](QJsonObject jsonObj){
        int err = jsonObj["error"].toInt();
        // 解析BackendError
        if (BackendErrorCodes::BE_ERR_EMAIL_NOT_EXITED == err) {
            _showErrTip(tr("User is not exited, need register!"), false);
            _enableBtn(true);   //重制按钮
            return;
        }
        if (BackendErrorCodes::BE_ERR_PWD_NOT_MATCH == err) {
            _showErrTip(tr("Password error, try again!"), false);
            _enableBtn(true);   //重制按钮
            return;
        }
        if (BackendErrorCodes::BE_ERR_GET_CAHT_SERVER == err) {
            _showErrTip(tr("Failed to get chat server, sorry!"), false);
            _enableBtn(true);   //重制按钮
            return;
        }
        // 校验ErrorCodes
        if (err != ErrorCodes::SUCCESS) {
            _showErrTip(tr("Register backend error, sorry!"), false);
            _enableBtn(true);   //重制按钮
            return;
        }

        // ⚠️存到ServerInfo中,用于创建TCP连接，
        // 连接创建成功像ChatServer服务发送TCP请求，
        // ChatServer服务向StatusServer询问uid token是否正确
        _uid = jsonObj["uid"].toInt();
        _token = jsonObj["token"].toString();
        ServerInfo server_info;
        server_info.host = jsonObj["host"].toString();
        server_info.port = jsonObj["port"].toString();
        server_info.uid = _uid;
        server_info.token = _token;

        qDebug() << "[uid is " << _uid
                 << "] [email is " << jsonObj["email"].toString()
                 << "] [token is " << _token
                 << "] [ChatServer ip: " << jsonObj["host"].toString()
                 << "] [ChatServer port: " << jsonObj["port"].toString() << "]";

        _showErrTip(tr("success, waiting for connect!"), true);

        // 发送连接信号 ⚠️TCP Manager slot
        emit signal_connect_tcp(server_info);
    });

}

void LoginDialog::_showErrTip(QString str, bool is_right) {
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
bool LoginDialog::_checkEmailValid()
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

bool LoginDialog::_checkPassValid()
{
    static const QRegularExpression passRegex(RegexPatterns::PASSWORD);
    auto pass = ui->pwd_edit->text();
    // fixed: 前端直接抛出密码错误
    if (pass.length() < 6 || pass.length() > 15) {
        _addTipErr(TipErrorCode::TIP_ERR_PWD_ERR, tr("Password Error"));
        return false;
    }
    _delTipErr(TipErrorCode::TIP_ERR_PWD_ERR);
    return true;
}

void LoginDialog::_addTipErr(TipErrorCode te, QString tips)
{
    _tip_errs[te] = tips;
    _showErrTip(tips, false);
}

void LoginDialog::_delTipErr(TipErrorCode te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }

    _showErrTip(_tip_errs.first(), false);
}

bool LoginDialog::_enableBtn(bool enabeld) {
    ui->login_btn->setEnabled(enabeld);
    ui->register_btn->setEnabled(enabeld);
    return true;
}


// slots
/**
 * @brief: 注册回调，发送signal给mainwindow切换center显示
*/
void LoginDialog::on_register_btn_clicked() {
    emit switchRegisterBtn();
}

void LoginDialog::slot_forget_pwd() {
    qDebug() << "Change password!";
    emit switchResetDialog();
}

void LoginDialog::on_login_btn_clicked() {
    qDebug() << "Login!";

    // 检查ui
    bool valid = _checkEmailValid();
    if(!valid){
        return;
    }

    valid = _checkPassValid();
    if(!valid){
        return;
    }
    // ⚠️
    _enableBtn(false);   //只允许点击一次按钮
    //发送http重置用户请求
    QJsonObject json_obj;
    json_obj["email"] = ui->email_edit->text();
    json_obj["password"] = xorString(ui->pwd_edit->text());
    HttpManager::GetInstance()->postHttpReq(QUrl(gate_url_prefix+"/user_login"),
                                            json_obj, ReqId::ID_USER_LOGIN,Modules::LOGINMOD);
    _showErrTip(tr("Login request has been sent!"), true);
}

void LoginDialog::slot_login(QString resp, ErrorCodes err_code, ReqId req_id) {
    // 1.Login
    if (ReqId::ID_USER_LOGIN == req_id) {
        // 反序列化并解析json resp
        if (err_code != ErrorCodes::SUCCESS) {
            _showErrTip(tr("Http Request Error"), false);
            _enableBtn(true);
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
        // handler 调用
        _handlers[req_id](jsonDoc.object());
    } else {
        qDebug() << "Unexpected login request!";
    }
}

void LoginDialog::slot_conn_success(bool is_success) {
    // 发送ID_CHAT_LOGIN登录请求，Chat Status交互验证token
    if (is_success) {
        _showErrTip(tr("Chat connect success, Logging in..."), true);
        QJsonObject jsonObj;
        if (_token.isEmpty()) {
            qDebug() << "ID_CHAT_LOGIN error, token is empty!";
            return;
        }
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;
        QJsonDocument jsonDoc(jsonObj);
        QByteArray jsonStr = jsonDoc.toJson(QJsonDocument::Indented);  // 缩进

        TcpManager::GetInstance()->signal_send_data(ReqId::ID_CHAT_LOGIN, jsonStr);
    } else {
        _showErrTip(tr("Tcp connect error!"), false);
        _enableBtn(true);   //重制按钮
    }
}

void LoginDialog::slot_login_failed(int err) {
    QString err_str = QString("ChatServer connect error: %1").arg(err);
    _showErrTip(err_str, false);
    _enableBtn(true);   //重制按钮
}

