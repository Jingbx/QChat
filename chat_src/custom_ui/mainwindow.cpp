/*
 * @File        mainwindow.cpp
 * @brief
 *
 * @Author      Jingbx
 * @Data        2024/12/10
 * @History
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../network_manager/tcp_manager.h"

/**
 * @brief ⚠️: 每次更改中心视图Qt都会析构之前的，所以更改界面时候需要new
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    // 设置初始中心视图login dialog 并让这些dialog都属于mainwindow这一个事件循环，防止不正常析构
    _login_dialog = new LoginDialog(this);
    // 将dialogs嵌入mainwindows, 确保显示
    _login_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_login_dialog);

    // slot 回调 register dialog视图
    connect(_login_dialog, &LoginDialog::switchRegisterBtn, this, &MainWindow::SlotSwitchRegisterBtn);
    // slot 回调 reset pwd视图
    connect(_login_dialog, &LoginDialog::switchResetDialog, this, &MainWindow::SlotSwitchResetDialog);
    // slot 回调 chat dialog视图
    connect(TcpManager::GetInstance().get(), &TcpManager::signal_switch_chatdlg,
            this, &MainWindow::SlotSwitchChatDialog);
    // TODO: slot 回调 chat page 播放器信号
    // connect(_chat_page)

    // ⚠️⚠️⚠️⚠️⚠️⚠️Test⚠️⚠️⚠️⚠️⚠️⚠️
    // emit TcpManager::GetInstance()->signal_switch_chatdlg();
}

MainWindow::~MainWindow() {
    delete ui;
    // if (_login_dialog) {
    //     delete _login_dialog;
    //     _login_dialog = nullptr;
    // }
    // if (_register_dialog) {
    //     delete _register_dialog;
    //     _register_dialog = nullptr;
    // }
}

// slots
void MainWindow::SlotSwitchRegisterBtn() {
    _register_dialog = new RegisterDialog(this);
    _register_dialog->hide();
    _register_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_register_dialog);

    _login_dialog->hide();
    _register_dialog->show();

    // 链接注册成功Return
    connect(_register_dialog, &RegisterDialog::signal_switch_login, this, &MainWindow::slot_switch_login);
}

void MainWindow::SlotSwitchResetDialog() {
    _reset_dialog = new ResetDialog(this);
    // _reset_dialog->hide();
    _reset_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_reset_dialog);

    _login_dialog->hide();
    _reset_dialog->show();
    // 链接取消修改代码Cancel
    connect(_reset_dialog, &ResetDialog::signal_switch_login2, this, &MainWindow::slot_switch_login2);
}

void MainWindow::SlotSwitchChatDialog() {
    _chat_dialog = new ChatDialog();
    _chat_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_chat_dialog);

    _login_dialog->hide();
    _chat_dialog->show();
    this->setMinimumSize(QSize(1050,800));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

void MainWindow::slot_switch_login() {
    _login_dialog = new LoginDialog(this);
    _login_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_login_dialog);

    _register_dialog->hide();
    _login_dialog->show();

    // slot 回调 register dialog视图
    connect(_login_dialog, &LoginDialog::switchRegisterBtn, this, &MainWindow::SlotSwitchRegisterBtn);
    // slot 回调 reset pwd视图
    connect(_login_dialog, &LoginDialog::switchResetDialog, this, &MainWindow::SlotSwitchResetDialog);
}


//从重置界面返回登录界面
void MainWindow::slot_switch_login2() {
    _login_dialog = new LoginDialog(this);
    _login_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_login_dialog);

    _reset_dialog->hide();
    _login_dialog->show();

    // slot 回调 register dialog视图
    connect(_login_dialog, &LoginDialog::switchRegisterBtn, this, &MainWindow::SlotSwitchRegisterBtn);
    // slot 回调 reset pwd视图
    connect(_login_dialog, &LoginDialog::switchResetDialog, this, &MainWindow::SlotSwitchResetDialog);
}





