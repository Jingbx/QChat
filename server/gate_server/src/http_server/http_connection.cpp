/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/http_connection.cpp
 * @date    : 2024-12-11 14:31:52
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-16 15:00:57
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#include "http_connection.h"
#include "logic_system.h"

HttpConnection::HttpConnection(boost::asio::io_context &ioc) : _socket(ioc) {

}

HttpConnection::~HttpConnection() {
    std::cout << "HttpConnection destructed at " << this << std::endl;
}

void HttpConnection::start() {
    auto self = shared_from_this();
    // 读取请求
    http::async_read(_socket, _buffer, _request, [self] (boost::beast::error_code err_code, size_t bytes_transferred) {
        try {
            if (err_code) {
                std::cout << "Http async read err, " << err_code.what() << std::endl;
                return;
            }
            // handel req
            boost::ignore_unused(bytes_transferred);
            self->_handleReq();
            self->_checkTimeout();
        } catch (const std::exception &e) {
            std::cout << "HttpConnection err, " << e.what() << std::endl;
            return;
        }
    });
}

tcp::socket & HttpConnection::getSocket() {
    return _socket;
}

/**
 * @brief 10->16
 */
uint8_t ToHex(uint8_t x) {
    return x > 9 ? x + 55 : x + 48; // 10->A
}

/**
 * @brief 16->10
 */
uint8_t FromHex(uint8_t x) {
    if (x >= 'A' && x <= 'Z') return x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') return x - 'a' + 10;
    else if (x > '0' && x < '9') return x - '0';
    else assert(false);
}

/**
 * @brief encode url
 */
std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		//判断是否仅有数字和字母以及某符号构成
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ') //为空字符
			strTemp += "+";
		else
		{
			//其他字符需要提前加%并且高四位和低四位分别转为16进制,如汉字
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] & 0x0F);
		}
	}
	return strTemp;
}

/**
 * @brief decode url
 */
std::string UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		//还原+为空
		if (str[i] == '+') strTemp += ' ';
		//遇到%将后面的两个字符从16进制转为char再拼接
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}


void HttpConnection::preParseGetUrl() {
    auto uri = _request.target();           // http://localhost:8080/get_test?key1=value1&key2=value2
    auto query_pos = uri.find('?');
    if (query_pos == std::string::npos) {
        _get_url = uri;
        return;
    }

    _get_url = uri.substr(0, query_pos);    // http://localhost:8080/get_test
    std::string query_str = uri.substr(query_pos + 1);

    size_t start = 0;
    while (start < query_str.size()) {
        auto pos = query_str.find('&', start);
        if (pos == std::string::npos) pos = query_str.size();
        auto kv_pos = query_str.find('=', start);
        if (kv_pos == std::string::npos || kv_pos > pos) {
            _get_params.insert(std::make_pair(UrlDecode(query_str.substr(start, pos - start)), ""));
        } else {
            _get_params.insert(std::make_pair(UrlDecode(query_str.substr(start, kv_pos - start)), 
                                              UrlDecode(query_str.substr(kv_pos + 1, pos - kv_pos - 1))));
        }
        start = pos + 1;
    }
}

void HttpConnection::_handleReq() {
    // 设置resp
    _response.version(_request.version());
    _request.keep_alive(false);
    // GET: 
    if (_request.method() == http::verb::get) {
        // TODO: 解析请求
        preParseGetUrl();
        // 处理请求
        bool ret = LogicSystem::GetInstance()->handleGet(_get_url, shared_from_this());
        if (!ret) {
            // 构造响应 
            _response.result(http::status::not_found);              // 404
            _response.set(http::field::content_type, "text/plain");
            boost::beast::ostream(_response.body()) << "url is not found\r\n";
            // 发送响应
            _writeResp();
            return;
        }
        _response.result(http::status::ok);
        _response.set(http::field::server, "GateServer");
        _writeResp();
        return;
    }
    // POST
    if (_request.method() == http::verb::post) {
        // 无需解析 handler json解析表单
        bool ret = LogicSystem::GetInstance()->handlePost(_request.target(), shared_from_this());   // note: core
        if (!ret) {
            _response.result(http::status::not_found);
            _response.set(http::field::content_type, "text/plain");
            boost::beast::ostream(_response.body()) << "url is not found\r\n";
            _writeResp();
            return;
        }
        _response.result(http::status::ok);
        _response.set(http::field::server, "GateServer");
        _writeResp();
        return;
    }
}

void HttpConnection::_writeResp() {
    auto self = shared_from_this();
    _response.content_length(_response.body().size());
    http::async_write(_socket, _response, [self] (boost::beast::error_code err_code, size_t bytes_transferre) {
        // 发送结束 优雅关闭并停止计时器
        self->_socket.shutdown(tcp::socket::shutdown_send, err_code);
        self->_timeout.cancel();
    });
    std::cout << "===== 已发送 =====" << std::endl;
}

void HttpConnection::_checkTimeout() {
    auto self = shared_from_this();
    _timeout.async_wait([self] (boost::beast::error_code err_code) {
        if (err_code) {
            // 超时断开连接,否则会手动停止计时器 
            self->_socket.close();  // Close socket to cancel any outstanding operation.
        }
        return;
    });
}
