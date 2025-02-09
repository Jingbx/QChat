/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/status_server/main.cpp
 * @date    : 2024-12-20 09:49:37
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : grpc::ServerBuilder开启grpc服务器的监听0.0.0.0并挂起等待；
 *            并开始boost事件循环
 * -----
 * Last Modified: 2024-12-21 15:52:11
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#include "src/config/conf.h"
#include "src/grpc_server/status_server_impl.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <memory>
GeneralConf *status_config = nullptr;

bool init_general_config(const std::string &conf_path) {
    status_config= new GeneralConf();
    return load_generate_conf(conf_path, status_config);
}

void clearAll() {
    if (status_config) {
        delete status_config;
        status_config = nullptr;
    }
}

void runServer(const std::string &url, StatusServerImpl &ssImpl) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(url, grpc::InsecureServerCredentials());
    builder.RegisterService(&ssImpl);  // 注册服务器实现方法
    
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    if (status_config) {
        std::cout << "=== Status Server start on " << status_config->status_server_port << " ===" << std::endl;
    } else {
        std::cout << "Please first init status server config!" << std::endl;
        exit(1);
    }

    // boost事件循环，监听信号触发
    boost::asio::io_context ioc;
    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&server] (const boost::system::error_code &err_code, int signal_num) {
        std::cout << " === Status Server Shutdowm ===" << std::endl;
        server->Shutdown();
    });
    
    // 事件循环detach一个线程来检测信号
    std::thread([&ioc] () {
        ioc.run();
    }).detach();

    // grpc server等待
    server->Wait();
    ioc.stop();
}

int main() {
    try {
        // config
        bool ret = init_general_config("conf/status_server.yaml");
        if (!ret) {
            std::cout << "Init config error!" << std::endl;
            exit(1);
        }
        // server url and impl
        std::string url(status_config->status_server_host + ":" 
                        + std::to_string(status_config->status_server_port));
        StatusServerImpl ssImpl;
        // run
        runServer(url, ssImpl);
    } catch (const std::exception &e) {
        std::cout << "Status Server run error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}