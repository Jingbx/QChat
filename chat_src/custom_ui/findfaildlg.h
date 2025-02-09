#ifndef FINDFAILDLG_H
#define FINDFAILDLG_H
#include "../custom_ui_wid/clicked_btn.h"
#include "../custom_ui_wid/clicked_label.h"
#include <QDialog>

namespace Ui {
class FindFailDlg;
}

class FindFailDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindFailDlg(QWidget *parent = nullptr);
    ~FindFailDlg();

private slots:


    void on_fail_sure_btn_clicked();

private:
    Ui::FindFailDlg *ui;
};

#endif // FINDFAILDLG_H
