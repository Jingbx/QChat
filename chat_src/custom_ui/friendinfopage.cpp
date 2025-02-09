#include "friendinfopage.h"
#include "ui_friendinfopage.h"
#include <QDebug>

FriendInfoPage::FriendInfoPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FriendInfoPage),_user_info(nullptr)
{
    ui->setupUi(this);
    ui->msg_chat->setState("normal","hover","press");
    ui->video_chat->setState("normal","hover","press");
    ui->voice_chat->setState("normal","hover","press");
}

FriendInfoPage::~FriendInfoPage()
{
    delete ui;
}

void FriendInfoPage::SetInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
    // 加载图片
    QPixmap pixmap(user_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->name_lb->setText(user_info->_name);
    ui->nick_lb->setText(user_info->_nick);
    ui->bak_lb->setText("苹果就去想橘子的问题");
}

void FriendInfoPage::on_msg_chat_clicked()
{
    qDebug() << "msg chat btn clicked";
    emit signal_jump_chat_item(_user_info);
}
