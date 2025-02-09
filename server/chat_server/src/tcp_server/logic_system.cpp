/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/tcp_server/logic_system.cpp
 * @date    : 2024-12-21 16:46:15
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-09 22:20:34
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#include "logic_system.h"
#include "../config/conf.h"
#include "../grpc_client/chat_grpc_client.h"
#include "../user_mgr/user_manager.h"
#include "csession.h"
#include "mysql_server/mysql_manager.h"
#include "redis_server/redis_manager.h"

#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>

#include "../config/conf.h"
#include <yaml-cpp/yaml.h>

extern GeneralConf *chat_config;

bool isPureDigit(std::string str) {
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

LogicSystem::~LogicSystem() {
    _b_stop = true;

    _consume_cv.notify_one();

    if (_consume_thread.joinable()) {
        _consume_thread.join();
    }
}

LogicSystem::LogicSystem() : _b_stop(false) {
    _initFuncCallbakes();
    // note: 构造就开启消费线程
    _consume_thread = std::thread(&LogicSystem::_consumeFromQue, this);
}

/**
 * @brief public:
 */
// TODO: Enhanced: 增加_b_stop判断? 需要吗? CSession是否先析构了?
void LogicSystem::pushToQue(std::shared_ptr<LogicNode> msg) {
    std::unique_lock<std::mutex> lock(_produce_mtx);

    // 如果队列已满，等待消费者处理并释放空间
    _produce_cv.wait(lock, [this]() {
        return this->_msg_que.size() <
               MAX_RECVQUE; // fixed: 当队列不满时返回true，允许继续push
    });

    // 将消息推入队列
    _msg_que.push(msg);

    // 如果队列从空变为非空，通知消费者
    if (_msg_que.size() == 1) {
        _consume_cv.notify_one();
    }
}

/**
 * @brief private:
 */
// callback handler
void LogicSystem::_initFuncCallbakes() {
    _func_callbacks[MSG_IDS::ID_CHAT_LOGIN] =
        std::bind(&LogicSystem::_handleChatLogin, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);

    _func_callbacks[MSG_IDS::ID_SEARCH_USER_REQ] =
        std::bind(&LogicSystem::_handleSearchUser, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);

    _func_callbacks[MSG_IDS::ID_ADD_FRIEND_REQ] = std::bind(
        &LogicSystem::_handleAddFriendApply, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);

    _func_callbacks[MSG_IDS::ID_AUTH_FRIEND_REQ] = std::bind(
        &LogicSystem::_handleAuthFriendApply, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);

    _func_callbacks[MSG_IDS::ID_TEXT_CHAT_MSG_REQ] = std::bind(
        &LogicSystem::_handleDealChatTextMsg, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
}

void LogicSystem::_handleChatLogin(std::shared_ptr<CSession> session,
                                   const short msg_id,
                                   const std::string &msg_data) {
    // 解析请求 并验证uid & token 并回包ID_CHAT_LOGIN_RSP
    Json::Reader reader;
    Json::Value req_root;
    Json::Value resp_root;
    bool ret = reader.parse(msg_data, req_root);
    if (!ret) {
        std::cout << "Failed to parse req, id=" << msg_id << std::endl;
        return;
    }

    auto uid = req_root["uid"].asInt();
    auto token = req_root["token"].asString();

    // Defer 在此处仅负责序列化和发送
    Defer defer([&]() {
        // 序列化
        std::string resp_str = resp_root.toStyledString();
        // 发送LOGIN回包
        session->send(MSG_IDS::ID_CHAT_LOGIN_RSP, resp_str);
    });
    // 1. 检查token是否对应
    // check redis
    std::string gt_token = "";
    ret = RedisManager::GetInstance()->Get(
        USERTOKENPREFIX + std::to_string(uid), gt_token);
    // return resp
    if (!ret) {
        std::cout << "HandleChatLogin redis GET uid error" << std::endl;
        resp_root["error"] = BackendErrorCodes::ERR_UID_INVALID;
        return;
    }
    if (gt_token != token) {
        std::cout << "HandleChatLogin redis token not match" << std::endl;
        resp_root["error"] = BackendErrorCodes::ERR_TOKEN_NOT_MATCH;
        return;
    }
    // 2. 获取User信息
    resp_root["error"] = ErrorCodes::SUCCESS;
    // TODO: 先从Redis中查找UserInfo没有再从Mysql中查找并设置 ✅
    std::string base_key = USER_BASE_INFO + std::to_string(uid);
    auto user_info = std::make_shared<UserInfo>();
    auto b_base = GetUserInfoFromDataBase(base_key, uid, user_info);
    if (!b_base) {
        std::cout << "GetUserInfoFromDataBase error, uid invalid: " << uid
                  << std::endl;
        resp_root["error"] = BackendErrorCodes::ERR_UID_INVALID;
        return;
    }
    resp_root["uid"] = uid;
    resp_root["name"] = user_info->user;
    resp_root["token"] = token;
    resp_root["email"] = user_info->email;
    resp_root["nick"] = user_info->nick;
    resp_root["desc"] = user_info->desc;
    resp_root["sex"] = user_info->sex;
    resp_root["icon"] = user_info->icon;
    std::cout << "==============================" << std::endl;
    std::cout << "icon " << user_info->icon << " name " << user_info->user;

    // TODO: 从数据库中查找申请者列表  ✅
    std::vector<std::shared_ptr<ApplyInfo>> apply_list;
    auto b_apply = getFriendApplyInfo(uid, apply_list);
    if (b_apply) {
        for (auto &apply : apply_list) {
            Json::Value obj;
            obj["name"] = apply->_name;
            obj["uid"] = apply->_uid;
            obj["icon"] = apply->_icon;
            obj["nick"] = apply->_nick;
            obj["sex"] = apply->_sex;
            obj["desc"] = apply->_desc;
            obj["status"] = apply->_status;
            resp_root["apply_list"].append(obj);
        }
    }
    // TODO: 获取好友列表
    std::vector<std::shared_ptr<UserInfo>> friend_list;
    bool b_friend_list = getFriendList(uid, friend_list);
    for (auto& friend_ele : friend_list) {
    	Json::Value obj;
    	obj["name"] = friend_ele->user;
    	obj["uid"] = friend_ele->uid;
    	obj["icon"] = friend_ele->icon;
    	obj["nick"] = friend_ele->nick;
    	obj["sex"] = friend_ele->sex;
    	obj["desc"] = friend_ele->desc;
    	obj["back"] = friend_ele->back;
    	resp_root["friend_list"].append(obj);
    }

    // TODO: 增加对应登录服务器的计数 ✅
    std::string login_cnt_str = "";
    int login_cnt = 0;
    RedisManager::GetInstance()->HGet(
        LOGIN_COUNT, chat_config->chat_self_server_name, login_cnt_str);
    if (!login_cnt_str.empty()) {
        // 存在登录计数
        login_cnt = std::stoi(login_cnt_str);
    }
    login_cnt++;
    auto cnt_str = std::to_string(login_cnt);
    RedisManager::GetInstance()->HSet(LOGIN_COUNT, chat_config->chat_self_server_name, cnt_str);
    // TODO: 绑定uid和session 绑定uid和对应的服务器名字 ✅
    session->setUid(uid);
    // 为用户设置登录ip server的名字
    std::string ipkey = USERIPPREFIX + std::to_string(uid);
    RedisManager::GetInstance()->Set(ipkey, chat_config->chat_self_server_name);
    // uid和session绑定管理,方便以后踢人操作
    UserManager::GetInstance()->bindUidAndSession(uid, session);
}

void LogicSystem::_handleSearchUser(std::shared_ptr<CSession> session,
                                    const short msg_id,
                                    const std::string &msg_data) {
    // 解析请求 并验证uid & token 并回包ID_CHAT_LOGIN_RSP
    Json::Reader reader;
    Json::Value req_root;
    Json::Value resp_root;
    bool ret = reader.parse(msg_data, req_root);
    if (!ret) {
        std::cout << "Failed to parse req, id=" << msg_id << std::endl;
        return;
    }
    auto uid_str = req_root["uid"].asString();

    // Defer 在此处仅负责序列化和发送
    Defer defer([&]() {
        // 序列化
        std::string resp_str = resp_root.toStyledString();
        // 发送LOGIN回包
        session->send(MSG_IDS::ID_SEARCH_USER_RSP, resp_str);
    });

    // fixed: 根据“uid”是全数字/非全数字来不同的查询方式
    bool is_pure_dig = isPureDigit(uid_str);
    std::string base_key = USER_BASE_INFO + uid_str;
    std::string name_key = NAME_INFO + uid_str;
    auto user_info = std::make_shared<UserInfo>();
    bool is_exited = false;
    if (is_pure_dig) {
        // 查询uid & name
        is_exited =
            GetUserInfoFromDataBase(base_key, std::stoi(uid_str), user_info);
        if (!is_exited) {
            is_exited = GetUserInfoFromDataBase(base_key, uid_str,
                                                user_info); // search name
        }
    } else {
        // 查询name
        is_exited = GetUserInfoFromDataBase(base_key, uid_str,
                                            user_info); // search name
    }
    if (!is_exited) {
        std::cout << "GetUserInfoFromDataBase error, user invalid: " << uid_str
                  << std::endl;
        resp_root["error"] = BackendErrorCodes::ERR_UID_INVALID;
        return;
    }

    resp_root["error"] = ErrorCodes::SUCCESS;
    resp_root["uid"] = user_info->uid;
    resp_root["name"] = user_info->user;
    resp_root["email"] = user_info->email;
    resp_root["nick"] = user_info->nick;
    resp_root["desc"] = user_info->desc;
    resp_root["sex"] = user_info->sex;
    resp_root["icon"] = user_info->icon;
    return;
}

void LogicSystem::_handleAddFriendApply(std::shared_ptr<CSession> session,
                                        const short msg_id,
                                        const std::string &msg_data) {
    // TODO: 增加离线情况，直接存到数据库对方登录再显示
    // 解析请求 并验证uid & token 并回包ID_CHAT_LOGIN_RSP
    Json::Reader reader;
    Json::Value req_root;
    Json::Value resp_root;
    bool ret = reader.parse(msg_data, req_root);
    if (!ret) {
        std::cout << "Failed to parse req, id=" << msg_id << std::endl;
        return;
    }
    auto uid = req_root["uid"].asInt();
    auto applyname = req_root["applyname"].asString();
    auto bakname = req_root["bakname"].asString();
    auto touid = req_root["touid"].asInt();
    std::cout << "AddFriendApply peer user: uid is  " << uid
              << " applyname  is " << applyname << " bakname is " << bakname
              << " touid is " << touid << std::endl;

    // note: 发到这就是成功
    resp_root["error"] = ErrorCodes::SUCCESS;
    // Defer 在此处仅负责序列化和发送
    Defer defer([&]() {
        // 序列化
        std::string resp_str = resp_root.toStyledString();
        // 发送LOGIN回包
        session->send(MSG_IDS::ID_ADD_FRIEND_RSP, resp_str);
    });
    // 1.更新数据库friend_apply关系
    MysqlManager::GetInstance()->addFriendApply(uid, touid);
    // 2.Redis查找Login时候存储的to_uid chatserver name(session)关系
    std::string peer_server_key = USERIPPREFIX + std::to_string(touid);
    std::string chat_server_name = "";
    bool is_exited =
        RedisManager::GetInstance()->Get(peer_server_key, chat_server_name);
    if (!is_exited)
        return;
    // 3.to_uid是本服务器？   直接内存发送(两种信号)
    std::string base_key = USER_BASE_INFO + std::to_string(uid);
    auto apply_info = std::make_shared<UserInfo>();
    bool b_info = GetUserInfoFromDataBase(base_key, uid, apply_info);

    YAML::Node config = YAML::LoadFile("conf/chat_server.yaml");
    std::string self_server_name =
        config["selfserver"]["name"].as<std::string>();

    if (self_server_name == chat_server_name) {
        // note: 对方离线不发送...
        auto peer_session =
            UserManager::GetInstance()->getSessionFromUid(touid);
        if (peer_session) {
            Json::Value notify_req;
            notify_req["error"] = ErrorCodes::SUCCESS;
            notify_req["applyuid"] = uid;
            notify_req["name"] = applyname;
            notify_req["desc"] = "";
            if (b_info) {
                notify_req["icon"] = apply_info->icon;
                notify_req["sex"] = apply_info->sex;
                notify_req["nick"] = apply_info->nick;
            } else {
                std::cout << "hhjgjjkhkljh" << std::endl;
                notify_req["error"] = BackendErrorCodes::ERR_UID_INVALID;
            }
            std::string notify_req_str = notify_req.toStyledString();
            session->send(ID_NOTIFY_ADD_FRIEND_REQ, notify_req_str);
        } else {
            std::cout << "Touid's session is not exited, peer outline..."
                      << std::endl;
        }
        return;
    }
    // 4.to_uid是其他服务器？  grpc通信 stub远程调用
    std::cout << "通过Grpc进行Chatserver通讯" << std::endl;
    AddFriendReq grpc_req;
    grpc_req.set_applyuid(uid);
    grpc_req.set_touid(touid);
    grpc_req.set_name(applyname);
    grpc_req.set_desc("");
    if (b_info) {
        grpc_req.set_icon(apply_info->icon);
        grpc_req.set_sex(apply_info->sex);
        grpc_req.set_nick(apply_info->nick);
    }
    // peer server 发送通知
    ChatGrpcClient::GetInstance()->NotifyAddFriend(chat_server_name, grpc_req);
    return;
}

void LogicSystem::_handleAuthFriendApply(std::shared_ptr<CSession> session,
                                         const short msg_id,
                                         const std::string &msg_data) {
    // TODO: 增加离线情况，直接存到数据库对方登录再显示
    // 解析请求 并验证uid & token 并回包ID_CHAT_LOGIN_RSP
    Json::Reader reader;
    Json::Value req_root;
    Json::Value resp_root;
    bool ret = reader.parse(msg_data, req_root);
    if (!ret) {
        std::cout << "Failed to parse req, id=" << msg_id << std::endl;
        return;
    }
    auto uid = req_root["fromuid"].asInt();
    auto touid = req_root["touid"].asInt();
    auto back_name = req_root["back"].asString();
    std::cout << "handleAuthFriendApply1 from " << uid << " auth friend to "
              << touid << std::endl;

    // note: 发到这就是成功
    resp_root["error"] = ErrorCodes::SUCCESS;
    auto user_info = std::make_shared<UserInfo>();

    std::string base_key = USER_BASE_INFO + std::to_string(touid);
    bool b_info = GetUserInfoFromDataBase(base_key, touid, user_info);
    if (b_info) {
        resp_root["name"] = user_info->user;
        resp_root["nick"] = user_info->nick;
        resp_root["icon"] = user_info->icon;
        resp_root["sex"] = user_info->sex;
        resp_root["uid"] = touid;
    } else {
        std::cout << "11234" << std::endl;
        resp_root["error"] = BackendErrorCodes::ERR_UID_INVALID;
    }
    // Defer 在此处仅负责序列化和发送
    Defer defer([&]() {
        // 序列化
        std::string resp_str = resp_root.toStyledString();
        // 发送LOGIN回包
        session->send(MSG_IDS::ID_AUTH_FRIEND_RSP, resp_str);
    });
    // 1.note: 更新数据库friend_apply state属性
    MysqlManager::GetInstance()->authFriendApply(uid, touid);
    // 2.note: 更新数据库添加好友 增加friend表成员
    MysqlManager::GetInstance()->addFriend(uid, touid, back_name);

    // 3.Redis查找Login时候存储的to_uid chatserver name(session)关系
    std::string peer_server_key = USERIPPREFIX + std::to_string(touid);
    std::string chat_server_name = "";
    bool is_exited =
        RedisManager::GetInstance()->Get(peer_server_key, chat_server_name);
    if (!is_exited)
        return;
    // 4.to_uid是本服务器？   直接内存发送(两种信号)
    YAML::Node config = YAML::LoadFile("conf/chat_server.yaml");
    std::string self_server_name =
        config["selfserver"]["name"].as<std::string>();

    if (self_server_name == chat_server_name) {
        // note: 对方离线不发送...
        auto peer_session =
            UserManager::GetInstance()->getSessionFromUid(touid);
        if (peer_session) {
            Json::Value notify_req;
            notify_req["error"] = ErrorCodes::SUCCESS;
            notify_req["fromuid"] = uid;
            notify_req["touid"] = touid;
            if (b_info) {
                notify_req["name"] = user_info->user;
                notify_req["nick"] = user_info->nick;
                notify_req["icon"] = user_info->icon;
                notify_req["sex"] = user_info->sex;
            } else {
                std::cout << "2123" << std::endl;
                notify_req["error"] = BackendErrorCodes::ERR_UID_INVALID;
            }
            std::string notify_req_str = notify_req.toStyledString();
            session->send(ID_NOTIFY_AUTH_FRIEND_REQ, notify_req_str);
        } else {
            std::cout << "Touid's session is not exited, peer outline..."
                      << std::endl;
        }
        return;
    }
    // 5.to_uid是其他服务器？  grpc通信 stub远程调用
    std::cout << "通过Grpc进行Chatserver通讯" << std::endl;
    AuthFriendReq grpc_req;
    grpc_req.set_fromuid(uid);
    grpc_req.set_touid(touid);
    // peer server 发送通知
    ChatGrpcClient::GetInstance()->NotifyAuthFriend(chat_server_name, grpc_req);
    return;
}

void LogicSystem::_handleDealChatTextMsg(std::shared_ptr<CSession> session,
                                         const short &msg_id,
                                         const std::string &msg_data) {
    // TODO: 增加离线情况，直接存到数据库对方登录再显示
    // 解析请求 并验证uid & token 并回包ID_CHAT_LOGIN_RSP
    Json::Reader reader;
    Json::Value req_root;
    Json::Value resp_root;
    bool ret = reader.parse(msg_data, req_root);
    if (!ret) {
        std::cout << "Failed to parse req, id=" << msg_id << std::endl;
        return;
    }

    const Json::Value text_array = req_root["text_array"];   // note:
    auto uid = req_root["fromuid"].asInt();
    auto touid = req_root["touid"].asInt();
    
    std::cout << "_handleDealChatTextMsg1 from " << uid << " auth friend to "
              << touid << std::endl;

    // note: 发到这就是成功
    resp_root["error"] = ErrorCodes::SUCCESS;
	resp_root["text_array"] = text_array;
	resp_root["fromuid"] = uid;
	resp_root["touid"] = touid;
    
	Defer defer([this, &resp_root, session]() {
		std::string return_str = resp_root.toStyledString();
		session->send(MSG_IDS::ID_TEXT_CHAT_MSG_RSP, return_str);
		});

    // Redis查找Login时候存储的to_uid chatserver name(session)关系
    std::string peer_server_key = USERIPPREFIX + std::to_string(touid);
    std::string chat_server_name = "";
    bool is_exited =
        RedisManager::GetInstance()->Get(peer_server_key, chat_server_name);
    if (!is_exited)
        return;
    // 4.to_uid是本服务器？   直接内存发送(两种信号)
    YAML::Node config = YAML::LoadFile("conf/chat_server.yaml");
    std::string self_server_name =
        config["selfserver"]["name"].as<std::string>();

    if (self_server_name == chat_server_name) {
        // note: 对方离线不发送...
        auto peer_session = UserManager::GetInstance()->getSessionFromUid(touid);
        if (peer_session) {
            //在内存中则直接发送通知对方
			std::string return_str = resp_root.toStyledString();
			session->send(MSG_IDS::ID_NOTIFY_TEXT_CHAT_MSG_REQ, return_str);
        } else {
            std::cout << "Touid's session is not exited, peer outline..."
                      << std::endl;
        }
        return;
    }
    // 5.to_uid是其他服务器？  grpc通信 stub远程调用
    std::cout << "通过Grpc进行Chatserver通讯" << std::endl;
    TextChatMsgReq grpc_req;
    grpc_req.set_fromuid(uid);
    grpc_req.set_touid(touid);
    // fixed:
	for (const auto& txt_obj : text_array) {
		auto content = txt_obj["content"].asString();
		auto msgid = txt_obj["msgid"].asString();
		std::cout << "content is " << content << std::endl;
		std::cout << "msgid is " << msgid << std::endl;
        // important: 
		auto *text_msg = grpc_req.add_textmsgs();
		text_msg->set_msgid(msgid);
		text_msg->set_msgcontent(content);
	}
    
    // peer server 发送通知
    ChatGrpcClient::GetInstance()->NotifyTextChatMsg(chat_server_name, grpc_req, text_array);
    return;                                            
}







// consume
void LogicSystem::_consumeFromQue() {
    while (true) {
        std::unique_lock<std::mutex> lock(_consume_mtx);
        _consume_cv.wait(lock,
                         [this]() { return _b_stop || !_msg_que.empty(); });

        // 如果接收到停止信号
        if (_b_stop && _msg_que.empty()) {
            break; // 且队列为空时退出
        }

        while (!_msg_que.empty()) {
            auto msg = _msg_que.pop_return();

            auto callback_iter = _func_callbacks.find(msg->_recvnode->_msg_id);
            std::cout << "Consume thread received msg, id="
                      << msg->_recvnode->_msg_id << std::endl;

            if (callback_iter != _func_callbacks.end()) {
                try {
                    callback_iter->second(
                        msg->_session, msg->_recvnode->_msg_id,
                        std::string(
                            msg->_recvnode->_data,
                            msg->_recvnode->_curr_len)); // note: msg_data
                } catch (const std::exception &e) {
                    std::cerr << "Error in callback for msg_id="
                              << msg->_recvnode->_msg_id << ": " << e.what()
                              << std::endl;
                }
            } else {
                std::cerr << "No callback found for msg_id="
                          << msg->_recvnode->_msg_id << std::endl;
            }
        }
    }
}

bool LogicSystem::GetUserInfoFromDataBase(std::string base_key, int uid,
                                          std::shared_ptr<UserInfo> &userinfo) {
    // redis
    std::string info_str = "";
    auto ret = RedisManager::GetInstance()->Get(base_key, info_str);
    if (ret) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        userinfo->uid = root["uid"].asInt();
        userinfo->user = root["name"].asString();
        userinfo->email = root["email"].asString();
        userinfo->nick = root["nick"].asString();
        userinfo->desc = root["desc"].asString();
        userinfo->sex = root["sex"].asInt();
        userinfo->icon = root["icon"].asString();
        std::cout << "GetUserInfoFromDataBase: User login uid is  "
                  << userinfo->uid << " name  is " << userinfo->user
                  << " email is " << userinfo->email << std::endl;
    } else {
        // mysql
        std::shared_ptr<UserInfo> user_info = nullptr;
        user_info = MysqlManager::GetInstance()->getUser(uid);
        if (user_info == nullptr) {
            std::cout << "GetUserInfo from redis and mysql error!" << std::endl;
            return false;
        }
        // important: 修改引用的智能指针，让它指向新的 UserInfo 对象
        userinfo = user_info;
        // 将数据库内容写入redis缓存
        Json::Value redis_root;
        redis_root["uid"] = uid;
        redis_root["name"] = userinfo->user;
        redis_root["email"] = userinfo->email;
        redis_root["nick"] = userinfo->nick;
        redis_root["desc"] = userinfo->desc;
        redis_root["sex"] = userinfo->sex;
        redis_root["icon"] = userinfo->icon;
        RedisManager::GetInstance()->Set(base_key, redis_root.toStyledString());
    }
    return true;
}

bool LogicSystem::GetUserInfoFromDataBase(std::string base_key,
                                          std::string name,
                                          std::shared_ptr<UserInfo> &userinfo) {
    // redis
    std::string info_str = "";
    auto ret = RedisManager::GetInstance()->Get(base_key, info_str);
    if (ret) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        userinfo->uid = root["uid"].asInt();
        userinfo->user = root["name"].asString();
        userinfo->email = root["email"].asString();
        userinfo->nick = root["nick"].asString();
        userinfo->desc = root["desc"].asString();
        userinfo->sex = root["sex"].asInt();
        userinfo->icon = root["icon"].asString();
        std::cout << "GetUserInfoFromDataBase: User login uid is  "
                  << userinfo->uid << " name  is " << userinfo->user
                  << " email is " << userinfo->email << std::endl;
    } else {
        // mysql
        std::shared_ptr<UserInfo> user_info = nullptr;
        user_info = MysqlManager::GetInstance()->getUser(name);
        if (user_info == nullptr) {
            std::cout << "GetUserInfo from redis and mysql error!" << std::endl;
            return false;
        }
        // important: 修改引用的智能指针，让它指向新的 UserInfo 对象
        userinfo = user_info;
        // 将数据库内容写入redis缓存
        Json::Value redis_root;
        redis_root["uid"] = userinfo->uid;
        redis_root["name"] = name;
        redis_root["email"] = userinfo->email;
        redis_root["nick"] = userinfo->nick;
        redis_root["desc"] = userinfo->desc;
        redis_root["sex"] = userinfo->sex;
        redis_root["icon"] = userinfo->icon;
        RedisManager::GetInstance()->Set(base_key, redis_root.toStyledString());
    }
    return true;
}

bool LogicSystem::getFriendApplyInfo(
    int to_uid, std::vector<std::shared_ptr<ApplyInfo>> &list) {
    // 从mysql获取好友申请列表
    return MysqlManager::GetInstance()->getApplyList(to_uid, list, 0, 10);
}

bool LogicSystem::getFriendList(
    int self_id, std::vector<std::shared_ptr<UserInfo>> &user_list) {
    // 从mysql获取好友列表
    return MysqlManager::GetInstance()->getFriendList(self_id, user_list);
}

