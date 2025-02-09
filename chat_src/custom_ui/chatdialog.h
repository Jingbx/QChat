#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QList>
#include <QListWidgetItem>
#include "../base/global.h"
#include "../user_manager/user_data.h"
#include "../custom_ui_wid/state_widget.h"

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
protected:
    bool eventFilter(QObject *watched, QEvent *event) override ;

    void handleGlobalMousePress(QMouseEvent *event) ;
private:
    void addChatUserList();
    void addContactUserList();
    void showSearchList(bool bsearch = false);
    void addLBGroup(StateWidget* lb);
    void clearLabelState(StateWidget *lb);

    void loadMoreConUser();

    void SetSelectChatItem(int uid = 0);
    void SetSelectChatPage(int uid = 0);

    void updateChatMsg(std::vector<std::shared_ptr<TextChatData> > msgdata);

private:
    Ui::ChatDialog *ui;
    bool _b_loading;
    ChatUIMode _mode;
    ChatUIMode _state;
    ChatUIMode _before_state;
    QList<StateWidget*> _lb_list;
    QWidget *_last_widget;
    //todo...
    QMap<int, QListWidgetItem*> _chat_items_added;
    int _cur_chat_uid;

public slots:
    /**
     * @brief Enhanced: 点击item重制ChatPage 与不同用户聊天
     */
    void slot_switch_chat_page(const QString &username);
    void slot_loading_chat_user();
    void slot_side_contact();
    void slot_side_chat();
    void slot_text_changed(const QString & str);
    // void slot_focus_out();
    void slot_apply_friend(std::shared_ptr<AddFriendApply> apply_info);
    void slot_loading_contact_user();
    void slot_switch_apply_friend_page();
    void slot_show_search(bool show);
    void slot_friend_info_page(std::shared_ptr<UserInfo> user_info);
    // 直接点击聊天列表进行聊天
    void slot_item_clicked(QListWidgetItem *item);
    // 搜索聊天，并且设计和对端uid服务器进行通信
    void slot_jump_chat_item(std::shared_ptr<SearchInfo> si);
    // 直接点击好友详细信息界面中的聊天按钮
    void slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info);

    // 聊天消息内容显示
    void slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg);
    void slot_append_send_chat_msg(std::shared_ptr<TextChatData> msg);
    // 添加好友后联系人列表和chat列表显示框框
    void slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);
};

#endif // CHATDIALOG_H
