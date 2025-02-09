#include "add_user_item.h"
#include "ui_adduseritem.h"
#include "../custom_ui_wid/clicked_btn.h"
#include "../custom_ui_wid/clicked_label.h"
AddUserItem::AddUserItem(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::AddUserItem)
{
    ui->setupUi(this);
    setListItemType(ListItemType::ADD_USER_TIP_ITEM);
}

AddUserItem::~AddUserItem()
{
    delete ui;
}
