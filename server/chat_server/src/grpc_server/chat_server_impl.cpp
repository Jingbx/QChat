/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/grpc_server/chat_grpc_server.cpp
 * @date    : 2024-12-31 18:29:33
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-09 22:21:19
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/
#include "chat_server_impl.h"
#include "../user_mgr/user_manager.h"
#include "../redis_server/redis_manager.h"
#include "../mysql_server/mysql_manager.h"
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>

ChatServerImpl::ChatServerImpl() {}

ChatServerImpl::~ChatServerImpl() {}

Status ChatServerImpl::NotifyAddFriend(ServerContext *context,
                                       const AddFriendReq *request,
                                       AddFriendRsp *reply) {
    std::cout << "=============== ChatServerImpl::NotifyAddFriend ===============" << std::endl;
    // 确认uid是否存在session在本服务器内存中
    auto session = UserManager::GetInstance()->getSessionFromUid(request->touid());
    Defer defer([request, reply] () {
        reply->set_error(ErrorCodes::SUCCESS);
        reply->set_applyuid(request->applyuid());
        reply->set_touid(request->touid());
    });
    
    if (!session) {
        std::cout << "Touid's session is null, peer outline..." << std::endl;
        return Status::OK;
    }
    
    // Notify add friend in 本服务器内存
    Json::Value notify_req;
    notify_req["error"] = ErrorCodes::SUCCESS;
	notify_req["applyuid"] = request->applyuid();
	notify_req["name"] = request->name();
	notify_req["desc"] = request->desc();
	notify_req["icon"] = request->icon();
	notify_req["sex"] = request->sex();
	notify_req["nick"] = request->nick();
    
    std::string notify_req_str = notify_req.toStyledString();
    session->send(ID_NOTIFY_ADD_FRIEND_REQ, notify_req_str);
    
    return Status::OK;
}

Status ChatServerImpl::NotifyAuthFriend(ServerContext *context,
                                        const AuthFriendReq *request,
                                        AuthFriendRsp *response) {
                                            
	//查找用户是否在本服务器 touid是本服务器 因为我门是验证端
	auto touid = request->touid();
	auto fromuid = request->fromuid();
	auto session = UserManager::GetInstance()->getSessionFromUid(touid);

	Defer defer([request, response]() {
		response->set_error(ErrorCodes::SUCCESS);
		response->set_fromuid(request->fromuid());
		response->set_touid(request->touid());
		});

	//用户不在内存中则直接返回
	if (session == nullptr) {
		return Status::OK;
	}

	//在内存中则直接发送通知对方
	Json::Value  rtvalue;
	rtvalue["error"] = ErrorCodes::SUCCESS;
	rtvalue["fromuid"] = request->fromuid();
	rtvalue["touid"] = request->touid();

	std::string base_key = USER_BASE_INFO + std::to_string(fromuid);
	auto user_info = std::make_shared<UserInfo>();
	bool b_info = GetUserInfoFromDataBase(base_key, fromuid, user_info);
	if (b_info) {
		rtvalue["name"] = user_info->user;
		rtvalue["nick"] = user_info->nick;
		rtvalue["icon"] = user_info->icon;
		rtvalue["sex"] = user_info->sex;
	}
	else {
		std::cout << "ChatServerImpl::NotifyAuthFriend1 出错没找到base_key" << std::endl;
		std::cout << "base_key is " << base_key << std::endl;
		
		rtvalue["error"] = BackendErrorCodes::ERR_UID_INVALID;
	}

	std::string return_str = rtvalue.toStyledString();

	session->send(ID_NOTIFY_AUTH_FRIEND_REQ, return_str);
	return Status::OK;
}

Status ChatServerImpl::NotifyTextChatMsg(::grpc::ServerContext *context,
                                         const TextChatMsgReq *request,
                                         TextChatMsgRsp *response) {
	//查找用户是否在本服务器 touid是本服务器 因为我门是验证端
	auto touid = request->touid();
	auto fromuid = request->fromuid();
	auto session = UserManager::GetInstance()->getSessionFromUid(touid);

	//用户不在内存中则直接返回
	if (session == nullptr) {
		return Status::OK;
	}

	//在内存中则直接发送通知对方
	Json::Value  rtvalue;
	rtvalue["error"] = ErrorCodes::SUCCESS;
	rtvalue["fromuid"] = request->fromuid();
	rtvalue["touid"] = request->touid();

	//important: 将聊天数据组织为数组
	Json::Value text_array;

	std::cout << "Number of text messages: " << request->textmsgs_size() << std::endl;
	for (int i = 0; i < request->textmsgs_size(); ++i) {
		const auto& msg = request->textmsgs(i);
		Json::Value element;
		element["content"] = msg.msgcontent();
		element["msgid"] = msg.msgid();
		text_array.append(element);
		std::cout << "Appending message: content = " << msg.msgcontent() 
				<< ", msgid = " << msg.msgid() << std::endl;
	}
	rtvalue["text_array"] = text_array;
	std::cout << "Final JSON to be sent: " << rtvalue.toStyledString() << std::endl;

	std::string return_str = rtvalue.toStyledString();

	session->send(ID_NOTIFY_TEXT_CHAT_MSG_REQ, return_str);
	return Status::OK;
}

bool ChatServerImpl::GetUserInfoFromDataBase(std::string base_key, 
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
