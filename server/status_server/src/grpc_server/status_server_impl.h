/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/status_server/src/grpc_server/status_server_impl.h
 * @date    : 2024-12-20 10:37:31
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : grpc StatusServer的服务端Implementation实施
 * -----
 * Last Modified: 2024-12-23 12:40:32
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * TODO: Execut command [ HGet logincount chatserver1  ] failure ! ChatSever进程还没有写入这个键
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#include "../../server_base/src/include/base.h"
#include "../../server_base/src/include/grpc_connection_pool.h"
#include "../../server_base/src/include/message.grpc.pb.h"
#include "../redis_server/redis_manager.h"
#include <atomic>
#include <climits>
#include <condition_variable>
#include <grpcpp/grpcpp.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

using grpc::Channel;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;

using message::LoginReq;
using message::LoginRsp;

using message::StatusService;

class ChatServer {
public:
    ChatServer() : host(""), port(""), name(""), conn_nums(0) {}
    ChatServer(const std::string &host_, const std::string &port_,
               const std::string &name_)
        : host(host_), port(port_), name(name_), conn_nums(0) {}
    /**
     * @brief 允许拷贝构造
     */
    ChatServer &operator=(const ChatServer &cs) {
        if (&cs == this) { // fixed:
            return *this;
        }
        this->host = cs.host;
        this->port = cs.port;
        this->name = cs.name;
        this->conn_nums = cs.conn_nums;
        return *this;
    }
    ~ChatServer() {}

public:
    std::string host;
    std::string port;
    std::string name;
    int conn_nums;
};

class StatusServerImpl final : public StatusService::Service {
public:
    StatusServerImpl();
    ~StatusServerImpl();

    Status GetChatServer(ServerContext *context,
                         const GetChatServerReq *request,
                         GetChatServerRsp *response) override;
                         
    Status Login(ServerContext* context, const LoginReq* request,
    	LoginRsp* reply) override;

private:
    void _parseChatServerConf(const std::string &conf_path);
    /**
     * @brief redis查询conn_num次数，挑少的
     */
    bool _getChatServer(ChatServer &cs);
    /**
     * @brief redis操作
     */
    bool _redisSetToken(int uid, const std::string &token);

private:
    std::map<std::string, ChatServer>           _servers;       // fixed: 确保顺序
    std::mutex                                  _server_mtx;
};
