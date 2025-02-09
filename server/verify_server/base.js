/*
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/verify_server/base.js
 * @date    : 2024-12-13 15:25:00
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : base设置
 * -----
 * Last Modified: 2024-12-16 11:18:44
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

let code_prefix = "code_";
let code_need_wait = "need_wait_";

/**
 * @breif grpc后端服务内部错误
 */
const Errors = {
    SUCCESS                 : 0,
    ERROR_REDIS             : 1,
    ERROR_EXCEPTION         : 2,
    ERROR_TOOMANYREQ        : 101
};



module.exports = {code_prefix, code_need_wait, Errors};