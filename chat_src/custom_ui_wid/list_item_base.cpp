#include "list_item_base.h"

ListItemBase::ListItemBase(QWidget *parent) : QWidget(parent) {
}

void ListItemBase::setListItemType(ListItemType list_item_type) {
    _list_item_type = list_item_type;
}

ListItemType ListItemBase::getListItemType() {
    return _list_item_type;
}

void ListItemBase::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
