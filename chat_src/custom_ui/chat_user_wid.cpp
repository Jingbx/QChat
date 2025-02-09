#include "chat_user_wid.h"
#include "ui_chatuserwid.h"
#include "../custom_ui_wid/clicked_btn.h"
#include "../custom_ui_wid/clicked_label.h"
ChatUserWid::ChatUserWid(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ChatUserWid)
{
    ui->setupUi(this);
    setListItemType(ListItemType::CHAT_USER_ITEM);
    ui->red_point->raise();
    showRedPoint(false);
}

ChatUserWid::~ChatUserWid()
{
    delete ui;
}

QSize ChatUserWid::sizeHint() const
{
    return QSize(250, 70); // 返回自定义的尺寸
}

void ChatUserWid::setInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
    // 加载图片
    QPixmap pixmap(_user_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_user_info->_name);
    ui->user_chat_lb->setText(_user_info->_last_msg);
}

void ChatUserWid::setInfo(std::shared_ptr<FriendInfo> friend_info)
{
    _user_info = std::make_shared<UserInfo>(friend_info);
    // 加载图片
    QPixmap pixmap(_user_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_user_info->_name);
    ui->user_chat_lb->setText(_user_info->_last_msg);
}

void ChatUserWid::showRedPoint(bool bshow)
{
    if(bshow){
        ui->red_point->show();
    }else{
        ui->red_point->hide();
    }
}

std::shared_ptr<UserInfo> ChatUserWid::getUserInfo()
{
    return _user_info;
}

void ChatUserWid::updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs) {
    qDebug() << "更新最新消息";
    qDebug() << "_user_info->_chat_msgs size: " << _user_info->_chat_msgs.size();
    QString last_msg = "";
    for (auto& msg : msgs) {
        last_msg = msg->_msg_content;
        _user_info->_chat_msgs.push_back(msg);
    }
    qDebug() << "to " << _user_info->_chat_msgs.size();
    _user_info->_last_msg = last_msg;
    ui->user_chat_lb->setText(_user_info->_last_msg);
}

QString ChatUserWid::getUserNameLb()
{
    return ui->user_name_lb->text();
}
