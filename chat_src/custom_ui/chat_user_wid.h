/*
 * @File        chat_user_wid.h
 * @brief       包含每个自定义的widget显示头像名字聊天记录时间
 *
 * @Author      Jingbx
 * @Data        2024/12/24
 * @History
 */
#ifndef CHAT_USER_WID_H
#define CHAT_USER_WID_H

#include <QWidget>
#include "../custom_ui_wid/list_item_base.h"
#include "../user_manager/user_data.h"

namespace Ui {
class ChatUserWid;
}

class ChatUserWid : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = nullptr);
    ~ChatUserWid();
    QSize sizeHint() const override;
    void setInfo(std::shared_ptr<UserInfo> user_info);
    void setInfo(std::shared_ptr<FriendInfo> friend_info);
    void showRedPoint(bool bshow);
    std::shared_ptr<UserInfo> getUserInfo();
    void updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs);
    // Enhance:
    QString getUserNameLb();
private:
    Ui::ChatUserWid *ui;
    std::shared_ptr<UserInfo> _user_info;
};

#endif // CHAT_USER_WID_H
