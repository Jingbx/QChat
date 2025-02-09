/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/logic_system.h
 * @date    : 2024-12-11 15:28:21
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : handle http请求底层逻辑
 * -----
 * Last Modified: 2024-12-20 15:59:04
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#include "../../server_base/src/include/base.h"
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem : public SingleTon<LogicSystem>,
                    std::enable_shared_from_this<LogicSystem> {
    friend class SingleTon<LogicSystem>;

  public:
    ~LogicSystem();
    // 初始化 RouteMap
    void initRouteMap();
    // GET
    void registerGet(std::string url, HttpHandler handler);
    bool handleGet(std::string url, std::shared_ptr<HttpConnection> conn);
    // POST
    void registerPost(std::string url, HttpHandler handler);
    bool handlePost(std::string url, std::shared_ptr<HttpConnection> conn);

  private:
    LogicSystem();

    std::unordered_map<std::string, HttpHandler> _get_routemap;
    std::unordered_map<std::string, HttpHandler> _post_routemap;
};
