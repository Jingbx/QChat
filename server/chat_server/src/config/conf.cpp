/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/config/conf.cpp
 * @date    : 2024-12-20 09:49:20
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-08 15:59:45
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/


#include "conf.h"
bool load_generate_conf(const std::string &conf_path, GeneralConf *conf) {
    if (conf_path.empty()) {
        std::cout << "Config path is empty" << std::endl;
        return false;
    }
    if (!conf) {
        std::cout << "CeneralConf is empty" << std::endl;
        return false;
    }

    // default
    conf->chat_self_server_name = "selfserver";
    conf->chat_self_server_host = "0.0.0.0";
    conf->chat_self_server_port = 8090;
    conf->chat_self_server_rpcport = 50055;
    conf->status_server_host = "127.0.0.1";
    conf->status_server_port = 50052;
    conf->grpc_conn_pool_size = 5;
    conf->mysql_conn_pool_size = 5;
    conf->mysql_host = "10.193.2.225"; // 127.0.0.1
    conf->mysql_port = "3308";
    conf->mysql_user = "root";
    conf->mysql_pass = "2882"; // Djl123456
    conf->mysql_schema = "jbxmysql";
    conf->redis_conn_pool_size = 5;
    conf->redis_host = "10.193.2.225";
    conf->redis_port = 6379;
    conf->redis_pass = "2882";
    // parse yaml
    YAML::Node config = YAML::LoadFile(conf_path);
    try {
        conf->chat_self_server_name = config["selfserver"]["name"].as<std::string>();
        conf->chat_self_server_host = config["selfserver"]["host"].as<std::string>();
        conf->chat_self_server_port = config["selfserver"]["port"].as<uint16_t>();
        conf->chat_self_server_rpcport = config["selfserver"]["rpc_port"].as<uint16_t>();
        conf->status_server_host =
            config["StatusServer"]["host"].as<std::string>();
        conf->status_server_port =
            config["StatusServer"]["port"].as<uint16_t>();
        conf->grpc_conn_pool_size = config["Grpc"]["pool_size"].as<size_t>();
        conf->mysql_conn_pool_size = config["Mysql"]["pool_size"].as<size_t>();
        conf->mysql_host = config["Mysql"]["host"].as<std::string>();
        conf->mysql_port = config["Mysql"]["port"].as<std::string>();
        conf->mysql_user = config["Mysql"]["user"].as<std::string>();
        conf->mysql_pass = config["Mysql"]["pass"].as<std::string>();
        conf->mysql_schema = config["Mysql"]["schema"].as<std::string>();
        conf->redis_conn_pool_size = config["Redis"]["pool_size"].as<size_t>();
        conf->redis_host = config["Redis"]["host"].as<std::string>();
        conf->redis_port = config["Redis"]["port"].as<uint16_t>();
        conf->redis_pass = config["Redis"]["pass"].as<std::string>();

    } catch (const YAML::Exception &e) {
        std::cout << "conf.cpp" << std::endl;
        std::cerr << "Catch a YAML::Exception at line: " << e.mark.line + 1
                  << " column " << e.mark.column + 1 << " -> " << e.msg
                  << std::endl;
        return false;
    }
    return true;
}