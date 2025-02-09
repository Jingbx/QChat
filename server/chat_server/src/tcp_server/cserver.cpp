/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/tcp_server/cserver.cpp
 * @date    : 2024-12-21 16:45:22
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-31 17:11:09
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#include "cserver.h"
#include "csession.h"
#include "user_mgr/user_manager.h"

CServer::CServer(boost::asio::io_context &ioc, uint16_t port)
    : _ioc(ioc), _port(port), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {
    std::cout << "=== CServer start on port " << _port << " ===" << std::endl;
    _start();
}
CServer::~CServer() {
    std::cout << "=== CServer destruct on port " << _port << " ===" << std::endl;
}

void CServer::dropSession(const std::string &session_id) {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_sessions.empty()) {
        return;
    }
    if (_sessions.find(session_id) == _sessions.end()) {
        return;
    }
    // todo: 踢人 移除UserManager中保存的uid session_id映射关系
    UserManager::GetInstance()->rmvUserSession(_sessions[session_id]->getUid());
    _sessions.erase(session_id);
}

void CServer::_start() {
    // auto self = shared_from_this();
    auto &io_context = AsioIOContextPool::GetInstance()->getIOContext();
    std::shared_ptr<CSession> new_session = std::make_shared<CSession>(io_context, this);
    // note: 
    _acceptor.async_accept(new_session->getSocket(), std::bind(&CServer::_handleSession, this, new_session, std::placeholders::_1));
}

void CServer::_handleSession(std::shared_ptr<CSession> session,
                             const boost::beast::error_code &err_code) {
    
    if (!err_code) {
        session->start();   // 读head和body
        std::lock_guard<std::mutex> lock(_mtx);
        _sessions.insert(std::make_pair(session->getSessionId(), session));
    } else {
		std::cout << "Failed to handle seesion, error is " << err_code.what() << std::endl;
	}
    // note: 继续监听
    _start();        
}   

