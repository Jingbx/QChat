/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/grpc_client/chat_grpc_client.h
 * @date    : 2024-12-31 18:29:08
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-08 14:11:48
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#ifndef __CHAT_GRPC_CLIENT_H_
#define __CHAT_GRPC_CLIENT_H_


#include "../../server_base/src/include/base.h"
#include "../../server_base/src/include/singleton.h"
#include "../../server_base/src/include/grpc_connection_pool.h"
#include "../../server_base/src/include/message.grpc.pb.h"
#include "../../server_base/src/include/message.pb.h"
#include "../user_mgr/user_data.h"
#include <grpcpp/grpcpp.h> 
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;

using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

class ChatGrpcClient :public SingleTon<ChatGrpcClient>
{
	friend class SingleTon<ChatGrpcClient>;
public:
	~ChatGrpcClient();
    // 添加好友
	AddFriendRsp NotifyAddFriend(std::string server_name, const AddFriendReq& req);
    // 好友验证
	AuthFriendRsp NotifyAuthFriend(std::string server_name, const AuthFriendReq& req);
    // 发送文本
	TextChatMsgRsp NotifyTextChatMsg(std::string server_name, const TextChatMsgReq& req, const Json::Value& rtvalue);
	// 从Redis Mysql中获取UserInfo
	bool GetUserInfoFromDataBase(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
    
private:
	ChatGrpcClient();
	void _parsePeerServerConf(std::vector<std::string> &peerServerNames);
private:
	/**
	 * @brief 每个peerserver都对应一个连接池
	 */
	std::unordered_map<std::string, 
                       std::unique_ptr<RPCConnectionPool<ChatService, 
                                                         ChatService::Stub>>> _conn_pools;	
};


#endif // __CHAT_CRPC_CLIENT_H_