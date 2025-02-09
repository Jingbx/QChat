/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/asio_iocontext_pool.cpp
 * @date    : 2024-12-14 10:29:04
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-13 21:14:25
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------
 ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#include "include/asio_iocontext_pool.h"

AsioIOContextPool::AsioIOContextPool(size_t pool_size)
    : _io_contexts(pool_size), _curr_work(0) {
    std::cout << "=== AsioIOContextPool is start ===" << std::endl;
    // 为每个work绑定一个ioc事件循环
    for (int i = 0; i < pool_size; ++i) {
        _io_workptrs.push_back(
            std::make_unique<boost::asio::io_context::work>(_io_contexts[i]));
    }
    // 每个work开辟一个线程
    for (int i = 0; i < _io_workptrs.size(); ++i) {
        _threads.emplace_back([this, i]() {
            this->_io_contexts[i].run(); // run ioc in thread to start this thread
        });
    }
}

AsioIOContextPool::~AsioIOContextPool() {
    std::cout << "=== AsioIOContextPool is stop ===" << std::endl;
    stop();
    // thread guard
    for (auto &thread : _threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

boost::asio::io_context &AsioIOContextPool::getIOContext() {
    assert(!_io_contexts.empty()); // 确保容器非空
    // std::cout << "==== _curr" << _curr_work % _io_contexts.size() << std::endl;
    auto &io_context =  _io_contexts[_curr_work++ % _io_contexts.size()];
    return io_context;
}

void AsioIOContextPool::stop() {
    // 手动停止work中的所有ioc事件循环，reset work
    for (auto &workptr : _io_workptrs) {
        workptr->get_io_context().stop();
        workptr.reset();
    }
}

