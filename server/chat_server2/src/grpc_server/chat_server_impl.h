/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/grpc_server/chat_grpc_server.h
 * @date    : 2024-12-31 18:29:30
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-31 21:22:25
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#ifndef __CHAT_SERVER_IMPL_H
#define __CHAT_SERVER_IMPL_H

#include "../../server_base/src/include/base.h"
#include "../../server_base/src/include/message.grpc.pb.h"
#include "../../server_base/src/include/message.pb.h"
#include "../user_mgr/user_data.h"
#include <grpcpp/grpcpp.h> 

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::ChatService;
using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

class ChatServerImpl : public ChatService::Service {
public:
    ChatServerImpl();
    ~ChatServerImpl();
    
	Status NotifyAddFriend(ServerContext* context, const AddFriendReq* request,
		                   AddFriendRsp* reply) override;

	Status NotifyAuthFriend(ServerContext* context, 
		                    const AuthFriendReq* request, AuthFriendRsp* response) override;

	Status NotifyTextChatMsg(::grpc::ServerContext* context, 
		const TextChatMsgReq* request, TextChatMsgRsp* response) override;
        
    bool GetUserInfoFromDataBase(std::string base_key, 
                                 int uid, 
                                 std::shared_ptr<UserInfo>& userinfo);
};


#endif // __CHAT_SERVER_IMPL_H