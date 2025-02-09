/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/tcp_server/cserver.h
 * @date    : 2024-12-21 16:45:16
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : 总监听类，负责新建存储session; 成员函数就是函数+类地址
 * -----
 * Last Modified: 2024-12-22 21:53:25
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#ifndef __CHAT_C_SERVER_H_
#define __CHAT_C_SERVER_H_
#include "../../server_base/src/include/base.h"
#include "../../server_base/src/include/asio_iocontext_pool.h"

class CSession;
class CServer : public std::enable_shared_from_this<CServer> {
public:
    CServer(boost::asio::io_context &ioc, uint16_t port);
    ~CServer();
    /**
     * @brief 从session删除某个特定session
     */
    void dropSession(const std::string &session_id);
private:
    void _start();
    void _handleSession(std::shared_ptr<CSession> session, const boost::beast::error_code &err_code);

private:
    tcp::acceptor   _acceptor;

    boost::asio::io_context &_ioc;
    uint16_t                _port;
    
    std::mutex                                                 _mtx;
    std::unordered_map<std::string, std::shared_ptr<CSession>> _sessions;
};


#endif  // __CHAT_C_SERVER_H_