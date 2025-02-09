/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/redis_manager.h
 * @date    : 2024
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : redis库操作封装，每次操作从池子里取一个连接用完归还，提高并发
 * -----
 * Last Modified: 2024-12-20 15:59:04
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#ifndef __REDIS_MANNAGER_H_
#define __REDIS_MANNAGER_H_

#include "../../server_base/src/include/base.h"
#include "../../server_base/src/include/redis_connection_pool.h"

class RedisManager : public SingleTon<RedisManager>,
                     public std::enable_shared_from_this<RedisManager> {
    friend class SingleTon<RedisManager>;

  public:
    bool Get(const std::string &key, std::string &value);
    bool Set(const std::string &key, const std::string &value);
    bool LPush(const std::string &key, const std::string &value);
    bool LPop(const std::string &key, std::string &value);
    bool RPush(const std::string &key, const std::string &value);
    bool RPop(const std::string &key, std::string &value);
    /**
     * @brief 二级key website(key1)  google(key2) (...value); jingbx(key2)
     * (...value)... redisCommand(HSET...) 方便处理一些字符串
     * redisCommandArgv(HSET...) 方便处理二进制数据
     */
    bool HSet(const std::string &key, const std::string &hkey,
              const std::string &value);
    bool HSet(const char *key, const char *hkey, const char *hvalue,
              size_t hvaluelen);
    bool HGet(const std::string &key, const std::string &hkey,
              std::string &hvalue);
    bool Del(const std::string &key);
    bool HDel(const std::string &key, const std::string &field);
    bool ExistsKey(const std::string &key);

  private:
    RedisManager();

  private:
    std::unique_ptr<RedisConnectionPool> _conns_pool;
};

#endif // __REDIS_MANNAGER_H_