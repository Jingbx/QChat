/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/grpc_client/chat_grpc_client.cpp
 * @date    : 2024-12-31 18:29:12
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-09 21:53:36
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#include "chat_grpc_client.h"
#include "../config/conf.h"
#include "../redis_server/redis_manager.h"
#include "../mysql_server/mysql_manager.h"
#include <climits>
#include <sstream>

extern GeneralConf *chat_config;
std::string conf_path = "conf/chat_server.yaml";
YAML::Node peer_servers_conf = YAML::LoadFile(conf_path);

ChatGrpcClient::ChatGrpcClient() {
    std::vector<std::string> peerServerNames;
    _parsePeerServerConf(peerServerNames);

    // important: 每个peer server对应一个grpc连接池
    for (const auto &peerServerName : peerServerNames) {
        if (peer_servers_conf[peerServerName]["name"].as<std::string>().empty()) {
            std::cout << "[Debug] 解析[peerServerName]['name']失败" << std::endl;
            continue;
        } 
        _conn_pools[peer_servers_conf[peerServerName]["name"].as<std::string>()] = 
            std::make_unique<RPCConnectionPool<ChatService, ChatService::Stub>>(
                chat_config->grpc_conn_pool_size, 
                peer_servers_conf[peerServerName]["host"].as<std::string>(),
                peer_servers_conf[peerServerName]["rpc_port"].as<std::string>()
            );
        // fixed:
        if (_conn_pools.size() == 0)  {
            std::cout << "chat grpc client pool is empty!" << std::endl;
        }
    }
}

ChatGrpcClient::~ChatGrpcClient() {

}

AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_name,
                                             const AddFriendReq &req) {
    std::cout << "=============== ChatGrpcClient::NotifyAddFriend1 ===============" << std::endl;
    AddFriendRsp rsp;
    Defer defer([&rsp, req] () {
        rsp.set_error(ErrorCodes::SUCCESS);
        rsp.set_applyuid(req.applyuid());
        rsp.set_touid(req.touid());
    });
    auto iter = _conn_pools.find(server_name);
    if (_conn_pools.end() == iter) {
        std::cout << "Not Find Server's pool" << std::endl;
        return rsp;
    }
    
    auto &stub_pool = iter->second; // note: 不许拷贝unique ptr
    auto stub = stub_pool->getConnection();
    
    ClientContext context;
    Status status = stub->NotifyAddFriend(&context, req, &rsp);
	Defer defercon([&stub, this, &stub_pool]() {
		stub_pool->recycleConnection(std::move(stub));
	});

    if (!status.ok()) {
		rsp.set_error(ErrorCodes::ERROR_RPC);   //显式设置 rsp.error 为 ERROR_RPC 的操作具有更高优先级，确保不会被 defer 回调中的 SUCCESS 值覆盖。 ？
		return rsp;        
    } 
    return rsp;
}

AuthFriendRsp ChatGrpcClient::NotifyAuthFriend(std::string server_name,
                                               const AuthFriendReq &req) {
    std::cout << "=============== ChatGrpcClient::NotifyAuthFriend1 ===============" << std::endl;
    AuthFriendRsp rsp;
    Defer defer([&rsp, req] () {
        rsp.set_error(ErrorCodes::SUCCESS);
        rsp.set_fromuid(req.fromuid());
        rsp.set_touid(req.touid());
    });
    auto iter = _conn_pools.find(server_name);
    if (_conn_pools.end() == iter) {
        std::cout << "Not Find Server's pool" << std::endl;
        return rsp;
    }
    
    auto &stub_pool = iter->second; // note: 不许拷贝unique ptr
    auto stub = stub_pool->getConnection();
    
    ClientContext context;
    Status status = stub->NotifyAuthFriend(&context, req, &rsp);
	Defer defercon([&stub, this, &stub_pool]() {
		stub_pool->recycleConnection(std::move(stub));
	});

    if (!status.ok()) {
		rsp.set_error(ErrorCodes::ERROR_RPC);   //显式设置 rsp.error 为 ERROR_RPC 的操作具有更高优先级，确保不会被 defer 回调中的 SUCCESS 值覆盖。 ？
		return rsp;        
    } 
    return rsp;                                             
}

TextChatMsgRsp ChatGrpcClient::NotifyTextChatMsg(std::string server_name,
                                                 const TextChatMsgReq &req,
                                                 const Json::Value &rtvalue) {
    std::cout << "=============== ChatGrpcClient::NotifyTextChatMsg1 ===============" << std::endl;
	TextChatMsgRsp rsp;
	rsp.set_error(ErrorCodes::SUCCESS);

	Defer defer([&rsp, &req]() {
		rsp.set_fromuid(req.fromuid());
		rsp.set_touid(req.touid());
		for (const auto& text_data : req.textmsgs()) {
			TextChatData *new_msg = rsp.add_textmsgs();
			new_msg->set_msgid(text_data.msgid());
			new_msg->set_msgcontent(text_data.msgcontent());
		}
	});

	auto find_iter = _conn_pools.find(server_name);
	if (find_iter == _conn_pools.end()) {
		return rsp;
	}

	auto& pool = find_iter->second;
	ClientContext context;
	auto stub = pool->getConnection();
	Status status = stub->NotifyTextChatMsg(&context, req, &rsp);
	Defer defercon([&stub, this, &pool]() {
		pool->recycleConnection(std::move(stub));
		});

	if (!status.ok()) {
		rsp.set_error(ErrorCodes::ERROR_RPC);
		return rsp;
	}

	return rsp;  
}


bool ChatGrpcClient::GetUserInfoFromDataBase(std::string base_key, 
                                             int uid, 
                                             std::shared_ptr<UserInfo> &userinfo) {
    // redis
    std::string info_str = "";
    auto ret = RedisManager::GetInstance()->Get(base_key, info_str);
    if (ret) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        userinfo->uid = root["uid"].asInt();
        userinfo->user = root["name"].asString();
        userinfo->email = root["email"].asString();
        userinfo->nick = root["nick"].asString();
        userinfo->desc = root["desc"].asString();
        userinfo->sex = root["sex"].asInt();
        userinfo->icon = root["icon"].asString();
        std::cout << "GetUserInfoFromDataBase: User login uid is  "
                  << userinfo->uid << " name  is " << userinfo->user
                  << " email is " << userinfo->email << std::endl;
    } else {
        // mysql
        std::shared_ptr<UserInfo> user_info = nullptr;
        user_info = MysqlManager::GetInstance()->getUser(uid);
        if (user_info == nullptr) {
            std::cout << "GetUserInfo from redis and mysql error!" << std::endl;
            return false;
        }
        // important: 修改引用的智能指针，让它指向新的 UserInfo 对象
        userinfo = user_info;
        // 将数据库内容写入redis缓存
        Json::Value redis_root;
        redis_root["uid"] = uid;
        redis_root["name"] = userinfo->user;
        redis_root["email"] = userinfo->email;
        redis_root["nick"] = userinfo->nick;
        redis_root["desc"] = userinfo->desc;
        redis_root["sex"] = userinfo->sex;
        redis_root["icon"] = userinfo->icon;
        RedisManager::GetInstance()->Set(base_key, redis_root.toStyledString());
    }
    return true;
}

void ChatGrpcClient::_parsePeerServerConf(std::vector<std::string> &peerServerNames) {
    try {
        if (conf_path.empty()) {
            std::cout << "Config path is empty" << std::endl;
            return;
        }
        
        std::string peer_servers_str = peer_servers_conf["peerserver"]["servers"].as<std::string>();
        std::stringstream peer_servers_ss(peer_servers_str);
        
        std::string ps_name;
        while (std::getline(peer_servers_ss, ps_name, ',')) {
            peerServerNames.push_back(ps_name);
        }
    } catch (const YAML::Exception &e) {
        std::cout << "chat_server/src/grpc_client/chat_grpc_client.cpp" << std::endl;
        std::cerr << "Catch a YAML::Exception at line: " << e.mark.line + 1
                  << " column " << e.mark.column + 1 << " -> " << e.msg
                  << std::endl;
        return;      
    }
}