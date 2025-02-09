#include "search_list.h"
#include<QScrollBar>
#include "../network_manager/tcp_manager.h"
#include "../custom_ui/add_user_item.h"
// #include "invaliditem.h"
#include "../custom_ui/findsuccessdlg.h"
#include "../custom_ui/findfaildlg.h"

#include "customize_edit.h"
// #include "findfaildlg.h"
#include "../custom_ui/loadingdialog.h"
#include "../user_manager/user_data.h"
#include "../user_manager/userinfo_group.h""
#include "list_item_base.h"

SearchList::SearchList(QWidget *parent):QListWidget(parent),_find_dlg(nullptr), _search_edit(nullptr), _send_pending(false)
{
    Q_UNUSED(parent);
     this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    //添加条目
    addTipItem();
    //连接搜索条目
    connect(TcpManager::GetInstance().get(), &TcpManager::signal_user_search, this, &SearchList::slot_user_search);
}

void SearchList::CloseFindDlg()
{
    if(_find_dlg){
        _find_dlg->hide();
        _find_dlg = nullptr;
    }
}

void SearchList::SetSearchEdit(QWidget* edit) {
    _search_edit = edit;
}

void SearchList::waitPending(bool pending)
{
    if(pending){
        _loadingDialog = new LoadingDialog(this);
        _loadingDialog->setModal(true);
        _loadingDialog->show();
    }else{
        _loadingDialog->hide();
        _loadingDialog->deleteLater();
    }
    _send_pending = pending;
}


void SearchList::addTipItem()
{
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item_tmp->setSizeHint(QSize(250,10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);


    auto *add_user_item = new AddUserItem();
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    qDebug() << "点击搜索!";
    QWidget *widget = this->itemWidget(item); // 获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为子类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->getListItemType();  // changed
    qDebug() << "搜索类型: " << itemType;
    if(itemType == ListItemType::INVALID_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }
    // ⚠️ 添加好友需要先向服务器发送请求 查询添加的用户是否存在
    if(itemType == ListItemType::ADD_USER_TIP_ITEM){
        if (_send_pending) {
            // 卡住等待TCP查询消息是否收到rsp
            qDebug()<< "_send_pending true";
            return;
        }
        if (!_search_edit) {
            qDebug()<< "_search_edit false";
            return;
        }
        // TODO: 等待收到rsp时候的加载动画
        waitPending(true);
        // Req消息体
        auto search_edit = dynamic_cast<CustomizeEdit*>(_search_edit);
        auto uid_str = search_edit->text();
        QJsonObject jsonObj;
        jsonObj["uid"] = uid_str;   // ⚠️ 服务器端会设置查找，可以是uid/name再redis sql中查找这里搜索框写什么无所谓了

        QJsonDocument doc(jsonObj);
        QByteArray jsonReq = doc.toJson(QJsonDocument::Compact);

        // 发送查找用户
        qDebug()<< "向服务器发送查找用户存在请求";
        emit TcpManager::GetInstance()->signal_send_data(ReqId::ID_SEARCH_USER_REQ,
                                                         jsonReq);
   }
   //清除弹出框
    CloseFindDlg();
}

// 信号由TCP处理回包从其中获取SearchInfo后发送
void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{
    qDebug() << "服务器存在被查找人!";
    waitPending(false);
    if (si == nullptr) {
        _find_dlg = std::make_shared<FindFailDlg>(this);    // ⚠️多态
    } else {
        //如果是自己，暂且先直接返回，以后看逻辑扩充
        auto self_uid = UserinfoGroup::GetInstance()->getCurrUid();
        if (si->_uid == self_uid) {
            return;
        }
        //此处分两种情况，一种是搜多到已经是自己的朋友了，一种是未添加好友
        //查找是否已经是好友
        bool bExist = UserinfoGroup::GetInstance()->checkFriendById(si->_uid);
        // TODO:
        if(bExist){
            //此处处理已经添加的好友，实现页面跳转
            //跳转到聊天界面指定的item中
            qDebug() << "已经是好友";
            emit signal_jump_chat_item(si);
            return;
        }
        //此处先处理为添加的好友
        _find_dlg = std::make_shared<FindSuccessDlg>(this);
        std::dynamic_pointer_cast<FindSuccessDlg>(_find_dlg)->SetSearchInfo(si);
    }
    if (_find_dlg != nullptr) {
        _find_dlg->exec();  // 阻塞 changed
    } else {
        qDebug() << "_find_dlg is nullptr";
        return;
    }

}
