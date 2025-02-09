/*
 * @File        registerdialog.h
 * @brief
 *
 * @Author      Jingbx
 * @Data        2024/12/11
 * @History
 */
#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "../base/global.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

signals:
    void signal_switch_login();

private slots:
    void on_getvericode_btn_clicked();
    /**
     * @brief on_http_finished 收到http回包后在Handler处理表中查找reqId对应的处理
     */
    void slot_register_mod_finished(QString resp, ErrorCodes err_code, ReqId req_id);

    void on_sure_btn_clicked();

    void on_return_login_clicked();

    void on_cancel_btn_clicked();

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
    bool _checkUserValid();
    bool _checkEmailValid();
    bool _checkPassValid();
    bool _checkVerifyValid();
    bool _checkConfirmValid();

    /**
     * @brief 增删错误显示
     */
    void _addTipErr(TipErrorCode te,QString tips);
    void _delTipErr(TipErrorCode te);

    /**
     * @brief 注册成功转到提示界面
     */
    void _changeTipPage();

private:
/// member
Ui::RegisterDialog *ui;
/**
  * @brief _handlers 类似 route action的处理 根据不同ReqId索引对应的callback
  */
QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
/**
 * @brief Map存储tip_err，如果判断通过就从Map中删除
 */
QMap<TipErrorCode, QString> _tip_errs;

// 定时器
QTimer  *_countdown_timer = nullptr;
int     _countdown = 5;

};
#endif // REGISTERDIALOG_H
