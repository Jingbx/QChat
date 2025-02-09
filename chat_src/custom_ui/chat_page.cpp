#include "chat_page.h"
#include "ui_chatpage.h"
#include "../custom_ui_wid/ChatItemBase.h"
#include "../bubble/TextBubble.h"
#include "../bubble/PictureBubble.h"
#include "../custom_ui_wid/MessageTextEdit.h"
#include "../bubble/VideoBubble.h"
#include <QStyleOption>
#include "../custom_ui_wid/clicked_btn.h"
#include "../custom_ui_wid/clicked_label.h"
#include "../user_manager/userinfo_group.h"
#include "../network_manager/tcp_manager.h"
ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);
    //设置按钮样式
    ui->receive_btn->setState("normal","hover","press");
    ui->send_btn->setState("normal","hover","press");

    //设置图标样式
    ui->emo_lb->setState("normal","hover","press","normal","hover","press");
    ui->file_lb->setState("normal","hover","press","normal","hover","press");

    // 连接enter发送
    connect(ui->chatEdit, &MessageTextEdit::send, this, &ChatPage::slot_send_enter_clicked);
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::resetChatPage(const QString &username) {
    qDebug() << "当前聊天人: " << username;
    //  重制title
    ui->title_lb->setText(username);
    // 清空当前页聊天记录
    ui->chat_data_list->removeAllItem();
    // 清空输入框
    ui->chatEdit->clearMessages();
    repolish(this);
    update();
}

void ChatPage::setUserInfo(std::shared_ptr<UserInfo> user_info)
{
    qDebug() << "ChatPage setUserInfo";
    _user_info = user_info;
    //设置ui界面
    ui->title_lb->setText(_user_info->_name);
    clearItems();
    for(auto & msg : user_info->_chat_msgs){
        appendChatMsg(msg);
    }
}

void ChatPage::appendChatMsg(std::shared_ptr<TextChatData> msg)
{
    auto self_uid = UserinfoGroup::GetInstance()->getCurrUid();
    auto self_info = UserinfoGroup::GetInstance()->getUserInfo(self_uid);
    ChatRole role;
    //todo... 添加聊天显示
    if (msg->_from_uid == self_info->uid) {
        role = ChatRole::Self;
        ChatItemBase* pChatItem = new ChatItemBase(role);

        pChatItem->setUserName(self_info->user);
        pChatItem->setUserIcon(QPixmap(self_info->icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
    else {
        role = ChatRole::Other;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        auto friend_info = UserinfoGroup::GetInstance()->getFriendById(msg->_from_uid);
        if (friend_info == nullptr) {
            return;
        }
        pChatItem->setUserName(friend_info->_name);
        pChatItem->setUserIcon(QPixmap(friend_info->_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }


}



void ChatPage::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatPage::on_send_btn_clicked()
{
    if (_user_info == nullptr) {
        qDebug() << "friend_info is empty";
        return;
    }

    auto user_uid = UserinfoGroup::GetInstance()->getCurrUid();
    auto user_info = UserinfoGroup::GetInstance()->getUserInfo(user_uid);
    auto pTextEdit = ui->chatEdit;
    ChatRole role = ChatRole::Self;
    QString userName = user_info->user;
    QString userIcon = user_info->icon;

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    QJsonObject textObj;
    QJsonArray textArray;
    int txt_size = 0;

    for(int i=0; i<msgList.size(); ++i)
    {
        //消息内容长度不合规就跳过
        if(msgList[i].content.length() > 1024){
            continue;
        }

        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;
        // 文本消息
        if(type == "text")
        {
            //生成唯一id
            QUuid uuid = QUuid::createUuid();
            //转为字符串
            QString uuidString = uuid.toString();

            pBubble = new TextBubble(role, msgList[i].content);
            if(txt_size + msgList[i].content.length() > 1024){
                textObj["fromuid"] = user_info->uid;
                textObj["touid"] = _user_info->_uid;
                textObj["text_array"] = textArray;
                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
                //发送并清空之前累计的文本列表
                txt_size = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
                //发送tcp请求给chat server
                emit TcpManager::GetInstance()->signal_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ,
                                                                 jsonData);
            }

            //将bubble和uid绑定，以后可以等网络返回消息后设置是否送达
            //_bubble_map[uuidString] = pBubble;

            // 累积
            txt_size += msgList[i].content.length();
            QJsonObject obj;
            QByteArray utf8Message = msgList[i].content.toUtf8();
            obj["content"] = QString::fromUtf8(utf8Message);
            obj["msgid"] = uuidString;
            textArray.append(obj);
            auto txt_msg = std::make_shared<TextChatData>(uuidString, obj["content"].toString(),
                                                          user_info->uid, _user_info->_uid);
            emit signal_append_send_chat_msg(txt_msg);
        }
        else if(type == "image")
        {
            pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
        }
        else if (type == "video") {
            pBubble = new VideoBubble(msgList[i].content, role);
        }
        else if(type == "file")
        {

        }
        //发送消息
        if(pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }

    }

    qDebug() << "textArray is " << textArray ;
    //发送给服务器
    textObj["text_array"] = textArray;
    textObj["fromuid"] = user_info->uid;
    textObj["touid"] = _user_info->_uid;
    QJsonDocument doc(textObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    //发送并清空之前累计的文本列表
    txt_size = 0;
    textArray = QJsonArray();
    textObj = QJsonObject();
    //发送tcp请求给chat server
    emit TcpManager::GetInstance()->signal_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ,
                                                  jsonData);
}


/****************************/
void ChatPage::slot_send_enter_clicked()
{
    if (_user_info == nullptr) {
        qDebug() << "friend_info is empty";
        return;
    }
    auto user_uid = UserinfoGroup::GetInstance()->getCurrUid();
    auto user_info = UserinfoGroup::GetInstance()->getUserInfo(user_uid);


    auto pTextEdit = ui->chatEdit;
    ChatRole role = ChatRole::Self;
    QString userName = user_info->user;
    QString userIcon = user_info->icon;

    // 从 pTextEdit 获取所有的消息内容（MsgInfo 列表）
    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    // textObj 和 textArray：存储消息的 JSON 数据结构，将用于组合多条消息以发送给聊天服务器。
    QJsonObject textObj;
    QJsonArray textArray;
    int txt_size = 0;

    for(int i=0; i<msgList.size(); ++i)
    {
        //消息内容长度不合规就跳过
        if(msgList[i].content.length() > 1024){
            continue;
        }

        QString type = msgList[i].msgFlag;
        qDebug() << type;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;

        if(type == "text")
        {
            //生成唯一id
            QUuid uuid = QUuid::createUuid();
            //转为字符串
            QString uuidString = uuid.toString();

            pBubble = new TextBubble(role, msgList[i].content);
            if(txt_size + msgList[i].content.length()> 1024){
                textObj["fromuid"] = user_info->uid;
                textObj["touid"] = _user_info->_uid;
                textObj["text_array"] = textArray;
                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
                //发送并清空之前累计的文本列表
                txt_size = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
                //发送tcp请求给chat server
                emit TcpManager::GetInstance()->signal_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ,
                                                                 jsonData);
            }

            //将bubble和uid绑定，以后可以等网络返回消息后设置是否送达
            //_bubble_map[uuidString] = pBubble;
            txt_size += msgList[i].content.length();
            QJsonObject obj;
            QByteArray utf8Message = msgList[i].content.toUtf8();
            obj["content"] = QString::fromUtf8(utf8Message);
            obj["msgid"] = uuidString;
            textArray.append(obj);
            // chat page显示消息
            auto txt_msg = std::make_shared<TextChatData>(uuidString, obj["content"].toString(),
                                                          user_info->uid, _user_info->_uid);
            emit signal_append_send_chat_msg(txt_msg);
        }
        else if(type == "image")
        {
            pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
        }
        else if (type == "video") {
            pBubble = new VideoBubble(msgList[i].content, role);
        }
        else if(type == "file")
        {

        }
        //发送消息
        if(pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }

    }

    qDebug() << "textArray is " << textArray ;
    // ⚠️: 便利完发送列表之后，剩下不足1024的消息最后发送给服务器
    textObj["text_array"] = textArray;
    textObj["fromuid"] = user_info->uid;
    textObj["touid"] = _user_info->_uid;
    QJsonDocument doc(textObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    //发送并清空之前累计的文本列表
    txt_size = 0;
    textArray = QJsonArray();
    textObj = QJsonObject();
    //发送tcp请求给chat server
    emit TcpManager::GetInstance()->signal_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
}


void ChatPage::clearItems()
{
    ui->chat_data_list->removeAllItem();
}
