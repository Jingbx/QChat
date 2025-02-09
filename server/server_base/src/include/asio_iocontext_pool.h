/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/asio_iocontext_pool.h
 * @date    : 2024-12-14 10:29:02
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * 单例模式线程池，每个work绑定ioc使其当无事件监听时ioc不会自动退出因为和work绑定会持续监听事件
 *            每个线程一个work，类似xrtc
 * -----
 * Last Modified: 2024-12-22 21:38:05
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------
 ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#ifndef _ASIO_IOCONTEXT_POOL_H_
#define _ASIO_IOCONTEXT_POOL_H_

#include "base.h"
#include <atomic>
#include <condition_variable>
#include <thread>

class AsioIOContextPool : public SingleTon<AsioIOContextPool> {
friend class SingleTon<AsioIOContextPool>;

public:
    ~AsioIOContextPool(); // thread guard
    AsioIOContextPool(const AsioIOContextPool &) = delete;
    AsioIOContextPool &operator=(const AsioIOContextPool &io_pool) = delete;
    // 轮询
    boost::asio::io_context &getIOContext();

    void stop();

private:
    AsioIOContextPool(size_t pool_size = 2); //std::thread::hardware_concurrency()

private:
    std::vector<boost::asio::io_context>                        _io_contexts;
    std::vector<std::unique_ptr<boost::asio::io_context::work>> _io_workptrs;
    std::vector<std::thread>                                    _threads;
    std::atomic<size_t>                                         _curr_work;
};

#endif  // __ASIO_IOCONTEXT_POOL_H_
