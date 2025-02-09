#ifndef CHAT_PAGE_H
#define CHAT_PAGE_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include "../custom_ui_wid/clicked_btn.h"
#include "../custom_ui_wid/clicked_label.h"
#include "../user_manager/user_data.h"
namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();
    // Enhanced: chatdialog中设置被点击者的姓名 作为聊天标题
    void resetChatPage(const QString &username);

    // 将当前的了聊天界面设置为当前聊天朋友的info
    void setUserInfo(std::shared_ptr<UserInfo> user_info);
    // 展示并添加朋友间聊天气泡消息与滚动
    void appendChatMsg(std::shared_ptr<TextChatData> msg);

private:
    void clearItems();

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private slots:
    void on_send_btn_clicked();
    void slot_send_enter_clicked();

private:
    Ui::ChatPage *ui;
    std::shared_ptr<UserInfo> _user_info;
    QMap<QString, QWidget*>  _bubble_map;
signals:
    void signal_append_send_chat_msg(std::shared_ptr<TextChatData> msg);
};

#endif // CHAT_PAGE_H
