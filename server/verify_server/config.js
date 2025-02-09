/*
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/verify_server/config.js
 * @date    : 2024-12-13 15:28:07
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : 读取config.json的配置
 * -----
 * Last Modified: 2024-12-15 21:27:25
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

const fs = require('node:fs'); // 明确调用 Node.js 内置模块 // 导入文件库

// parse json
let config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
// get params
let email_user = config.email.user
let email_pass = config.email.pass

let mysql_host = config.mysql.host;
let mysql_port = config.mysql.port;
let mysql_passwd = config.mysql.passwd;

let redis_host = config.redis.host;
let redis_port = config.redis.port;
let redis_passwd = config.redis.passwd;

let vcode_wait = config.vericode.wait;
let vcode_wait_label = config.vericode.wait_lable;
let vcode_expired = config.vericode.expired;

let code_prefix = "code_";


module.exports = {
    email_user, email_pass,
    mysql_host, mysql_port, mysql_passwd,
    redis_host, redis_port, redis_passwd,
    vcode_wait, vcode_wait_label, vcode_expired, 
    code_prefix
}