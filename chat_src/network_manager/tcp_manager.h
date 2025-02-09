/*
 * @File        tcp_manager.h
 * @brief
 *
 * @Author      Jingbx
 * @Data        2024/12/20
 * @History
 */

#ifndef TCP_MANAGER_H
#define TCP_MANAGER_H

#include "../base/global.h"
#include "../base/singleton.h"
#include "../user_manager/user_data.h"

class TcpManager : public QObject,
                   public Singleton<TcpManager>,
                   public std::enable_shared_from_this<TcpManager> {
    Q_OBJECT
friend class Singleton<TcpManager>;
public:
    ~TcpManager();
private:
    TcpManager();
    void _initHandlers();           // 类似Http的logic请求处理，此处是TCP请求
    void _handleMsg(ReqId id, int len, QByteArray data);
private:
    QTcpSocket  _socket;
    bool        _b_recv_pending;   // 是否收到完整规定长度的数据包,正在读一个包true, 读完了该下一个了false

    QString     _host;
    uint16_t    _port;

    quint16     _msg_id;
    quint16     _msg_len;
    QByteArray  _buffer;

    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> _handlers;

signals:
    void signal_conn_success(bool is_success);
    void signal_send_data(ReqId reqId, QByteArray data);
    void signal_login_failed(int err);
    void signal_switch_chatdlg();
    void signal_user_search(std::shared_ptr<SearchInfo> si);                  // TODO: 搜索发起信号
    void signal_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);                  // 服务器验证回包
    void signal_friend_apply(std::shared_ptr<AddFriendApply>);
    void signal_add_auth_friend(std::shared_ptr<AuthInfo>);
    void signal_text_chat_msg(std::shared_ptr<TextChatMsg> msg);
public slots:
    void slot_connect_tcp(ServerInfo si);
    void slot_send_data(ReqId reqId, QByteArray data);
};

#endif // TCP_MANAGER_H
