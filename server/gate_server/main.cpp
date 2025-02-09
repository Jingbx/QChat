#include "config/conf.h"
#include "http_server/c_server.h"
#include <iostream>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>

GeneralConf *gate_config = nullptr;

bool init_general_config(const std::string &conf_path) {
    gate_config = new GeneralConf();
    return load_generate_conf(conf_path, gate_config);
}

void clearAll() {
    if (gate_config) {
        delete gate_config;
        gate_config = nullptr;
    }
}

int main() {
    try {
        std::string gateserver_config_path = "conf/gate_server.yaml";
        bool ret = init_general_config(gateserver_config_path);
        if (!ret) {
            std::cerr << "Load Config Err" << std::endl;
            return EXIT_FAILURE;
        }
        uint16_t port = static_cast<uint16_t>(gate_config->gate_server_port);

        // test redisManager
        // TestRedisManager();

        // eventloop
        boost::asio::io_context ioc{1};
        // init termial signal
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        // wait eventloop over
        signals.async_wait(
            [&ioc](boost::beast::error_code err_code, int sig_nums) {
                if (err_code) {
                    return;
                }
                ioc.stop();
            });

        // start cserver
        std::make_shared<CServer>(ioc, port)->start();
        std::cout << "GateServer listen on port " << port << std::endl;
        ioc.run();

    } catch (const std::exception &e) {
        std::cout << "Main exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    clearAll();
}