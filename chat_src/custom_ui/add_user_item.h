/*
 * @File        add_user_item.h
 * @brief       类似好友聊天的小框，这里是搜索的小框
 *
 * @Author      Jingbx
 * @Data        2024/12/25
 * @History
 */
#ifndef ADD_USER_ITEM_H
#define ADD_USER_ITEM_H

#include <QWidget>
#include "../custom_ui_wid/list_item_base.h"
namespace Ui {
class AddUserItem;
}

class AddUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit AddUserItem(QWidget *parent = nullptr);
    ~AddUserItem();
    QSize sizeHint() const override {
        return QSize(250, 70); // 返回自定义的尺寸
    }
protected:

private:
    Ui::AddUserItem *ui;
};

#endif // ADD_USER_ITEM_H
