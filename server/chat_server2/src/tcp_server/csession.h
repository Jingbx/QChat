/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/tcp_server/csession.h
 * @date    : 2024-12-21 16:46:22
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : tcp长连接
 * -----
 * Last Modified: 2024-12-31 17:12:29
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#ifndef __CHAT_C_SESSION_H_
#define __CHAT_C_SESSION_H_

#include "../../server_base/src/include/base.h"
#include "../../server_base/src/include/circle_que_lk.h"
#include "msg_node.h"

class CServer;
class LogicSystem;

class CSession : public std::enable_shared_from_this<CSession> {
public:
    CSession(boost::asio::io_context &ioc, CServer *server);
    tcp::socket& getSocket() { return _socket; }
    std::string getSessionId() { return _session_id; }
    
    void setUid(const int & uid) { _uid = uid; }
    int getUid() { return _uid; }
    
    size_t getSendQueSize() { return _send_msg_que.size(); }

    void start();
    void close();
    /**
     * @brief socket.async_read_some封装
     */
    void asyncReadHead(size_t total_len);
    void asyncReadBody(size_t read_len);
    /**
     * @brief socket.async_write封装
     */
	void send(short msg_id, std::string msg);
	void send(short msg_id, short msg_len, char* msg_data);

private:
    std::string _generateUuid();
    /**
     * @brief 对async_read_some的封装
     */
    void _asyncReadFull(size_t total_len, std::function<void(const boost::beast::error_code&, size_t)> handler);
    void _asyncReadSome(size_t begin_len, size_t total_len, std::function<void(const boost::beast::error_code &, size_t)> handler);
    /**
     * @brief async_write()的处理，只有从队列0->1时候会绑定async_write _writeHandler直到处理完
     */
    void _writeHandler(const boost::beast::error_code &ec, std::shared_ptr<CSession> session_self);
    
private:
    tcp::socket     _socket;
    std::string     _session_id;
    int             _uid;
    CServer         *_server;

    /**
     * @brief char*数据存储块
     */
    char _data[MAX_LENGTH]; // async_read_some存储位置
    std::shared_ptr<MsgNode>                                _recv_head_node;
    std::shared_ptr<RecvNode>                               _recv_body_node;
    // Enhanced: 设置send队列大小; 线程安全; 逻辑处理在logic中
    CircleQueLk<std::shared_ptr<SendNode>, MAX_SENDQUE>     _send_msg_que; 
    
    /**
     * @brief 
     */    
    std::atomic<bool>   _b_stop;
       
};


class LogicNode {
friend class LogicSystem;
public:
    LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recvnode)
        : _session(session), _recvnode(recvnode) {}
private:
	std::shared_ptr<CSession> _session;
	std::shared_ptr<RecvNode> _recvnode;
};

#endif  // __CHAT_C_SESSION_H_