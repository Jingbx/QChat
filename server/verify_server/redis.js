/*
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/verify_server/redis.js
 * @date    : 2024-12-15 16:12:35
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : js ioredis接口封装
 * -----
 * Last Modified: 2024-12-15 21:39:35
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

const Redis = require("ioredis")
const config_module = require("./config")

/**
 * @breif 创建redis客户端实例
 */
const RedisCli = new Redis({
    host: config_module.redis_host,
    port: config_module.redis_port,
    password: config_module.redis_passwd
});

/**
 * @breif 客户端监听err
 */
RedisCli.on("error", function (err) {
    console.log("RedisCli connect error");
    RedisCli.quit();
});

/**
 * @breif GET
 */
async function GetRedis(key) {
    try {
        const value = await RedisCli.get(key); // 同步获取键值
        if (value === null || value === undefined || value === "") { // 明确检查 null/空字符串等
            console.log(`Get <${key}>: Key not found or empty`);
            return null; // 表示键不存在或没有意义的值
        }
        console.log(`Get <${key}>: ${value}`); // 日志更清晰地展示值
        return value;
    } catch (error) {
        console.log('Get key error: ', error);
        return null;
    }
}

/**
 * @breif isExited
 */
async function QueryRedis(key) {
    try {
        const result = await RedisCli.exists(key);
        if (result === null) {
            console.log(key, ' is cannot be found');
            return null;
        }
        console.log(key, ' is exist');
        return result;
    } catch (error) {
        console.log('QueryRedis is error: ', error);
        return null;
    }
}

/**
 * @breif set kv and expire time 设置键值对到期时间
 */
async function SetRedisWithExpireTime(key, value, exptime) {
    try {
        // 设置键和值
        await RedisCli.set(key, value)
        // 设置过期时间（以秒为单位）
        await RedisCli.expire(key, exptime);
        return true;
    } catch (error) {
        console.log('SetRedisExpire error is', error);
        return false;
    }
}

async function ClearRedis(pattern = '*') {
    try {
        if (pattern === '*') {
            // 清空整个 Redis 数据库
            await RedisCli.flushdb();
            console.log('All Redis keys have been cleared.');
            return true;
        }

        // 获取匹配 pattern 的键列表
        const keys = await RedisCli.keys(pattern);
        if (keys.length === 0) {
            console.log(`No Redis keys matched pattern: ${pattern}`);
            return true; // 没有匹配的键也是正常情况
        }

        // 批量删除匹配的键
        const delResult = await RedisCli.del(...keys);
        console.log(`Cleared ${delResult} Redis keys matching pattern: ${pattern}`);
        return true;
    } catch (error) {
        console.log('ClearRedis encountered an error:', error);
        return false;
    }
}

module.exports = { GetRedis, QueryRedis, SetRedisWithExpireTime, ClearRedis };