/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/redis_manager.cpp
 * @date    : 2024-12-14 22:07:10
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-20 10:32:09
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#include "redis_manager.h"
#include "config/conf.h"

extern GeneralConf *gate_config;

RedisManager::RedisManager() {
    _conns_pool.reset(new RedisConnectionPool(
        gate_config->redis_conn_pool_size, gate_config->redis_host.c_str(),
        gate_config->redis_port, gate_config->redis_pass.c_str()));
}

bool RedisManager::Get(const std::string &key, std::string &value) {
    // get context (connection)
    auto context = _conns_pool->getConnection();
    if (!context)
        return false;
    // do command
    auto reply = (redisReply *)redisCommand(context, "GET %s", key.c_str());
    if (!reply) {
        std::cout << "Execut command [ GET " << key << "  " << value
                  << " ] failure ! " << std::endl;
        _conns_pool->recycleConnection(context);
        return false;
    }
    if (reply->type != REDIS_REPLY_STRING) {
        std::cout << "Execut command [ GET " << key << "  " << value
                  << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _conns_pool->recycleConnection(context);
        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
    _conns_pool->recycleConnection(context);
    std::cout << "Execut command [ GET " << key << "  " << value
              << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::Set(const std::string &key, const std::string &value) {
    auto context = _conns_pool->getConnection();
    if (context == nullptr) {
        return false;
    }
    auto reply = (redisReply *)redisCommand(context, "SET %s %s", key.c_str(),
                                            value.c_str());

    if (!reply) {
        std::cout << "Execut command [ SET " << key << "  " << value
                  << " ] failure ! " << std::endl;
        _conns_pool->recycleConnection(context);
        return false;
    }

    // 如果执行失败则释放连接
    if (!(reply->type == REDIS_REPLY_STATUS &&
          (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0))) {
        std::cout << "Execut command [ SET " << key << "  " << value
                  << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _conns_pool->recycleConnection(context);
        return false;
    }

    // 执行成功 释放redisCommand执行后返回的redisReply所占用的内存
    freeReplyObject(reply);
    std::cout << "Execut command [ SET " << key << "  " << value
              << " ] success ! " << std::endl;
    _conns_pool->recycleConnection(context);
    return true;
}

bool RedisManager::LPush(const std::string &key, const std::string &value) {
    auto context = _conns_pool->getConnection();
    if (context == nullptr) {
        return false;
    }
    auto reply = (redisReply *)redisCommand(context, "LPUSH %s %s", key.c_str(),
                                            value.c_str());
    if (!reply) {
        std::cout << "Execut command [ LPUSH " << key << "  " << value
                  << " ] failure ! " << std::endl;
        _conns_pool->recycleConnection(context);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Execut command [ LPUSH " << key << "  " << value
                  << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _conns_pool->recycleConnection(context);
        return false;
    }
    freeReplyObject(reply);
    _conns_pool->recycleConnection(context);
    std::cout << "Execut command [ LPUSH " << key << "  " << value
              << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::LPop(const std::string &key, std::string &value) {
    auto context = _conns_pool->getConnection();
    if (context == nullptr) {
        return false;
    }
    auto reply = (redisReply *)redisCommand(context, "LPOP %s %s", key.c_str());
    if (!reply) {
        std::cout << "Execut command [ LPOP " << key << "  " << value
                  << " ] failure ! " << std::endl;
        _conns_pool->recycleConnection(context);
        return false;
    }
    if (reply->type != REDIS_REPLY_NIL) {
        std::cout << "Execut command [ LPOP " << key << "  " << value
                  << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _conns_pool->recycleConnection(context);
        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
    _conns_pool->recycleConnection(context);
    std::cout << "Execut command [ LPOP " << key << "  " << value
              << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::RPush(const std::string &key, const std::string &value) {
    auto context = _conns_pool->getConnection();
    if (context == nullptr) {
        return false;
    }
    auto reply = (redisReply *)redisCommand(context, "RPUSH %s %s", key.c_str(),
                                            value.c_str());
    if (!reply) {
        std::cout << "Execut command [ RPUSH " << key << "  " << value
                  << " ] failure ! " << std::endl;
        _conns_pool->recycleConnection(context);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Execut command [ RPUSH " << key << "  " << value
                  << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _conns_pool->recycleConnection(context);
        return false;
    }
    freeReplyObject(reply);
    _conns_pool->recycleConnection(context);
    std::cout << "Execut command [ RPUSH " << key << "  " << value
              << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::RPop(const std::string &key, std::string &value) {
    auto context = _conns_pool->getConnection();
    if (context == nullptr) {
        return false;
    }
    auto reply = (redisReply *)redisCommand(context, "RPOP %s %s", key.c_str());
    if (!reply) {
        std::cout << "Execut command [ RPOP " << key << "  " << value
                  << " ] failure ! " << std::endl;
        _conns_pool->recycleConnection(context);
        return false;
    }
    if (reply->type != REDIS_REPLY_NIL) {
        std::cout << "Execut command [ RPOP " << key << "  " << value
                  << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _conns_pool->recycleConnection(context);
        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
    _conns_pool->recycleConnection(context);
    std::cout << "Execut command [ RPOP " << key << "  " << value
              << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::HSet(const std::string &key, const std::string &hkey,
                        const std::string &value) {
    auto context = _conns_pool->getConnection();
    if (context == nullptr) {
        return false;
    }
    auto reply = (redisReply *)redisCommand(
        context, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
    if (!reply) {
        std::cout << "Execut command [ HSET " << key << "  " << value
                  << " ] failure ! " << std::endl;
        _conns_pool->recycleConnection(context);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ HSET " << key << "  " << value
                  << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _conns_pool->recycleConnection(context);
        return false;
    }
    freeReplyObject(reply);
    _conns_pool->recycleConnection(context);
    std::cout << "Execut command [ HSET " << key << "  " << value
              << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::HSet(const char *key, const char *hkey, const char *hvalue,
                        size_t hvaluelen) {
    auto context = _conns_pool->getConnection();
    if (context == nullptr) {
        return false;
    }

    const char *argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;

    auto reply = (redisReply *)redisCommandArgv(context, 4, argv, argvlen);
    if (!reply) {
        std::cout << "Execut command [ HSet " << key << "  " << hkey << "  "
                  << hvalue << " ] failure ! " << std::endl;
        _conns_pool->recycleConnection(context);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ HSet " << key << "  " << hkey << "  "
                  << hvalue << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _conns_pool->recycleConnection(context);
        return false;
    }
    std::cout << "Execut command [ HSet " << key << "  " << hkey << "  "
              << hvalue << " ] success ! " << std::endl;
    freeReplyObject(reply);
    _conns_pool->recycleConnection(context);
    return true;
}

bool RedisManager::HGet(const std::string &key, const std::string &hkey,
                        std::string &hvalue) {
    auto context = _conns_pool->getConnection();
    if (context == nullptr) {
        hvalue = "";
        return false;
    }

    const char *argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();

    auto reply = (redisReply *)redisCommandArgv(context, 3, argv, argvlen);
    if (!reply) {
        std::cout << "Execut command [ HGet " << key << " " << hkey
                  << "  ] failure ! " << std::endl;
        hvalue = "";
        _conns_pool->recycleConnection(context);
        return false;
    }
    if (reply->type != REDIS_REPLY_STRING) {
        std::cout << "Execut command [ HGet " << key << " " << hkey
                  << "  ] failure ! " << std::endl;
        hvalue = "";
        freeReplyObject(reply);
        _conns_pool->recycleConnection(context);
        return false;
    }
    hvalue = reply->str;
    freeReplyObject(reply);
    _conns_pool->recycleConnection(context);
    std::cout << "Execut command [ HGet " << key << " " << hkey
              << "  ] success ! " << std::endl;
    return true;
}

bool RedisManager::Del(const std::string &key) {
    auto context = _conns_pool->getConnection();
    if (context == nullptr) {
        return false;
    }
    auto reply = (redisReply *)redisCommand(context, "DEL %s", key.c_str());
    if (reply == nullptr) {
        std::cout << "Execut command [ Del " << key << " ] failure ! "
                  << std::endl;
        _conns_pool->recycleConnection(context);
        return false;
    }

    if (reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ Del " << key << " ] failure ! "
                  << std::endl;
        freeReplyObject(reply);
        _conns_pool->recycleConnection(context);
        return false;
    }
    freeReplyObject(reply);
    _conns_pool->recycleConnection(context);
    std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::HDel(const std::string &key, const std::string &field) {
    auto context = _conns_pool->getConnection();
    if (context == nullptr) {
        return false;
    }

    // TODO:
    // Defer defer(
    //     [&connect, this]() { _conns_pool->recycleConnection(context); });

    redisReply *reply = (redisReply *)redisCommand(context, "HDEL %s %s",
                                                   key.c_str(), field.c_str());
    if (reply == nullptr) {
        std::cerr << "HDEL command failed" << std::endl;
        return false;
    }

    bool success = false;
    if (reply->type == REDIS_REPLY_INTEGER) {
        success = reply->integer > 0;
    }

    freeReplyObject(reply);
    return success;
}

bool RedisManager::ExistsKey(const std::string &key) {
    auto context = _conns_pool->getConnection();
    if (context == nullptr) {
        return false;
    }

    auto reply = (redisReply *)redisCommand(context, "exists %s", key.c_str());
    if (reply == nullptr) {
        std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
        _conns_pool->recycleConnection(context);
        return false;
    }

    if (reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
        std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
        _conns_pool->recycleConnection(context);
        freeReplyObject(reply);
        return false;
    }
    std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
    freeReplyObject(reply);
    _conns_pool->recycleConnection(context);
    return true;
}
