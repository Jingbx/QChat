/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/c_server.h
 * @date    : 2024-12-11 11:29:41
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : CServer负责连接的建立，通过acceptor来监听连接
 *            单独一个线程用于分发连接到线程池，线程池调取work&ioc建立新的HttpConnection
 * -----
 * Last Modified: 2024-12-20 15:59:22
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#ifndef __CSERVER_H_
#define __CSERVER_H_

#include "../../server_base/src/include/base.h"
#include "asio_iocontext_pool.h"
#include <string>

class CServer : public std::enable_shared_from_this<CServer> {
  public:
    CServer(boost::asio::io_context &ioc, uint16_t &port);
    ~CServer();
    /**
     * @brief 开始监听peer连接
     */
    void start(); // 无拷贝构造
  private:
    boost::asio::io_context &_ioc; // io事件循环 确保在一个loop中，用引用
    tcp::acceptor _acceptor;
};

#endif // _CSERVER_H_