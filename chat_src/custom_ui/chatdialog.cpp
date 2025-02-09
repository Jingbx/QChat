#include <QAction>
#include <QDebug>
#include <vector>
#include <QRandomGenerator>
#include "../base/global.h"
#include "../custom_ui_wid/clicked_btn.h"
#include "chatdialog.h"
#include "chat_user_wid.h"
#include "ui_chatdialog.h"  // 确保包含这个头文件
#include "../custom_ui_wid/state_widget.h"
#include "../custom_ui_wid/search_list.h"
#include "../custom_ui_wid/chat_user_list.h"
#include "../custom_ui_wid/contact_user_list.h"
#include "../user_manager/userinfo_group.h"
#include "../custom_ui/conuser_item.h"
#include "../custom_ui/applyfriendpage.h"
#include "../network_manager/tcp_manager.h"
#include "../custom_ui/friendinfopage.h"

ChatDialog::ChatDialog(QWidget *parent) :
                                        QDialog(parent), ui(new Ui::ChatDialog),
                                        _b_loading(false),
                                        _mode(ChatUIMode::ChatMode), _state(ChatUIMode::ChatMode),
                                        _last_widget(ui->empty_page) {
    ui->setupUi(this);
    // 设置空白页为起始界面
    ui->stackedWidget->setCurrentWidget(ui->empty_page);
    // 添加好友按钮
    ui->add_btn->setState("normal","hover","press");
    ui->add_btn->setProperty("state","normal");
    // customize edit搜索框
    ui->search_edit->setMaxLenght(30);
    QAction *searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/resources/search.png"));
    ui->search_edit->addAction(searchAction,QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("search..."));

    QAction *cleanAction = new QAction(ui->search_edit);
    cleanAction->setIcon(QIcon(":/resources/close_transparent.png"));   // 默认透明
    ui->search_edit->addAction(cleanAction, QLineEdit::TrailingPosition);
    connect(ui->search_edit, &QLineEdit::textChanged, [cleanAction](const QString &text) {
        if (!text.isEmpty()) {
            cleanAction->setIcon(QIcon(":/resources/close_search.png"));    // 显示
        } else {
            cleanAction->setIcon(QIcon(":/resources/close_transparent.png"));    // 无文本输入 透明
        }
    }) ;
    connect(cleanAction, &QAction::triggered, [this, cleanAction] () {          // 连接清除动作
        ui->search_edit->clear();
        cleanAction->setIcon(QIcon(":/resources/close_transparent.png"));
        ui->search_edit->clearFocus();
        showSearchList(false);                          // ⚠️: 如果清除搜索框了就不显示search list
    });
    showSearchList(false);                              // default
    // 连接最近聊天记录chat_user_list
    connect(ui->chat_user_list, &ChatUserList::signal_loading_chat_user,
            this, &ChatDialog::slot_loading_chat_user);
    // TODO: ⚠️模拟添加用户列表
    addChatUserList();

    //聊天页面点击好友申请条目的信号   改
    // connect(ui->chat_user_list, &ChatUserList::signal_switch_chat_page,
    //         this,&ChatDialog::slot_switch_chat_page);
    connect(ui->chat_user_list, &QListWidget::itemClicked, this, &ChatDialog::slot_item_clicked);

    //侧边栏模拟加载自己头像 TODO: 从登录后的服务器获取
    QString head_icon = UserinfoGroup::GetInstance()->getIcon();
    // QString head_icon = ":/resources/Joy.png";
    QPixmap pixmap(head_icon); // 加载图片
    QPixmap scaledPixmap = pixmap.scaled(ui->side_head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); // 将图片缩放到label的大小
    ui->side_head_lb->setPixmap(scaledPixmap); // 将缩放后的图片设置到QLabel上
    ui->side_head_lb->setScaledContents(true); // 设置QLabel自动缩放图片内容以适应大小
    ui->side_chat_lb->setProperty("state","normal");
    ui->side_chat_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    ui->side_contact_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    // ⚠️添加侧边栏按钮到列表，connect确认当前被选中和之前被选中一致，不一致就清空上次被选中的，确保只有一个侧边栏按钮被选中
    addLBGroup(ui->side_chat_lb);
    addLBGroup(ui->side_contact_lb);
    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);

    //链接搜索框输入变化
    connect(ui->search_edit, &QLineEdit::textChanged, this, &ChatDialog::slot_text_changed);
    //检测鼠标点击位置判断是否要清空搜索框
    this->installEventFilter(this); // 安装事件过滤器

    //设置聊天label选中状态
    ui->side_chat_lb->SetSelected(true);

    //设置选中条目
    SetSelectChatItem();
    //更新聊天界面信息
    SetSelectChatPage();

    //连接加载联系人的信号和槽函数
    connect(ui->con_user_list, &ContactUserList::signal_loading_contact_user,
            this, &ChatDialog::slot_loading_contact_user);

    //连接联系人页面点击好友申请条目的信号
    connect(ui->con_user_list, &ContactUserList::signal_switch_apply_friend_page,
            this,&ChatDialog::slot_switch_apply_friend_page);

    //连接清除搜索框操作
    connect(ui->friend_apply_page, &ApplyFriendPage::signal_show_search, this, &ChatDialog::slot_show_search);

    //为searchlist 设置search edit
    ui->search_list->SetSearchEdit(ui->search_edit);

    //连接申请添加好友信号
    connect(TcpManager::GetInstance().get(), &TcpManager::signal_friend_apply, this, &ChatDialog::slot_apply_friend);

    //⚠️通过验证后需要在Chat聊天也添加item: 连接认证添加好友信号
    connect(TcpManager::GetInstance().get(), &TcpManager::signal_add_auth_friend,
            this, &ChatDialog::slot_add_auth_friend);

    //⚠️通过验证后需要在Chat聊天也添加item: 链接自己认证回复信号
    connect(TcpManager::GetInstance().get(), &TcpManager::signal_auth_rsp,
            this, &ChatDialog::slot_auth_rsp);

    // 连接点击联系人item发出的信号和用户信息展示槽函数
    connect(ui->con_user_list, &ContactUserList::signal_switch_friend_info_page,
            this,&ChatDialog::slot_friend_info_page);

    // //设置中心部件为chatpage
    // ui->stackedWidget->setCurrentWidget(ui->chat_page);

// 聊天
    //连接聊天列表点击信号
    connect(ui->chat_user_list, &QListWidget::itemClicked, this, &ChatDialog::slot_item_clicked);

    //连接searchlist跳转聊天界面
    connect(ui->search_list, &SearchList::signal_jump_chat_item, this,
                             &ChatDialog::slot_jump_chat_item);

    //连接好友信息界面发送的点击事件
    connect(ui->friend_info_page, &FriendInfoPage::signal_jump_chat_item, this,
            &ChatDialog::slot_jump_chat_item_from_infopage);

    //连接对端消息通知
    connect(TcpManager::GetInstance().get(), &TcpManager::signal_text_chat_msg,
            this, &ChatDialog::slot_text_chat_msg);

    connect(ui->chat_page, &ChatPage::signal_append_send_chat_msg,
            this, &ChatDialog::slot_append_send_chat_msg);
}

ChatDialog::~ChatDialog() {

}

/**
 * @brief 只允许search列表部分的单击
 */
bool ChatDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        handleGlobalMousePress(mouseEvent);
    }
    return QDialog::eventFilter(watched, event);
}

void ChatDialog::handleGlobalMousePress(QMouseEvent *event)
{
    // 实现点击位置的判断和处理逻辑
    // 先判断是否处于搜索模式，如果不处于搜索模式则直接返回
    if( _mode != ChatUIMode::SearchMode){
        return;
    }

    // 将鼠标点击位置转换为搜索列表坐标系中的位置
    QPoint posInSearchList = ui->search_list->mapFromGlobal(event->globalPos());
    // 判断点击位置是否在聊天列表的范围内
    if (!ui->search_list->rect().contains(posInSearchList)) {
        // 如果不在聊天列表内，清空输入框
        ui->search_edit->clear();
        showSearchList(false);
    }
}

// test===============
std::vector<QString> strs = {"Hello!", "How are you?", "Good Morning!", "What's up?", "Bye!"};
std::vector<QString> heads = {":/resources/head_1.jpg", ":/resources/head_2.jpg", ":/resources/head_3.jpg", ":/resources/head_4.jpg", ":/resources/head_5.jpg"};
std::vector<QString> names = {"CC", "Bob", "Charlie", "Diana", "Eve"};
// test===============
void ChatDialog::addChatUserList() {
    //先按照好友列表加载聊天记录，等以后客户端实现聊天记录数据库之后再按照最后信息排序
    auto friend_list = UserinfoGroup::GetInstance()->getChatListPerPage();
    if (friend_list.empty() == false) {
        for(auto & friend_ele : friend_list){
            auto find_iter = _chat_items_added.find(friend_ele->_uid);
            if(find_iter != _chat_items_added.end()){
                continue;
            }
            auto *chat_user_wid = new ChatUserWid();
            auto user_info = std::make_shared<UserInfo>(friend_ele);
            chat_user_wid->setInfo(user_info);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->chat_user_list->addItem(item);
            ui->chat_user_list->setItemWidget(item, chat_user_wid);
            _chat_items_added.insert(friend_ele->_uid, item);
        }

        //更新已加载条目
        UserinfoGroup::GetInstance()->updateChatLoadedCount();
    }

    //模拟测试条目
    // 创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();

        auto *chat_user_wid = new ChatUserWid();
        auto user_info = std::make_shared<UserInfo>(0,names[name_i],
                                                    names[name_i],heads[head_i],0,strs[str_i]);
        chat_user_wid->setInfo(user_info);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
    }
}


void ChatDialog::showSearchList(bool bsearch) {
    // 搜索模式
    if (bsearch) {
        qDebug() << "显示搜索列表";
        ui->chat_user_list->hide();
        ui->con_user_list->hide();
        ui->search_list->show();
        _mode = ChatUIMode::SearchMode;
        _state = ChatUIMode::SearchMode;
    }
    // 聊天模式
    if (_state == ChatUIMode::ChatMode) {
        qDebug() << "显示聊天列表";
        ui->con_user_list->hide();
        ui->search_list->hide();
        ui->chat_user_list->show();
        _mode = ChatUIMode::ChatMode;
        _before_state = _mode;
    }
    // 联系人模式
    if (_state == ChatUIMode::ContactMode) {
        qDebug() << "显示联系人列表";
        ui->search_list->hide();
        ui->chat_user_list->hide();
        ui->con_user_list->show();
        _mode = ChatUIMode::ContactMode;
        _before_state = _mode;
    }
}

void ChatDialog::slot_loading_chat_user() {
    if (_b_loading) {
        return;
    }
    _b_loading = true;
    LoadingDialog *loading_dlg = new LoadingDialog(this);
    loading_dlg->setModal(true);
    loading_dlg->show();
    qDebug() << "add new data to list.....";
    // 真实返回数据
    loadMoreConUser();
    loading_dlg->deleteLater();
    _b_loading = false;
}

/**
 * @brief Core 确保只有一个侧边脸是被选中的
 */
// void ChatDialog::closeFindDlg()
// {
//     ui->search_list->closeFindDlg();
// }

void ChatDialog::addLBGroup(StateWidget* lb)
{
    _lb_list.push_back(lb);
}

void ChatDialog::clearLabelState(StateWidget *lb)
{
    for(auto & ele: _lb_list){
        if(ele == lb){
            continue;
        }

        ele->ClearState();
    }
}
// 改
void ChatDialog::slot_switch_chat_page(const QString &username) {
    qDebug() << "切换到聊天页";
    _last_widget = ui->chat_page;
    // 设置title_lb
    ui->chat_page->resetChatPage(username);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
}

void ChatDialog::slot_side_chat()
{
    qDebug()<< "receive side chat clicked";
    clearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    _state = ChatUIMode::ChatMode;
    showSearchList(false);
}

void ChatDialog::slot_side_contact(){
    qDebug()<< "receive side contact clicked";
    clearLabelState(ui->side_contact_lb);
    //设置
    ui->stackedWidget->setCurrentWidget(ui->empty_page);
    _state = ChatUIMode::ContactMode;
    showSearchList(false);
}

void ChatDialog::slot_text_changed(const QString &str)
{
    //qDebug()<< "receive slot text changed str is " << str;
    if (!str.isEmpty()) {
        showSearchList(true);
    } else {
        _state = _before_state;
        showSearchList(false);
    }
}


void ChatDialog::loadMoreConUser()
{
    auto friend_list = UserinfoGroup::GetInstance()->getConListPerPage();
    if (friend_list.empty() == false) {
        for(auto & friend_ele : friend_list){
            auto *chat_user_wid = new ConUserItem();
            chat_user_wid->SetInfo(friend_ele->_uid,friend_ele->_name,
                                   friend_ele->_icon);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->con_user_list->addItem(item);
            ui->con_user_list->setItemWidget(item, chat_user_wid);
        }

        //更新已加载条目
        UserinfoGroup::GetInstance()->updateContactLoadedCount();
    }
}
void ChatDialog::slot_loading_contact_user() {
    qDebug() << "加载更多联系人";
    if(_b_loading){
        return;
    }

    _b_loading = true;
    LoadingDialog *loadingDialog = new LoadingDialog(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list.....";
    loadMoreConUser();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();

    _b_loading = false;
}


void ChatDialog::slot_switch_apply_friend_page() {
    qDebug() << "跳转添加好友页面";
    _last_widget = ui->friend_apply_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
}

void ChatDialog::slot_show_search(bool show) {
    showSearchList(show);
}

void ChatDialog::slot_apply_friend(std::shared_ptr<AddFriendApply> apply_info) {
    qDebug() << "收到新好友申请, 来自用户<" << apply_info->_name << ">!";
    bool b_already = UserinfoGroup::GetInstance()->alreadyApply(apply_info->_from_uid);
    if(b_already){
        qDebug() << "已经收到该好友的请求，不再继续显示了";
        return;
    }
    // 不是好友添加到申请列表中
    UserinfoGroup::GetInstance()->addApplyList(std::make_shared<ApplyInfo>(apply_info));
    // 红点
    ui->side_contact_lb->ShowRedPoint(true);
    ui->con_user_list->ShowRedPoint(true);
    // 添加到联系人列表
    ui->friend_apply_page->AddNewApply(apply_info);
}


void ChatDialog::slot_friend_info_page(std::shared_ptr<UserInfo> user_info) {
    qDebug()<<"跳转好友信息页面";
    _last_widget = ui->friend_info_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_info_page);
    ui->friend_info_page->SetInfo(user_info);
}



void ChatDialog::slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info)
{
    // 添加并显示联系人
    qDebug() << "receive slot_add_auth__friend uid is " << auth_info->_uid
             << " name is " << auth_info->_name << " nick is " << auth_info->_nick;

    //判断如果已经是好友则跳过
    auto bfriend = UserinfoGroup::GetInstance()->checkFriendById(auth_info->_uid);
    if(bfriend){
        return;
    }

    UserinfoGroup::GetInstance()->addFriend(auth_info);

    int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    int str_i = randomValue % strs.size();
    int head_i = randomValue % heads.size();
    int name_i = randomValue % names.size();

    auto* chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(auth_info);
    chat_user_wid->setInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(auth_info->_uid, item);
}

void ChatDialog::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    // 添加并显示联系人
    qDebug() << "receive slot_auth_rsp uid is " << auth_rsp->_uid
             << " name is " << auth_rsp->_name << " nick is " << auth_rsp->_nick;

    //判断如果已经是好友则跳过
    auto bfriend = UserinfoGroup::GetInstance()->checkFriendById(auth_rsp->_uid);
    if(bfriend){
        return;
    }

    UserinfoGroup::GetInstance()->addFriend(auth_rsp);
    int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    int str_i = randomValue % strs.size();
    int head_i = randomValue % heads.size();
    int name_i = randomValue % names.size();

    auto* chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(auth_rsp);
    chat_user_wid->setInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(auth_rsp->_uid, item);
}


//
void ChatDialog::SetSelectChatItem(int uid) {
    if(ui->chat_user_list->count() <= 0){
        return;
    }

    if(uid == 0){
        ui->chat_user_list->setCurrentRow(0);
        QListWidgetItem *firstItem = ui->chat_user_list->item(0);
        if(!firstItem){
            return;
        }

        //转为widget
        QWidget *widget = ui->chat_user_list->itemWidget(firstItem);
        if(!widget){
            return;
        }

        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if(!con_item){
            return;
        }

        _cur_chat_uid = con_item->getUserInfo()->_uid;

        return;
    }

    auto find_iter = _chat_items_added.find(uid);
    if(find_iter == _chat_items_added.end()){
        qDebug() << "uid " <<uid<< " not found, set curent row 0";
        ui->chat_user_list->setCurrentRow(0);
        return;
    }

    ui->chat_user_list->setCurrentItem(find_iter.value());

    _cur_chat_uid = uid;
}

void ChatDialog::SetSelectChatPage(int uid) {
    if( ui->chat_user_list->count() <= 0){
        return;
    }

    if (uid == 0) {
        auto item = ui->chat_user_list->item(0);
        //转为widget
        QWidget* widget = ui->chat_user_list->itemWidget(item);
        if (!widget) {
            return;
        }

        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if (!con_item) {
            return;
        }

        //设置信息
        auto user_info = con_item->getUserInfo();
        ui->chat_page->setUserInfo(user_info);
        return;
    }

    auto find_iter = _chat_items_added.find(uid);
    if(find_iter == _chat_items_added.end()){
        return;
    }

    //转为widget
    QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
    if(!widget){
        return;
    }

    //判断转化为自定义的widget
    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "qobject_cast<ListItemBase*>(widget) is nullptr";
        return;
    }

    auto itemType = customItem->getListItemType();
    if(itemType == CHAT_USER_ITEM){
        auto con_item = qobject_cast<ChatUserWid*>(customItem);
        if(!con_item){
            return;
        }

        //设置信息
        auto user_info = con_item->getUserInfo();
        ui->chat_page->setUserInfo(user_info);

        return;
    }
}


// 点击聊天列表聊天
void ChatDialog::slot_item_clicked(QListWidgetItem *item)
{
    qDebug() << "点击跳转聊天列表";
    QWidget *widget = ui->chat_user_list->itemWidget(item); // 获取自定义widget对象
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


    if(itemType == ListItemType::CHAT_USER_ITEM){
        // 创建对话框，提示用户
        qDebug()<< "contact user item clicked ";

        auto chat_wid = qobject_cast<ChatUserWid*>(customItem);
        auto user_info = chat_wid->getUserInfo();
        //跳转到聊天界面
        ui->chat_page->setUserInfo(user_info);  // 加载聊天对象信息以及内存中的聊天记录
        _cur_chat_uid = user_info->_uid;

        ui->stackedWidget->setCurrentWidget(ui->chat_page);
        _last_widget = ui->chat_page;
        return;
    }
}

// 搜索聊天
void ChatDialog::slot_jump_chat_item(std::shared_ptr<SearchInfo> si)
{
    qDebug() << "搜索跳转聊天列表";
    auto find_iter = _chat_items_added.find(si->_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "跳转至 chat item , uid is " << si->_uid;
                                                           ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_lb->SetSelected(true);
        SetSelectChatItem(si->_uid);
        //更新聊天界面信息
        SetSelectChatPage(si->_uid);
        slot_side_chat();
        return;
    }

    //如果没找到，则创建新的插入listwidget

    auto* chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(si);
    chat_user_wid->setInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);

    _chat_items_added.insert(si->_uid, item);

    ui->side_chat_lb->SetSelected(true);
    SetSelectChatItem(si->_uid);
    //更新聊天界面信息
    SetSelectChatPage(si->_uid);
    slot_side_chat();

}

// 好友详细界面点击聊天
void ChatDialog::slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info)
{
    qDebug() << "好友详细界面跳转聊天列表";
    auto find_iter = _chat_items_added.find(user_info->_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "jump to chat item , uid is " << user_info->_uid;
        ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_lb->SetSelected(true);
        SetSelectChatItem(user_info->_uid);
        //更新聊天界面信息
        SetSelectChatPage(user_info->_uid);
        slot_side_chat();
        return;
    }

    //如果没找到，则创建新的插入listwidget

    auto* chat_user_wid = new ChatUserWid();
    chat_user_wid->setInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);

    _chat_items_added.insert(user_info->_uid, item);

    ui->side_chat_lb->SetSelected(true);
    SetSelectChatItem(user_info->_uid);
    //更新聊天界面信息
    SetSelectChatPage(user_info->_uid);
    slot_side_chat();
}



// 聊天信息显示
void ChatDialog::updateChatMsg(std::vector<std::shared_ptr<TextChatData> > msgdata)
{
    for(auto & msg : msgdata){
        if(msg->_from_uid != _cur_chat_uid){
            break;
        }

        ui->chat_page->appendChatMsg(msg);
    }
}
// 收到信息
void ChatDialog::slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg)
{
    qDebug() << "收到对方消息回调";
    auto find_iter = _chat_items_added.find(msg->_from_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "set chat item msg, uid is " << msg->_from_uid;
        QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
        auto chat_wid = qobject_cast<ChatUserWid*>(widget);
        if(!chat_wid){
            qDebug() << "错误";
            return;
        }
        chat_wid->updateLastMsg(msg->_chat_msgs);
        //更新当前聊天页面记录
        updateChatMsg(msg->_chat_msgs);
        UserinfoGroup::GetInstance()->appendFriendChatMsg(msg->_from_uid,msg->_chat_msgs);
        return;
    }

    //如果没找到，则创建新的插入listwidget

    auto* chat_user_wid = new ChatUserWid();
    //查询好友信息
    auto fi_ptr = UserinfoGroup::GetInstance()->getFriendById(msg->_from_uid);
    chat_user_wid->setInfo(fi_ptr);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    chat_user_wid->updateLastMsg(msg->_chat_msgs);
    UserinfoGroup::GetInstance()->appendFriendChatMsg(msg->_from_uid,msg->_chat_msgs);
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(msg->_from_uid, item);
}

// 发送信息
void ChatDialog::slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata)
{
    if (_cur_chat_uid == 0) {
        return;
    }

    auto find_iter = _chat_items_added.find(_cur_chat_uid);
    if (find_iter == _chat_items_added.end()) {
        return;
    }

    //转为widget
    QWidget* widget = ui->chat_user_list->itemWidget(find_iter.value());
    if (!widget) {
        return;
    }

    //判断转化为自定义的widget
    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if (!customItem) {
        qDebug() << "qobject_cast<ListItemBase*>(widget) is nullptr";
        return;
    }

    auto itemType = customItem->getListItemType();
    if (itemType == CHAT_USER_ITEM) {
        auto con_item = qobject_cast<ChatUserWid*>(customItem);
        if (!con_item) {
            return;
        }

        //设置信息
        auto user_info = con_item->getUserInfo();
        user_info->_chat_msgs.push_back(msgdata);
        std::vector<std::shared_ptr<TextChatData>> msg_vec;
        msg_vec.push_back(msgdata);
        UserinfoGroup::GetInstance()->appendFriendChatMsg(_cur_chat_uid,msg_vec);
        return;
    }
}
