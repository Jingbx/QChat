#include "ui_applyfriendpage.h"
#include "applyfriendpage.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include <QRandomGenerator>
#include "applyfrienditem.h"
#include "../custom_ui_wid/apply_friend_list.h"
#include "../network_manager/tcp_manager.h"
#include "../user_manager/userinfo_group.h"
#include "authenfriend.h"
#include "applyfriend.h"


// test===============
std::vector<QString> afp_strs = {"Fuck!", "Fuck you?", "Fuck your mom!", "Fuck your dad?", "Bye!"};
std::vector<QString> afp_heads = {":/resources/head_1.jpg", ":/resources/head_2.jpg", ":/resources/head_3.jpg", ":/resources/head_4.jpg", ":/resources/head_5.jpg"};
std::vector<QString> afp_names = {"AA", "BB", "CC", "DD", "EE"};
// test===============


ApplyFriendPage::ApplyFriendPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ApplyFriendPage)
{
    ui->setupUi(this);
    connect(ui->apply_friend_list, &ApplyFriendList::signal_show_search, this, &ApplyFriendPage::signal_show_search);
    // TODO: ⚠️模拟
    loadApplyList();
    //接受tcp传递的authrsp信号处理
    connect(TcpManager::GetInstance().get(), &TcpManager::signal_auth_rsp, this, &ApplyFriendPage::slot_auth_rsp);
}

ApplyFriendPage::~ApplyFriendPage()
{
    delete ui;
}

void ApplyFriendPage::AddNewApply(std::shared_ptr<AddFriendApply> apply)
{
    //TODO: ⚠️ 先模拟头像随机，以后头像资源增加资源服务器后再显示  --- 涉及文件传输
    int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    int head_i = randomValue % afp_heads.size();
	auto* apply_item = new ApplyFriendItem();
    auto apply_info = std::make_shared<ApplyInfo>(apply->_from_uid,
             apply->_name, apply->_desc,afp_heads[head_i], apply->_name, 0, 0);
    apply_item->SetInfo( apply_info);
	QListWidgetItem* item = new QListWidgetItem;
	//qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
	item->setSizeHint(apply_item->sizeHint());
	item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
	ui->apply_friend_list->insertItem(0,item);
	ui->apply_friend_list->setItemWidget(item, apply_item);
    apply_item->ShowAddBtn(true);
    // TODO: 收到审核好友信号 显示认证界面！
    connect(apply_item, &ApplyFriendItem::signal_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info) {
        auto* authFriend = new AuthenFriend(this);
        authFriend->setModal(true);
        authFriend->SetApplyInfo(apply_info);
        authFriend->show();
        });
}

void ApplyFriendPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ApplyFriendPage::loadApplyList()
{
    //添加好友申请真数据
    auto apply_list = UserinfoGroup::GetInstance()->getApplyList();
    for(auto &apply: apply_list){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int head_i = randomValue % afp_heads.size();
        auto* apply_item = new ApplyFriendItem();
        apply->SetIcon(afp_heads[head_i]);
        apply_item->SetInfo(apply);
        QListWidgetItem* item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui->apply_friend_list->insertItem(0,item);
        ui->apply_friend_list->setItemWidget(item, apply_item);
        if(apply->_status){
            apply_item->ShowAddBtn(false);
        }else{
             apply_item->ShowAddBtn(true);
             auto uid = apply_item->GetUid();
             _unauth_items[uid] = apply_item;
        }

        //收到审核好友信号
        connect(apply_item, &ApplyFriendItem::signal_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info) {
            auto* authFriend = new AuthenFriend(this);
            authFriend->setModal(true);
            authFriend->SetApplyInfo(apply_info);
            authFriend->show();
            });
    }

    // TODO: ⚠️模拟假数据，创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%afp_strs.size();
        int head_i = randomValue%afp_heads.size();
        int name_i = randomValue%afp_names.size();

        auto *apply_item = new ApplyFriendItem();
        auto apply = std::make_shared<ApplyInfo>(0, afp_names[name_i], afp_strs[str_i],
                                    afp_heads[head_i], afp_names[name_i], 0, 1);
        apply_item->SetInfo(apply);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui->apply_friend_list->addItem(item);
        ui->apply_friend_list->setItemWidget(item, apply_item);
        //收到审核好友信号
        connect(apply_item, &ApplyFriendItem::signal_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info){
            auto *authFriend =  new AuthenFriend(this);
            authFriend->setModal(true);
            authFriend->SetApplyInfo(apply_info);
            authFriend->show();
        });
    }
}

void ApplyFriendPage::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp) {
    auto uid = auth_rsp->_uid;
    auto find_iter = _unauth_items.find(uid);
    if (find_iter == _unauth_items.end()) {
        return;
    }

    find_iter->second->ShowAddBtn(false);
}



