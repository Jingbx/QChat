/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/src_base/grpc_connection_pool.tpp
 * @date    : 2024-12-19 19:16:34
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-08 14:26:20
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#ifndef __SERVER_GRPC_CONNECTION_POOL_TPP_
#define __SERVER_GRPC_CONNECTION_POOL_TPP_

#include "include/grpc_connection_pool.h"

template <typename GrpcServer, typename GrpcServerStub>
RPCConnectionPool<GrpcServer, GrpcServerStub>::RPCConnectionPool(size_t pool_size, std::string host, std::string port)
    : _pool_size(pool_size), _host(host), _port(port), _b_stop(false) {
    // 初始化连接池
    for (size_t i = 0; i < _pool_size; ++i) {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
        _conns_que.push(GrpcServer::NewStub(channel)); // important: 此处走的是右值版本push 使用 NewStub 创建连接
    }
}

template <typename GrpcServer, typename GrpcServerStub>
RPCConnectionPool<GrpcServer, GrpcServerStub>::~RPCConnectionPool() {
    stop();
    while (!_conns_que.empty()) {
        _conns_que.pop();
    }
}

template <typename GrpcServer, typename GrpcServerStub>
std::unique_ptr<GrpcServerStub> RPCConnectionPool<GrpcServer, GrpcServerStub>::getConnection() {
    std::unique_lock<std::mutex> lock(_mtx);
    _cv.wait(lock, [this] () {
        return _b_stop || !_conns_que.empty(); // 如果停止或队列不为空，继续执行
    });

    if (_b_stop) {
        std::cout << "RPCConectionPool has been closed" << std::endl;
        return nullptr;
    }

    return _conns_que.pop_return();  // 返回从队列中弹出的连接 important: 右值返回不会报错
}

template <typename GrpcServer, typename GrpcServerStub>
void RPCConnectionPool<GrpcServer, GrpcServerStub>::recycleConnection(std::unique_ptr<GrpcServerStub> conn) {
    if (_b_stop) {
        return;
    }
    _conns_que.push(std::move(conn));  // 将连接放回池中
    _cv.notify_one();  // 唤醒一个等待的线程
}

template <typename GrpcServer, typename GrpcServerStub>
void RPCConnectionPool<GrpcServer, GrpcServerStub>::stop() {
    _b_stop = true;
    _cv.notify_all();  // 唤醒所有等待的线程
}

#endif  // __SERVER_GRPC_CONNECTION_POOL_TPP_