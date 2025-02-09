/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/logic_system.cpp
 * @date    : 2024-12-11 15:28:52
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-09 20:45:31
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#include "logic_system.h"
#include "http_connection.h"
#include "mysql_server/mysql_manager.h"
#include "redis_server/redis_manager.h"
#include "verify_client/verify_grpc_client.h"
#include "status_client/status_grpc_client.h"

LogicSystem::LogicSystem() { initRouteMap(); }

LogicSystem::~LogicSystem() {}

void LogicSystem::initRouteMap() {
    // GET Route
    registerGet("/get_test", [](std::shared_ptr<HttpConnection> conn) {
        boost::beast::ostream(conn->_response.body()) << "receive GET req\r\n";
        std::stringstream ss;
        for (const auto &kv_pair : conn->_get_params) {
            ss << kv_pair.first << " : " << kv_pair.second << "\n";
        }
        boost::beast::ostream(conn->_response.body()) << ss.str() << "\r\n";
    });

    // POST Route
    // 1. VeriCode
    registerPost("/get_vericode", [](std::shared_ptr<HttpConnection> conn) {
        // parse request(反序列化)
        std::string request_body = boost::beast::buffers_to_string(
            conn->_request.body().data()); // 序列化的
        std::cout << "/get_vericode request body: " << request_body
                  << std::endl;
        Json::Reader reader;
        Json::Value req_root;
        Json::Value resp_root;
        // 回复响应记得序列化:
        // note:解析错误或键不存在直接返回错误响应不继续通知grpc;正确的话通信grpc准备验证服务
        conn->_response.set(http::field::content_type, "text/json");
        bool parse_ret = reader.parse(request_body, req_root);
        if (!parse_ret) {
            std::cout << "Failed to parse Json data in POST!" << std::endl;
            // 构建错误response
            resp_root["error"] = ErrorCodes::ERROR_JSON;
            std::string resp_jsonstr = resp_root.toStyledString(); // 序列化
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        if (!req_root.isMember("email")) {
            std::cout << "Failed to parse Json data in POST, err: no key named "
                         "[email]"
                      << std::endl;
            resp_root["error"] = ErrorCodes::ERROR_JSON;
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        // right:
        auto email = req_root["email"].asString();
        std::cout << "email is " << email << std::endl;
        GetVerifyRsp resp =
            VerifyCodeClient::GetInstance()->getVerifyCode(email);
        // Enhanced: 增加请求频繁错误
        if (resp.error() == BackendErrorCodes::ERR_TOOMANYREQ) {
            resp_root["error"] =
                BackendErrorCodes::ERR_TOOMANYREQ; // TODO: 多增加一步
                                                   // 包括TooManyRequest错误
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        resp_root["error"] = resp.error();
        resp_root["email"] = email;
        std::string resp_jsonstr = resp_root.toStyledString();
        boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
        return true;
    });

    // 2.Register
    registerPost("/user_register", [](std::shared_ptr<HttpConnection> conn) {
        // 解析req2jsonObj
        std::string request_body =
            boost::beast::buffers_to_string(conn->_request.body().data());
        std::cout << "/user_register request body: " << request_body
                  << std::endl;
        Json::Value req_root;
        Json::Value resp_root;
        Json::Reader reader;
        bool parse_ret = reader.parse(request_body, req_root);
        // 返回resp2jsonBinary
        conn->_response.set(http::field::content_type, "text/json");
        if (!parse_ret) {
            std::cout << "Failed to parse Json data in POST!" << std::endl;
            resp_root["error"] = ErrorCodes::ERROR_JSON;
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        auto username = req_root["user"].asString();
        auto password = req_root["password"].asString(); // TODO: unused
        auto confirm = req_root["confirm"].asString();   // TODO: unused
        auto email = req_root["email"].asString();
        auto verifycode = req_root["verifycode"].asString();

        // TODO: pwd != confirm
        // jingbx 觉得没必要前端已经验证了...

        // vericode expired
        std::string real_verifycode;
        bool is_got = RedisManager::GetInstance()->Get(CODEPREFIX + email,
                                                       real_verifycode);
        if (!is_got) {
            std::cout << "VerifyCode is expired or not exited!" << std::endl;
            resp_root["error"] =
                BackendErrorCodes::ERR_VARIFY_EXPIRED; // 过期错误
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        // vericode error
        if (real_verifycode != verifycode) {
            std::cout << "VerifyCode is error!" << std::endl;
            resp_root["error"] = BackendErrorCodes::
                ERR_VARIFY_ERR; // 与Redis中值不同验证码输入错误
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        // TODO: MySQL验证用户是否已经存在 ✅
        int uid = MysqlManager::GetInstance()->registerUser(username, email,
                                                            password);
        if (uid == 0 || uid == -1) {
            std::cout << "User or Email is exist" << ": uid=" << uid
                      << std::endl;
            resp_root["error"] = BackendErrorCodes::ERR_USER_OR_EMAIL_EXISTED;
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }

        // right: 都透传回去?
        resp_root["error"] = ErrorCodes::SUCCESS;
        resp_root["uid"] = uid;
        resp_root["email"] = email;
        resp_root["user"] = username;
        resp_root["password"] = password;
        resp_root["confirm"] = confirm;
        // resp_root["icon"] = icon;    // TODO:
        resp_root["verifycode"] = req_root["verifycode"].asString();
        std::cout << "用户SQL uid=" << uid << std::endl;
        std::string jsonstr = resp_root.toStyledString();
        boost::beast::ostream(conn->_response.body()) << jsonstr;
        return true;
    });
    // 3.Reset password
    registerPost("/reset_pwd", [](std::shared_ptr<HttpConnection> conn) {
        // 解析req2jsonObj
        std::string request_body =
            boost::beast::buffers_to_string(conn->_request.body().data());
        std::cout << "/reset_pwd request body: " << request_body << std::endl;
        Json::Value req_root;
        Json::Value resp_root;
        Json::Reader reader;
        bool parse_ret = reader.parse(request_body, req_root);
        // 返回resp2jsonBinary
        conn->_response.set(http::field::content_type, "text/json");
        if (!parse_ret) {
            std::cout << "Failed to parse Json data in POST!" << std::endl;
            resp_root["error"] = ErrorCodes::ERROR_JSON;
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        auto username = req_root["user"].asString();
        auto new_password = req_root["new_password"].asString();
        auto email = req_root["email"].asString();
        auto verifycode = req_root["verifycode"].asString();

        // vericode expired
        std::string real_verifycode;
        bool is_got = RedisManager::GetInstance()->Get(CODEPREFIX + email,
                                                       real_verifycode);
        if (!is_got) {
            std::cout << "VerifyCode is expired or not exited!" << std::endl;
            resp_root["error"] =
                BackendErrorCodes::ERR_VARIFY_EXPIRED; // 过期错误
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        // vericode error
        if (real_verifycode != verifycode) {
            std::cout << "VerifyCode is error!" << std::endl;
            resp_root["error"] = BackendErrorCodes::
                ERR_VARIFY_ERR; // 与Redis中值不同验证码输入错误
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        // TODO: MySQL验证用户名和该邮箱是否匹配 ✅
        bool ret = MysqlManager::GetInstance()->checkEmail(username, email);
        if (!ret) {
            std::cout << "User and Email is not matched!" << std::endl;
            resp_root["error"] = BackendErrorCodes::ERR_NO_MATCHED_EMAIL;
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        // TODO: MySQL更新密码
        ret = MysqlManager::GetInstance()->updatePwd(username, new_password);
        if (!ret) {
            std::cout << "Update password error!" << std::endl;
            resp_root["error"] = BackendErrorCodes::ERR_UPDATE_PWD;
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        // right: 都透传回去?
        resp_root["error"] = ErrorCodes::SUCCESS;
        resp_root["user"] = username;
        resp_root["email"] = email;
        resp_root["password"] = new_password;
        resp_root["verifycode"] = req_root["verifycode"].asString();
        std::string jsonstr = resp_root.toStyledString();
        boost::beast::ostream(conn->_response.body()) << jsonstr;
        return true;
    });
    // 4. Login
    registerPost("/user_login", [](std::shared_ptr<HttpConnection> conn) {
        // 解析req2jsonObj
        std::string request_body =
            boost::beast::buffers_to_string(conn->_request.body().data());
        std::cout << "/user_login request body: " << request_body << std::endl;
        Json::Value req_root;
        Json::Value resp_root;
        Json::Reader reader;
        bool parse_ret = reader.parse(request_body, req_root);
        // 返回resp2jsonBinary
        conn->_response.set(http::field::content_type, "text/json");
        if (!parse_ret) {
            std::cout << "Failed to parse Json data in POST!" << std::endl;
            resp_root["error"] = ErrorCodes::ERROR_JSON;
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        }
        auto email = req_root["email"].asString();
        auto password = req_root["password"].asString();
        UserInfo user_info;
        // TODO:
        // MySQL验证密码是否存在/匹配->查询不到邮箱row=0则报出需要注册的错误/验证登录✅
        int err =
            MysqlManager::GetInstance()->checkPwd(email, password, user_info);
        if (err < 0) {
            std::cout << "Login error!" << std::endl;
            resp_root["error"] = BackendErrorCodes::ERR_USER_LOGIN;
            std::string resp_jsonstr = resp_root.toStyledString();
            boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
            return true;
        } else if (err != 0) {
            if (MysqlDaoErrorCodes::SQL_ERR_EMAIL_NOT_EXITED == err) {
                std::cout << "User is not exited!" << std::endl;
                resp_root["error"] = BackendErrorCodes::ERR_EMAIL_NOT_EXITED;
                std::string resp_jsonstr = resp_root.toStyledString();
                boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
                return true;
            }
            if (MysqlDaoErrorCodes::SQL_ERR_PWD_NOT_MATCH == err) {
                std::cout << "Password is not matched!" << std::endl;
                resp_root["error"] = BackendErrorCodes::ERR_PWD_NOT_MATCH;
                std::string resp_jsonstr = resp_root.toStyledString();
                boost::beast::ostream(conn->_response.body()) << resp_jsonstr;
                return true;
            }
        }
        // TODO: StateServer获取ChatServer长链接ip port token 负载均衡方式
        auto resp = StatusGrpcClient::GetInstance()->getChatServer(user_info.uid);
        err = resp.error();
        std::cout << "gate server resp err: " << err << std::endl;
		if (BackendErrorCodes::ERR_GET_CAHT_SERVER == err) {
			std::cout << " grpc get chat server failed, error is " << resp.error() << std::endl;
			resp_root["error"] = BackendErrorCodes::ERR_GET_CAHT_SERVER;
			std::string jsonstr = resp_root.toStyledString();
			boost::beast::ostream(conn->_response.body()) << jsonstr;
			return true;            
		}
        if (ErrorCodes::SUCCESS != err) {
			std::cout << " grpc get chat server failed, error is " << resp.error() << std::endl;
			resp_root["error"] = ErrorCodes::ERROR_RPC;
			std::string jsonstr = resp_root.toStyledString();
			boost::beast::ostream(conn->_response.body()) << jsonstr;
			return true;               
        }
        // right: 都透传回去?
		std::cout << "succeed to load userinfo uid is " << user_info.uid << std::endl;
		resp_root["error"] = err;
		resp_root["email"] = email;
		resp_root["uid"] = user_info.uid;
		resp_root["token"] = resp.token();
		resp_root["host"] = resp.host();
		resp_root["port"] = resp.port();
		std::string jsonstr = resp_root.toStyledString();
		boost::beast::ostream(conn->_response.body()) << jsonstr;
		return true;
    });
}

void LogicSystem::registerGet(std::string url, HttpHandler handler) {
    _get_routemap.insert(std::make_pair(url, handler));
}

bool LogicSystem::handleGet(std::string url,
                            std::shared_ptr<HttpConnection> conn) {
    // 查找路由表
    if (_get_routemap.find(url) == _get_routemap.end()) {
        return false;
    }
    // handle
    _get_routemap[url](conn);
    return true;
}

void LogicSystem::registerPost(std::string url, HttpHandler handler) {
    _post_routemap.insert(std::make_pair(url, handler));
}

bool LogicSystem::handlePost(std::string url,
                             std::shared_ptr<HttpConnection> conn) {
    if (_post_routemap.find(url) == _post_routemap.end()) {
        return false;
    }
    _post_routemap[url](conn);
    return true;
}


