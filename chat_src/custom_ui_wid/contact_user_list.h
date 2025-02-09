/*
 * @File        contact_user_list.h
 * @brief       联系人列表
 *
 * @Author      Jingbx
 * @Data        2025/01/09
 * @History
 */
#ifndef CONTACT_USER_LIST_H
#define CONTACT_USER_LIST_H
#include <QListWidget>
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QDebug>
#include <memory>
#include "../user_manager/user_data.h"
class ConUserItem;

class ContactUserList : public QListWidget
{
    Q_OBJECT
public:
    ContactUserList(QWidget* parent = nullptr);
    void ShowRedPoint(bool bshow = true);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override ;
private:
    void addContactUserList();

public slots:
    /**
      * @brief 两个组标签：添加好友和联系人列表是标题，不允许点击  group tip item
      *        两个组标签下面是添加新朋友可点击item和联系人item  customer item
      */
     void slot_item_clicked(QListWidgetItem *item);
    /**
      * @brief 发送添加好友请求TCP
      */
     void slot_add_auth_firend(std::shared_ptr<AuthInfo>);
     /**
      * @brief 同意别人的验证请求，TCP给服务器发送同意， 服务器回给Rsp
      */
     void slot_auth_rsp(std::shared_ptr<AuthRsp>);

signals:
    void signal_loading_contact_user();
    /**
     * @brief 切换到第一个group tip item1 添加朋友列表
     */
    void signal_switch_apply_friend_page();
    /**
     * @brief 切换到第一个group tip item2 显示好友具体信息，可以发起聊天等
     */
    void signal_switch_friend_info_page(std::shared_ptr<UserInfo> user_info);
private:
    bool                _load_pending;
    ConUserItem         *_add_friend_item;
    QListWidgetItem     *_groupitem;
};

#endif // CONTACT_USER_LIST_H
