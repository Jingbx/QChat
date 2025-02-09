/*
 * @File        applyfriendpage.h
 * @brief       联系人侧边栏中新的朋友的右侧界面
 *
 * @Author      Jingbx
 * @Data        2024/12/31
 * @History
 */
#ifndef APPLYFRIENDPAGE_H
#define APPLYFRIENDPAGE_H

#include <QWidget>
#include "../user_manager/user_data.h"
#include <memory>
#include <QJsonArray>
#include "applyfrienditem.h"

namespace Ui {
class ApplyFriendPage;
}

class ApplyFriendPage : public QWidget
{
    Q_OBJECT

public:
    explicit ApplyFriendPage(QWidget *parent = nullptr);
    ~ApplyFriendPage();
    void AddNewApply(std::shared_ptr<AddFriendApply> apply);
protected:
    void paintEvent(QPaintEvent *event);
private:
    void loadApplyList();
    Ui::ApplyFriendPage *ui;
    std::unordered_map<int, ApplyFriendItem*> _unauth_items;
public slots:
    void slot_auth_rsp(std::shared_ptr<AuthRsp> );
signals:
    void signal_show_search(bool);
};

#endif // APPLYFRIENDPAGE_H
