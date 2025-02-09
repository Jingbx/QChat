/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/main.cpp
 * @date    : 2024-12-21 16:26:47
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-09 21:12:45
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#include "config/conf.h"
#include "tcp_server/cserver.h"
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include "../server_base/src/include/asio_iocontext_pool.h"
#include "../server_base/src/include/base.h"
#include "redis_server/redis_manager.h"
#include "grpc_server/chat_server_impl.h"
#include "grpc_client/chat_grpc_client.h"

GeneralConf *chat_config = nullptr;

bool init_general_config(const std::string &conf_path) {
    chat_config = new GeneralConf();
    return load_generate_conf(conf_path, chat_config);
}

void clearAll() {
    if (chat_config) {
        delete chat_config;
        chat_config = nullptr;
    }
}

int main()
{
    try {
        init_general_config("conf/chat_server2.yaml");
        auto pool = AsioIOContextPool::GetInstance();
        // note: 服务器开启时设置Redis登录数量重新设为为0
        RedisManager::GetInstance()->HSet(LOGIN_COUNT, chat_config->chat_self_server_name, "0");
        
        // note: grpc服务单独线程启动
        ChatServerImpl server_impl;  // 自定义服务器实现
        std::string chat_server_ip = chat_config->chat_self_server_host + ":" +
                                     std::to_string(chat_config->chat_self_server_rpcport);
        grpc::ServerBuilder builder;
        
        builder.AddListeningPort(chat_server_ip, grpc::InsecureServerCredentials());
        builder.RegisterService(&server_impl);

        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        std::cout << "Chat Server RPC listening on " << chat_config->chat_self_server_rpcport << std::endl;
        
        std::thread grpc_server_thread([&server] () {
            server->Wait();
        });

        
        boost::asio::io_context  io_context;
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&io_context, pool, &server](auto, auto) {
            io_context.stop();
            server->Shutdown();
        });

        CServer s(io_context, chat_config->chat_self_server_port);    
        io_context.run();
        // note: 清理缓存，让新登录用户第一次加载从sql中获取
        RedisManager::GetInstance()->HDel(LOGIN_COUNT, chat_config->chat_self_server_name);
        RedisManager::GetInstance()->HDel(USER_BASE_INFO, chat_config->chat_self_server_name);
        RedisManager::GetInstance()->HDel(USERIPPREFIX, chat_config->chat_self_server_name);
        // fixed: todo: 
        std::string u13 = "ubaseinfo_13";
        std::string u14 = "ubaseinfo_14";
        std::string u18 = "ubaseinfo_18";
        RedisManager::GetInstance()->Del(u13);
        RedisManager::GetInstance()->Del(u14);
        RedisManager::GetInstance()->Del(u18);
        grpc_server_thread.join();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        RedisManager::GetInstance()->HDel(LOGIN_COUNT, chat_config->chat_self_server_name);
        RedisManager::GetInstance()->HDel(LOGIN_COUNT, chat_config->chat_self_server_name);
        RedisManager::GetInstance()->HDel(USER_BASE_INFO, chat_config->chat_self_server_name);
        RedisManager::GetInstance()->HDel(USERIPPREFIX, chat_config->chat_self_server_name);
    }
}