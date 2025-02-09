#include "empty_page.h"
#include "ui_emptypage.h"

EmptyPage::EmptyPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EmptyPage)
{
    ui->setupUi(this);
    // 设置图片并自动缩放
    QPixmap pixmap(":/resources/empty_label_plane.png"); // 替换为你的图片路径
    ui->empty_label->setPixmap(pixmap);
    ui->empty_label->setScaledContents(true); // 启用自动缩放
}

EmptyPage::~EmptyPage()
{
    delete ui;
}
