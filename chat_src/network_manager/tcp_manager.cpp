/*
 * @File        tcp_manager.cpp
 * @brief
 *
 * @Author      Jingbx
 * @Data        2024/12/21
 * @History
 */
#include "tcp_manager.h"
#include "../user_manager/userinfo_group.h"
#include <QDataStream>
#include "../user_manager/user_data.h"


TcpManager::TcpManager() {
    // 不使用shared_from_this, 因为没构造完?
    // connect connection
    QObject::connect(&_socket, &QTcpSocket::connected, [&] () {
        qDebug() << "Tcp connected!";
        emit signal_conn_success(true);
    });
    // connect read
    QObject::connect(&_socket, &QTcpSocket::readyRead, [&] () {
        // 读头后根据len读数据直到满足大小后，将读取的msg body数据给_handle_msg处理
        _buffer.append(_socket.readAll());
        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_6_7);
        while (true) {
            // parse head
            if (!_b_recv_pending) {
                if (_buffer.size() < TCP_HEAD_LEN) {
                    return;
                }
                stream >> _msg_id >> _msg_len;
                _buffer =_buffer.mid(TCP_HEAD_LEN); // TCP_HEAD_LEN前的被截断
            }
            // parse body
            if (_buffer.size() < _msg_len) {
                _b_recv_pending = true;
                return;
            }
            // 取出msg body并处理
            _b_recv_pending = false;
            QByteArray  msg_body = _buffer.mid(0, _msg_len);
            _buffer = _buffer.mid(_msg_len);
            qDebug() << "Receive msg: <id:" << _msg_id
                     << "> <len:" << _msg_len
                     << "> <body:" << msg_body << ">";
            _handleMsg(ReqId(_msg_id), _msg_len, msg_body);
        }
    });
    // connect send 需要传入id和body需要slot而不是捕获
    QObject::connect(this, &TcpManager::signal_send_data, this, &TcpManager::slot_send_data);
    // connect error
    QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
                     [&](QAbstractSocket::SocketError socketError) {
                         Q_UNUSED(socketError)
                         qDebug() << "Error:" << _socket.errorString();
                    });
    // connect deconnection
    QObject::connect(&_socket, &QTcpSocket::disconnected, [&] () {
        qDebug() << "Tcp disconnected!";
    });
    // 注册Handlers
    _initHandlers();
}

TcpManager::~TcpManager() {

}

void TcpManager::_initHandlers() {
    // 1.ID_CHAT_LOGIN_RSP
    _handlers.insert(ReqId::ID_CHAT_LOGIN_RSP, [this] (ReqId id, quint16 len, QByteArray data) {
        // 反序列化
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 解析判断错误
        if (jsonDoc.isNull()) {
            qDebug() << "Json deserialization error!";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Chat login error, err: " << err;
            emit signal_login_failed(err);
            return;
        }
        int err = jsonObj["error"].toInt();
        // TODO: backenderr 判断
        if (BackendErrorCodes::BE_ERR_UID_INVALID == err) {
            qDebug() << "Chat login: UID INVALID, err: " << err;
            emit signal_login_failed(err);
            return;
        }
        if (BackendErrorCodes::BE_ERR_TOKEN_NOT_MATCH == err) {
            qDebug() << "Chat login: TOKEN NOT MATCH, err: " << err;
            emit signal_login_failed(err);
            return;
        }
        //
        if (ErrorCodes::SUCCESS != err) {
            qDebug() << "Chat login error, err: " << err;
            emit signal_login_failed(err);
            return;
        }
        qDebug() << "Handle response for msg: <id=" << id << "> <msg len=" << len << ">";
        // 更改界面并单例类存储用户信息
        // TODO: 存储用户信息 ✅
        auto uid = jsonObj["uid"].toInt();
        auto user = jsonObj["user"].toString();
        auto email = jsonObj["email"].toString();
        auto token = jsonObj["token"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        UserinfoGroup::GetInstance()->addUserInfo(uid, user, email, token, nick, icon, sex);
        // TODO: 添加好友信息
        if(jsonObj.contains("apply_list")){
            qDebug() << "从数据库中获取申请列表";
            UserinfoGroup::GetInstance()->appendApplyList(jsonObj["apply_list"].toArray());
        } else {
            qDebug() << "从数据库中获取申请列表失败";
        }
        // TODO: 好友信息
        if (jsonObj.contains("friend_list")) {
            qDebug() << "从数据库中获取好友列表";
            UserinfoGroup::GetInstance()->appendFriendList(jsonObj["friend_list"].toArray());
        } else {
            qDebug() << "从数据库中获取好友列表失败";
        }
        emit signal_switch_chatdlg();
    });

    // 2. ID_SEARCH_USER_REQ  查找要添加的用户是否存在
    _handlers.insert(ReqId::ID_SEARCH_USER_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Search user Failed, err is Json Parse Err" << err;

            emit this->signal_user_search(nullptr);
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "查找失败 用户并非QChat用户, err is " << err;
            emit this->signal_user_search(nullptr);
            return;
        }

        auto search_info =  std::make_shared<SearchInfo>(jsonObj["uid"].toInt(), jsonObj["name"].toString(),
                                                        jsonObj["nick"].toString(), jsonObj["desc"].toString(),
                                                        jsonObj["sex"].toInt(), jsonObj["icon"].toString());

        emit this->signal_user_search(search_info);
    });

    // 3. ID_NOTIFY_ADD_FRIEND_REQ  添加好友被动方, 发送好友后通知对方
    _handlers.insert(ReqId::ID_NOTIFY_ADD_FRIEND_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Add friend Failed, err is Json Parse Err" << err;
            emit this->signal_user_search(nullptr);
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "添加好友失败收到req error, err is " << err;
            emit signal_user_search(nullptr);
            return;
        }
        // 将返回的对端信息打包，用于本端前端显示
        auto from_uid = jsonObj["applyuid"].toInt();
        auto name = jsonObj["name"].toString();
        auto desc = jsonObj["desc"].toString();
        auto icon = jsonObj["icon"].toString();
        auto nick = jsonObj["nick"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto apply_friend_info = std::make_shared<AddFriendApply>(from_uid, name, desc, icon, nick, sex);

        // 通知好友申请，等待认证和界面显示
        qDebug() << "收到添加好友通知，对方昵称: " << from_uid;
        emit signal_friend_apply(apply_friend_info);
    });

    // 4. ID_ADD_FRIEND_RSP  添加好友主动方，发送好友后自己得到的rsp
    _handlers.insert(ReqId::ID_ADD_FRIEND_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "ID_ADD_FRIEND_RSP handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Add friend Failed, err is Json Parse Err" << err;

            emit this->signal_user_search(nullptr);
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "添加好友失败收到rsp error, err is " << err;
            return;
        }

        qDebug() << "添加好友请求发送成功，等待认证..." ;
    });

    // 5. ID_NOTIFY_AUTH_FRIEND_REQ 认证好友被动接收方(认证端)
    _handlers.insert(ID_NOTIFY_AUTH_FRIEND_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "ID_NOTIFY_AUTH_FRIEND_REQ handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Auth Friend Failed0, err is " << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth Friend Failed1, err is " << err;
            return;
        }

        int from_uid = jsonObj["fromuid"].toInt();
        QString name = jsonObj["name"].toString();
        QString nick = jsonObj["nick"].toString();
        QString icon = jsonObj["icon"].toString();
        int sex = jsonObj["sex"].toInt();

        auto auth_info = std::make_shared<AuthInfo>(from_uid,name,
                                                    nick, icon, sex);
        qDebug() << "对方已认证！" ;
        emit signal_add_auth_friend(auth_info);
    });
    // 6. ID_AUTH_FRIEND_RSP认证好友发起主动方(认证回复端)
    _handlers.insert(ID_AUTH_FRIEND_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "ID_AUTH_FRIEND_RSP handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Auth Friend Failed2, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth Friend Failed3, err is " << err;
            return;
        }

        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto uid = jsonObj["uid"].toInt();
        auto rsp = std::make_shared<AuthRsp>(uid, name, nick, icon, sex);
        emit signal_auth_rsp(rsp);

        qDebug() << "好友认证成功" ;
    });

    // 7.ID_TEXT_CHAT_MSG_RSP
    _handlers.insert(ID_TEXT_CHAT_MSG_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "ID_TEXT_CHAT_MSG_RSP handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Chat Msg Rsp Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Chat Msg Rsp Failed, err is " << err;
            return;
        }

        qDebug() << "消息发送成功 " ;
        //ui设置送达等标记 todo...
    });

    // 8.ID_NOTIFY_TEXT_CHAT_MSG_REQ
    _handlers.insert(ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "ID_NOTIFY_TEXT_CHAT_MSG_REQ handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Notify Chat Msg Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Notify Chat Msg Failed, err is " << err;
            return;
        }

        qDebug() << "成功收到对方消息 ";
        auto msg_ptr = std::make_shared<TextChatMsg>(jsonObj["fromuid"].toInt(),
                                                     jsonObj["touid"].toInt(),jsonObj["text_array"].toArray());
        emit signal_text_chat_msg(msg_ptr);
    });
}

void TcpManager::_handleMsg(ReqId id, int len, QByteArray data) {
    if (_handlers.find(id) == _handlers.end()) {
        qDebug() << "No handler to match <" << id << ">";
        return;
    }
    _handlers[id](id, len, data);
}

/**
 * @brief Slots
 */
void TcpManager::slot_connect_tcp(ServerInfo si) {
    // user login slot建立连接
    qDebug() << "Create tcp connection...";
    _socket.connectToHost(si.host, static_cast<quint16>(si.port.toInt()));
    // qDebug() << "Connection error:" << _socket.errorString();
}

void TcpManager::slot_send_data(ReqId reqId, QByteArray databytes) {

    uint16_t id = static_cast<uint16_t>(reqId);
    quint16 len = static_cast<quint16>(databytes.size());  // 确保长度符合 uint16_t 范围


    QByteArray block;
    QDataStream out_stream(&block, QIODevice::WriteOnly);
    out_stream.setByteOrder(QDataStream::BigEndian);  // 设置字节序为 BigEndian


    out_stream << id << len;
    qDebug() << "Send msg_id: " << id << ", msg_len: " << len;
    // 将消息体追加到数据块
    block.append(databytes);

    // 将数据发送到套接字
    qint64 bytesWritten = _socket.write(block);
    if (bytesWritten == -1) {
        qDebug() << "Failed to send data:" << _socket.errorString();
    } else {
        qDebug() << "Sent data successfully:" << bytesWritten << "bytes";
    }
}
