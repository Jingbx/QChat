/*
 * @File        apply_friend_list.h
 * @brief       联系人侧边栏中新的朋友的右侧界面中的最近的新好友列表
 *
 * @Author      Jingbx
 * @Data        2024/12/31
 * @History
 */
#ifndef APPLY_FRIEND_LIST_H
#define APPLY_FRIEND_LIST_H
#include <QListWidget>
#include <QEvent>

class ApplyFriendList: public QListWidget
{
     Q_OBJECT
public:
    ApplyFriendList(QWidget *parent = nullptr);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:

signals:
    void signal_show_search(bool);
};

#endif // APPLY_FRIEND_LIST_H
