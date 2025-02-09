#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "../base/global.h"

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

private slots:
    void slot_reset_mod_finish(QString resp, ErrorCodes err, ReqId id);

    void on_fp_return_btn_clicked();

    void on_fp_verify_btn_clicked();

    void on_fp_sure_btn_clicked();

private:
    bool _checkUserValid();
    bool _checkPassValid();
    void _showErrTip(QString str, bool is_right);
    bool _checkEmailValid();
    bool _checkVerifyValid();
    void _addTipErr(TipErrorCode te,QString tips);
    void _delTipErr(TipErrorCode te);
    void _initHandlers();
    Ui::ResetDialog *ui;
    QMap<TipErrorCode, QString> _tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
signals:
    void signal_switch_login2();
};

#endif // RESETDIALOG_H
