#include "contact_user_list.h"
#include "../base/global.h"
#include "../custom_ui_wid/list_item_base.h"
#include "../custom_ui/group_tip_item.h"
#include "../custom_ui/conuser_item.h"
#include <QRandomGenerator>
#include "../network_manager/tcp_manager.h"
#include "../user_manager/userinfo_group.h"
#include <QTimer>
#include <QCoreApplication>

// test===============
std::vector<QString> con_strs = {"Fuck!", "Fuck you?", "Fuck your mom!", "Fuck your dad?", "Bye!"};
std::vector<QString> con_heads = {":/resources/head_1.jpg", ":/resources/head_2.jpg", ":/resources/head_3.jpg", ":/resources/head_4.jpg", ":/resources/head_5.jpg"};
std::vector<QString> con_names = {"AA", "BB", "CC", "DD", "EE"};
// test===============


ContactUserList::ContactUserList(QWidget *parent): QListWidget(parent), _add_friend_item(nullptr)
  ,_load_pending(false)
{
     this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
     this->viewport()->installEventFilter(this);

    //TODO: ⚠️模拟从数据库或者后端传输过来的数据,进行列表加载
    addContactUserList();

    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &ContactUserList::slot_item_clicked);

    // //链接对端同意认证后通知的信号
    connect(TcpManager::GetInstance().get(), &TcpManager::signal_add_auth_friend,this,
            &ContactUserList::slot_add_auth_firend);

    // //链接自己点击同意认证后界面刷新
    connect(TcpManager::GetInstance().get(), &TcpManager::signal_auth_rsp,this,
            &ContactUserList::slot_auth_rsp);
}


void ContactUserList::ShowRedPoint(bool bshow /*= true*/)
{
    _add_friend_item->ShowRedPoint(bshow);
}

// TODO: 模拟
void ContactUserList::addContactUserList()
{
    //获取好友列表

    // 先设置普通item再替换
    // group tip item 1
    auto * groupTip = new GroupTipItem();
    groupTip->SetGroupTip(tr("NEW FRIENDS"));
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(groupTip->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, groupTip);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    // custom item of group tip item 1
    _add_friend_item = new ConUserItem();
    _add_friend_item->setObjectName("new_friend_item");
    _add_friend_item->SetInfo(0,tr("new friends"),":/resources/add_friend.png");
    _add_friend_item->setListItemType(ListItemType::APPLY_FRIEND_ITEM);
    QListWidgetItem *add_item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    add_item->setSizeHint(_add_friend_item->sizeHint());
    this->addItem(add_item);
    this->setItemWidget(add_item, _add_friend_item);
    //默认设置新的朋友申请条目被选中
    this->setCurrentItem(add_item);


    // group tip item 2
    auto * groupCon = new GroupTipItem();
    groupCon->SetGroupTip(tr("CONTACTS"));
    _groupitem = new QListWidgetItem;
    _groupitem->setSizeHint(groupCon->sizeHint());
    this->addItem(_groupitem);
    this->setItemWidget(_groupitem, groupCon);
    _groupitem->setFlags(_groupitem->flags() & ~Qt::ItemIsSelectable);
    // custom items of group tip item 2
    // ⚠️加载后端发送过来的好友列表  逐页加载
    auto con_list = UserinfoGroup::GetInstance()->getConListPerPage();
    for(auto & con_ele : con_list){
        auto *con_user_wid = new ConUserItem();
        con_user_wid->SetInfo(con_ele->_uid,con_ele->_name, con_ele->_icon);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(con_user_wid->sizeHint());
        this->addItem(item);
        this->setItemWidget(item, con_user_wid);
    }
    // 更新_count_loaded计数  更新现在更新到哪里了
    UserinfoGroup::GetInstance()->updateContactLoadedCount();

    // 模拟列表， 创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%con_strs.size();
        int head_i = randomValue%con_heads.size();
        int name_i = randomValue%con_names.size();

        auto *con_user_wid = new ConUserItem();
        con_user_wid->SetInfo(0,con_names[name_i], con_heads[head_i]);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(con_user_wid->sizeHint());
        this->addItem(item);
        this->setItemWidget(item, con_user_wid);
    }
}

bool ContactUserList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport()) {
        if (event->type() == QEvent::Enter) {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y();
        int numSteps = numDegrees / 15; // 计算滚动步数

        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        //int pageSize = 10; // 每页加载的联系人数量

        if (maxScrollValue - currentValue <= 0) {

            auto b_loaded = UserinfoGroup::GetInstance()->isLoadChatFin();
            if(b_loaded){
                return true;
            }

            if(_load_pending){
                return true;
            }

            _load_pending = true;

            QTimer::singleShot(100, [this](){
                _load_pending = false;
                QCoreApplication::quit(); // 完成后退出应用程序
                });
            // 滚动到底部，加载新的联系人
            qDebug()<<"load more contact user";
            //发送信号通知聊天界面加载更多聊天内容
            emit signal_loading_contact_user();
         }

        return true; // 停止事件传递
    }

    return QListWidget::eventFilter(watched, event);

}

/**
 * @brief 根据widget的类型发送不同信号
 */
void ContactUserList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = this->itemWidget(item); // 获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->getListItemType();
    if(itemType == ListItemType::INVALID_ITEM
            || itemType == ListItemType::GROUP_TIP_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }

   if(itemType == ListItemType::APPLY_FRIEND_ITEM){

       // 创建对话框，提示用户
       qDebug()<< "apply friend item clicked ";
       //跳转到好友申请界面
       emit signal_switch_apply_friend_page();
       return;
   }

   if(itemType == ListItemType::CONTACT_USER_ITEM){
       // 创建对话框，提示用户
       qDebug()<< "contact user item clicked ";

       auto con_item = qobject_cast<ConUserItem*>(customItem);
       auto user_info = con_item->GetInfo();
       //跳转到好友申请界面
       emit signal_switch_friend_info_page(user_info);
       return;
   }
}

void ContactUserList::slot_add_auth_firend(std::shared_ptr<AuthInfo> auth_info)
{
    qDebug() << "对方验证通过，添加好友到列表 ";
    bool isFriend = UserinfoGroup::GetInstance()->checkFriendById(auth_info->_uid);
    if(isFriend){
        return;
    }
    // 在 groupitem 之后插入新项
    int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    // ⚠️TODO: 模拟头像和介绍
    int str_i = randomValue%con_strs.size();
    int head_i = randomValue%con_heads.size();

    auto *con_user_wid = new ConUserItem();
    con_user_wid->SetInfo(auth_info);
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(con_user_wid->sizeHint());

    // 获取 groupitem 的索引
    int index = this->row(_groupitem);
    // 在 groupitem 之后插入新项
    this->insertItem(index + 1, item);

    this->setItemWidget(item, con_user_wid);

}

void ContactUserList::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    qDebug() << "同意好友验证，添加好友到列表";
    bool isFriend = UserinfoGroup::GetInstance()->checkFriendById(auth_rsp->_uid);
    if(isFriend){
        return;
    }
    // 在 groupitem 之后插入新项
    int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    int str_i = randomValue%con_strs.size();
    int head_i = randomValue%con_heads.size();

    auto *con_user_wid = new ConUserItem();
    con_user_wid->SetInfo(auth_rsp->_uid ,auth_rsp->_name, con_heads[head_i]);
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(con_user_wid->sizeHint());

    // 获取 groupitem 的索引
    int index = this->row(_groupitem);
    // 在 groupitem 之后插入新项
    this->insertItem(index + 1, item);

    this->setItemWidget(item, con_user_wid);

}


