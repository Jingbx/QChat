/*
 * @File        loadingdialog.h
 * @brief       继续加载用户/好友列表通过Dialog without btn实现
 *
 * @Author      Jingbx
 * @Data        2024/12/24
 * @History
 */
#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include "../base/global.h"
#include <QDialog>
#include "../custom_ui_wid/clicked_btn.h"
#include "../custom_ui_wid/clicked_label.h"
namespace Ui {
    class LoadingDialog;
}

class LoadingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadingDialog(QWidget *parent = nullptr);
    ~LoadingDialog();

private:
    Ui::LoadingDialog *ui;
};

#endif // LOADINGDIALOG_H
