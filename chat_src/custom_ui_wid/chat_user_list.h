/*
 * @File        chat_user_list.h
 * @brief       联系人列表 搜索 最近聊天 列表 继承QListWidget
 *              重写eventFilter实现悬浮/点击/离开事件感知，并捕获滚轮事件
 *              🎉Enhanced: 点击才加载ChatPage否则为EmptyPage,
 *                        信号传给chatdialog附加上从item上获取的username在chatpage上显示
 *
 * @Author      Jingbx
 * @Data        2024/12/23
 * @History
 */
#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H

#include "../base/global.h"
#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollEvent>
#include <QScrollBar>

class ChatUserList : public QListWidget {
    Q_OBJECT
public:
    ChatUserList(QWidget *parent = nullptr);
    ~ChatUserList();
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
signals:
    // 加载最近聊天, 滚动到底部通知加载更多
    void signal_loading_chat_user();
    // 切换回聊天界面
    void signal_switch_chat_page(const QString &username);
private:
    bool _load_pending;
};

#endif // CHATUSERLIST_H
