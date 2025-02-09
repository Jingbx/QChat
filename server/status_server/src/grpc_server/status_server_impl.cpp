/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/status_server/src/grpc_server/status_server_impl.cpp
 * @date    : 2024-12-20 10:37:33
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-31 20:46:44
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#include "status_server_impl.h"

std::string generate_unique_string() {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    // 使用 boost::uuids::to_string 将 uuid 转换为字符串
    std::string uuid_str = boost::uuids::to_string(uuid);
    return uuid_str;
}

StatusServerImpl::StatusServerImpl() {
    _parseChatServerConf("conf/status_server.yaml");
}

StatusServerImpl::~StatusServerImpl() {}

Status StatusServerImpl::GetChatServer(ServerContext *context,
                                       const GetChatServerReq *request,
                                       GetChatServerRsp *response) {
    std::string prefix("Jbx grpc status server has received :  ");
    ChatServer cs;
    bool ret = _getChatServer(cs);
    if (!ret) {
        response->set_host("");
        response->set_port("");
        response->set_error(BackendErrorCodes::ERR_GET_CAHT_SERVER);        // TODO: gateserver status客户端接收设置
        // response->set_token(generate_unique_string());
        // ret = _redisSetToken(request->uid(), response->token());
        return Status::OK;
    }
    response->set_host(cs.host);
    response->set_port(cs.port);
    response->set_error(ErrorCodes::SUCCESS);
    response->set_token(generate_unique_string());
    ret = _redisSetToken(request->uid(), response->token());    // note: 获取ChatServer后创建token返回并放入Redis
    if (!ret) std::cout << "Redis set token error" << std::endl;
    return Status::OK;
}

Status StatusServerImpl::Login(ServerContext *context, const LoginReq *request,
                               LoginRsp *reply) {
    // 查询token: 不存在表明uid不合法 存在但不同表明token不合法
    int uid = request->uid();
    std::string token = request->token();
    std::string token_key = USERTOKENPREFIX + std::to_string(uid);
    std::string token_value= "";
    
    bool ret = RedisManager::GetInstance()->Get(token_key, token_value);
    if (!ret) {
        reply->set_error(BackendErrorCodes::ERR_UID_INVALID);
        return Status::OK;
    }
    if (token_value != token) {
        reply->set_error(BackendErrorCodes::ERR_TOKEN_INVALID);
        return Status::OK;
    }
	reply->set_error(ErrorCodes::SUCCESS);
	reply->set_uid(uid);
	reply->set_token(token);
	return Status::OK;                            
}

/**
 * @brief 获取ChatServer
 */
void StatusServerImpl::_parseChatServerConf(const std::string &conf_path) {
    try {
        if (conf_path.empty()) {
            std::cout << "Config path is empty" << std::endl;
            return;
        }
        YAML::Node chat_server_conf = YAML::LoadFile(conf_path);

        std::string chat_servers_str =
            chat_server_conf["ChatServers"]["names"].as<std::string>();
        std::stringstream chat_servers_ss(chat_servers_str);
        // split
        std::vector<std::string> cs_names;
        std::string cs_name;
        while (std::getline(chat_servers_ss, cs_name, ',')) {
            cs_names.push_back(cs_name);
        }
        // parse all chatserver
        for (const auto &name : cs_names) {
            ChatServer chat_server;
            chat_server.name = chat_server_conf[name]["name"].as<std::string>();
            chat_server.host = chat_server_conf[name]["host"].as<std::string>();
            chat_server.port = chat_server_conf[name]["port"].as<std::string>();
            _servers[chat_server.name] = chat_server;
        }

    } catch (const YAML::Exception &e) {
        std::cerr << "Catch a YAML::Exception at line: " << e.mark.line + 1
                  << " column " << e.mark.column + 1 << " -> " << e.msg
                  << std::endl;
        return;
    }
}

bool StatusServerImpl::_getChatServer(ChatServer &cs) {
    std::lock_guard<std::mutex> lock(_server_mtx);
    
    if (_servers.empty()) {
        std::cout << "No ChatServer, check your status conf" << std::endl;
        return false;
    }
    ChatServer selected_cs = _servers.begin()->second;
    std::string conn_nums = "";
    RedisManager::GetInstance()->HGet(LOGIN_COUNT, selected_cs.name, conn_nums);
    if (conn_nums.empty()) {
        selected_cs.conn_nums = INT_MAX;
    } else {
        selected_cs.conn_nums = std::stoi(conn_nums);
    }

    // 遍历查找
    for (auto &server : _servers) {
        if (selected_cs.name == server.second.name) {
            continue;
        }
        std::string conn_nums = "";
        RedisManager::GetInstance()->HGet(LOGIN_COUNT, server.second.name, conn_nums);
        if (conn_nums.empty()) {
            server.second.conn_nums = INT_MAX;
        } else {
            server.second.conn_nums = std::stoi(conn_nums);
        }
        if (selected_cs.conn_nums > server.second.conn_nums) {
            selected_cs = server.second;
        } else {
            continue;
        }
    }
    cs = selected_cs;
    return true;
}


/**
 * @brief redis
 */
bool StatusServerImpl::_redisSetToken(int uid, const std::string &token) {
    auto token_key = USERTOKENPREFIX + std::to_string(uid);
    return RedisManager::GetInstance()->Set(token_key, token);
}