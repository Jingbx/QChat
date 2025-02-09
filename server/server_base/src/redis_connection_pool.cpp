/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/src_base/redis_connection_pool.cpp
 * @date    : 2024-12-15 11:24:09
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-23 13:58:55
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#include "include/redis_connection_pool.h"

RedisConnectionPool::RedisConnectionPool(size_t pool_size, const char *host,
                                         uint16_t port, const char *pwd)
    : _pool_size(pool_size), _host(host), _port(port), _b_stop(false) {
    for (size_t i = 0; i < _pool_size; ++i) {
        // connect
        auto *context = redisConnect(_host, _port);
        if (!context || context->err != 0) {
            if (context == nullptr) redisFree(context);
            continue;
        }
        // AUTH注册
        auto reply = (redisReply *)redisCommand(context, "AUTH %s", pwd);
        if (reply->type == REDIS_REPLY_ERROR) {
            std::cout << "Redis authentication error" << std::endl;
            freeReplyObject(reply);
            redisFree(context);
            continue;
        }
        // 存储连接context
        freeReplyObject(reply);
        _conns_que.push(context);
        // std::cout << "Redis authentication successful" << std::endl;
    }
}

RedisConnectionPool::~RedisConnectionPool() {
    close();
    while (!_conns_que.empty()) {
        _conns_que.pop();
    }
}

redisContext *RedisConnectionPool::getConnection() {
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _cv.wait(lock, [this] () {
            if (this->_b_stop) {
                return true;
            }
            return !this->_conns_que.empty();
        });
    }
    if (_b_stop) {
        return nullptr;
    }
    return _conns_que.pop_return();
}

void RedisConnectionPool::recycleConnection(redisContext *context) {
    if (_b_stop) {
        return;
    }
    _conns_que.push(context);
    _cv.notify_one();
}

void RedisConnectionPool::close() {
    _b_stop = true;
    _cv.notify_all();
}