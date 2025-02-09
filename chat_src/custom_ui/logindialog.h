#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "../base/global.h"
#include "../custom_ui_wid/clicked_btn.h"
#include "../custom_ui_wid/clicked_label.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    /// function
    /**
     * @brief initHttpHandlers 初始化Req处理器
     */
    void _initHttpHandlers();

    /**
     * @brief showErrTip ui显示err_tip信息
     * @param err_tip信息
     */
    void _showErrTip(QString str, bool is_right);

    /**
     * @brief check input text
     * @return
     */
    bool _checkEmailValid();
    bool _checkPassValid();

    /**
     * @brief 增删错误显示
     */
    void _addTipErr(TipErrorCode te,QString tips);
    void _delTipErr(TipErrorCode te);
    bool _enableBtn(bool enabled);

private:
    Ui::LoginDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
    QMap<TipErrorCode, QString> _tip_errs;

    int     _uid;
    QString _token;

signals:
    void switchRegisterBtn();
    void switchResetDialog();
    void signal_connect_tcp(ServerInfo);

private slots:
    void on_register_btn_clicked();
    void slot_forget_pwd();
    void on_login_btn_clicked();
    void slot_login(QString resp, ErrorCodes err_code, ReqId req_id);
    // tcp
    void slot_conn_success(bool is_success);
    void slot_login_failed(int err);
};

#endif // LOGINDIALOG_H
