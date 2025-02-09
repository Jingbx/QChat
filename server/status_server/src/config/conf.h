/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/status_server/src/config/conf.h
 * @date    : 2024-12-20 09:49:15
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-20 14:06:46
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#ifndef __STATUS_SERVER_CONF_H_
#define __STATUS_SERVER_CONF_H_

#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

struct GeneralConf {
    std::string         status_server_host;
    uint16_t            status_server_port;
    size_t              grpc_conn_pool_size;
    size_t              mysql_conn_pool_size;
    std::string         mysql_host;
    std::string         mysql_port;
    std::string         mysql_user;
    std::string         mysql_pass;
    std::string         mysql_schema;
    std::string         redis_host;
    uint16_t            redis_port;
    std::string         redis_pass;
    size_t              redis_conn_pool_size;
};

bool load_generate_conf(const std::string &conf_path, GeneralConf *conf);

#endif // __STATUS_SERVER_CONF_H_