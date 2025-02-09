#include "loadingdialog.h"
#include "ui_loadingdialog.h"
#include <QLabel>
#include <QMovie>
#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QScreen>
#include "../custom_ui_wid/clicked_btn.h"
#include "../custom_ui_wid/clicked_label.h"
LoadingDialog::LoadingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadingDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明
    // 获取屏幕尺寸
    setFixedSize(parent->size()); // 设置对话框为全屏尺寸
}

LoadingDialog::~LoadingDialog()
{
    delete ui;
}
