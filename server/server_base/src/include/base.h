#ifndef __SERVER_BASE_H_
#define __SERVER_BASE_H_

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "singleton.h"

namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

/**
 * @brief 验证码key前缀
 */
#define CODEPREFIX "code_"
#define USERIPPREFIX "uip_"
#define USERTOKENPREFIX "utoken_"
#define IPCOUNTPREFIX "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define NAME_INFO  "nameinfo_"
#define LOGIN_COUNT "logincount"

/**
 * @brief Tcp设置
 */
#define MAX_LENGTH  1024*2
#define HEAD_TOTAL_LEN 4
#define HEAD_ID_LEN 2
#define HEAD_DATA_LEN 2
#define MAX_RECVQUE  10000
#define MAX_SENDQUE 1000

/**
 * @brief TCP请求ID
 */
enum MSG_IDS {
    ID_CHAT_LOGIN 				    = 1005, 		// 对应Client;验证uid & token
    ID_CHAT_LOGIN_RSP 			    = 1006, 		// 对应Client
    ID_SEARCH_USER_REQ 			    = 1007, 
    ID_SEARCH_USER_RSP 			    = 1008, 
    ID_ADD_FRIEND_REQ 			    = 1009,
    ID_ADD_FRIEND_RSP  			    = 1010,
    ID_NOTIFY_ADD_FRIEND_REQ 	    = 1011, 
    ID_AUTH_FRIEND_REQ 			    = 1013,
    ID_AUTH_FRIEND_RSP 			    = 1014, 
    ID_NOTIFY_AUTH_FRIEND_REQ 	    = 1015, 
    ID_TEXT_CHAT_MSG_REQ 		    = 1017, 
    ID_TEXT_CHAT_MSG_RSP 		    = 1018, 
    ID_NOTIFY_TEXT_CHAT_MSG_REQ     = 1019, 
};

/**
 * @brief GateServer内部错误
 */
enum ErrorCodes {
    SUCCESS = 0,
    ERROR_JSON = 1001,
    ERROR_RPC = 1002,
};

/**
 * @brief GateServer作为grpc客户端得到的错误需返回给前端，应对应grpc
 *        并对应客户端BackEnd错误码，从而正确匹配
 */
enum BackendErrorCodes {
    ERR_TOOMANYREQ = 101, // TODO
    ERR_VARIFY_EXPIRED = 102,
    ERR_VARIFY_ERR = 103,
    ERR_USER_OR_EMAIL_EXISTED = 104,
    ERR_NO_MATCHED_EMAIL = 105,
    ERR_UPDATE_PWD = 106,
    ERR_USER_LOGIN = 107,
    ERR_EMAIL_NOT_EXITED = 108,
    ERR_PWD_NOT_MATCH = 109,
    ERR_GET_CAHT_SERVER = 110,
    ERR_UID_INVALID = 111,
    ERR_TOKEN_INVALID = 112,
    ERR_TOKEN_NOT_MATCH = 113
};

/**
 * @brief MysqlDaoErrorCodes
 */
enum MysqlDaoErrorCodes {
    SQL_SUCCESS = 0,
    SQL_ERR_EMAIL_NOT_EXITED = 11, // TODO
    SQL_ERR_PWD_NOT_MATCH = 12,
    SQL_ERR_UID_NOT_EXITED = 13
};

/**
 * @brief Defer
 */
class Defer {
  public:
    Defer(std::function<void()> func) : _func(func) {}
    ~Defer() { _func(); }

  private:
    std::function<void()> _func;
};

#endif // __SERVER_BASE_H_