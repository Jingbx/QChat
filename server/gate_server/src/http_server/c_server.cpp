/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/c_server.cpp
 * @date    : 2024-12-11 11:31:25
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-21 19:21:16
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/



#include "c_server.h"
#include "http_connection.h"

CServer::CServer(boost::asio::io_context &ioc, uint16_t &port) : 
                _ioc(ioc), 
                _acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {
    
}

CServer::~CServer() {
    
}

void CServer::start() {
    auto self = shared_from_this();
    // 初始化io线程池1次
    auto& io_context = AsioIOContextPool::GetInstance()->getIOContext();    //只会初始化一次
    std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);
    // 监听 try catch处理peer关闭的异常
    try {
        _acceptor.async_accept(new_con->getSocket(), [self, new_con] (boost::beast::error_code err_code) {
            // 错误: 放弃本次连接 持续事件循环
            if (err_code) {
                self->start();
                return;
            }

            // 正确: 封装新链接统一处理 TODO: 加入队列
            new_con->start();
			//继续监听
			self->start();
        });
    } catch (const std::exception &e) {
        std::cout << "CServer accept http connection err, " << e.what() << std::endl;
        self->start();
    }
}

