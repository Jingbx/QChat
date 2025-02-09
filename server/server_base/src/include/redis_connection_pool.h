/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/src_base/redis_connection_pool.h
 * @date    : 2024-12-15 11:24:05
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : redis连接池，redis单线程，一次性创建多个连接提高并发能力
 * -----
 * Last Modified: 2025-01-07 13:47:52
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * TODO: 1.7                         补充心跳机制给Redis
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/
#include "base.h"
#include "circle_que_lk.h"
#include <hiredis/hiredis.h>
#include <atomic>
#include <condition_variable>

class RedisConnectionPool {
public:
    RedisConnectionPool(size_t pool_size, const char *host, uint16_t port, const char *pwd);
    ~RedisConnectionPool();
    
    redisContext *getConnection();
    void recycleConnection(redisContext *context);
    void close();
    
private:
    size_t                      _pool_size;
    const char*                 _host;
    int                         _port;
    
    
    CircleQueLk<redisContext *, 10> _conns_que;
    
    std::condition_variable     _cv;
    std::mutex                  _mtx;
    std::atomic<bool>           _b_stop;
};

