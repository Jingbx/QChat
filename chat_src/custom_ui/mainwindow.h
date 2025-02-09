/*
 * @File        mainwindow.h
 * @brief
 *
 * @Author      Jingbx
 * @Data        2024/12/10
 * @History
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
#include "chatdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void SlotSwitchRegisterBtn();
    void SlotSwitchResetDialog();
    void SlotSwitchChatDialog();

private slots:
    void slot_switch_login();
    void slot_switch_login2();

private:
    Ui::MainWindow  *ui;
    LoginDialog     *_login_dialog;
    RegisterDialog  *_register_dialog;
    ResetDialog     *_reset_dialog;
    ChatDialog      *_chat_dialog;
};
#endif // MAINWINDOW_H
