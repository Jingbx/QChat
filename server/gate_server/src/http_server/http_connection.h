/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/http_connection.h
 * @date    : 2024-12-11 14:31:44
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : 处理连接请求 回复相应
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

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
    friend class LogicSystem;

  public:
    HttpConnection(boost::asio::io_context &ioc);
    ~HttpConnection();
    void start();
    tcp::socket &getSocket(); // 拷贝构造/赋值被delete
  private:
    /**
     * @brief 根据http req类型使用不同Handler, get, post...
     */
    void preParseGetUrl();
    void _handleReq();
    void _writeResp();
    void _checkTimeout();

  private:
    tcp::socket _socket;

    std::string _get_url;
    std::unordered_map<std::string, std::string> _get_params;

    boost::beast::flat_buffer _buffer{8192};
    http::request<http::dynamic_body> _request;
    http::response<http::dynamic_body> _response;

    boost::asio::steady_timer _timeout{_socket.get_executor(),
                                       std::chrono::seconds(60)};
};