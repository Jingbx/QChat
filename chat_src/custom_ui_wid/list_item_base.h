#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H

#include "../base/global.h"
#include <QStyleOption>
#include <QPainter>
#include <QStyle>
class ListItemBase : public QWidget {
    Q_OBJECT
public:
    // 显式使用构造函数，不允许隐式转换
    explicit ListItemBase(QWidget *parent = nullptr);
    void setListItemType(ListItemType list_item_type);
    ListItemType getListItemType();
protected:
    virtual void paintEvent(QPaintEvent *event) override;
private:
    ListItemType    _list_item_type;
};

#endif // LISTITEMBASE_H
