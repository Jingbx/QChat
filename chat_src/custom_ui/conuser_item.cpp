#include "conuser_item.h"
#include "ui_conuseritem.h"

ConUserItem::ConUserItem(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ConUserItem)
{
    ui->setupUi(this);
    setListItemType(ListItemType::CONTACT_USER_ITEM);
    ui->red_point->raise();
    ShowRedPoint(false);
}

ConUserItem::~ConUserItem()
{
    delete ui;
}

QSize ConUserItem::sizeHint() const
{
    return QSize(250, 70); // 返回自定义的尺寸
}

void ConUserItem::SetInfo(std::shared_ptr<AuthInfo> auth_info)
{
    _info = std::make_shared<UserInfo>(auth_info);
    // 加载图片
    qDebug() << "ConUserItem::SetInfo auth_info icon path: " << _info->_icon;
    QPixmap pixmap(_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_info->_name);
}

void ConUserItem::SetInfo(int uid, QString name, QString icon)
{
     // _info = std::make_shared<UserInfo>(uid,name, name, icon, 0);

     // // 加载图片
     // QPixmap pixmap(_info->_icon);

     // // 设置图片自动缩放
     // ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
     // ui->icon_lb->setScaledContents(true);

     // ui->user_name_lb->setText(_info->_name);
     _info = std::make_shared<UserInfo>(uid, name, name, icon, 0);

     // 加载图片
     qDebug() << "ConUserItem::SetInfo icon path: " << _info->_icon;
     QPixmap pixmap(_info->_icon);

     // 将图片缩放到 40x40
     pixmap = pixmap.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);

     // 设置图标
     ui->icon_lb->setPixmap(pixmap);
     ui->icon_lb->setScaledContents(true);

     // 设置用户名
     ui->user_name_lb->setText(_info->_name);
}

void ConUserItem::SetInfo(std::shared_ptr<AuthRsp> auth_rsp){
    _info = std::make_shared<UserInfo>(auth_rsp);

    // 加载图片
    qDebug() << "ConUserItem::SetInfo auth_rsp icon path: " << _info->_icon;
    QPixmap pixmap(_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_info->_name);
}

void ConUserItem::ShowRedPoint(bool show)
{
    if(show){
        ui->red_point->show();
    }else{
        ui->red_point->hide();
    }

}

std::shared_ptr<UserInfo> ConUserItem::GetInfo()
{
    return _info;
}
