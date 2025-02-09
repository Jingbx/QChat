#include "conf.h"
/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/src_base/conf.cpp
 * @date    : 2024-12-13 12:21:34
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-08 16:00:27
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

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
    conf->gate_server_port = 8080;
    conf->verify_server_host = "127.0.0.1";
    conf->verify_server_port = 50051;
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

        conf->gate_server_port = config["GateServer"]["port"].as<uint16_t>();
        conf->verify_server_host =
            config["VerifyServer"]["host"].as<std::string>();
        conf->verify_server_port =
            config["VerifyServer"]["port"].as<uint16_t>();
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
        std::cerr << "Catch a YAML::Exception at line: " << e.mark.line + 1
                  << " column " << e.mark.column + 1 << " -> " << e.msg
                  << std::endl;
        return false;
    }
    return true;
}