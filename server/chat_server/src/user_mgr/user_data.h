/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/user_mgr/user_data.h
 * @date    : 2024-12-31 16:39:30
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-31 16:46:46
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#ifndef __USER_DATA_H_
#define __USER_DATA_H_

#include <string>

struct UserInfo {
    UserInfo()
        : user(""), uid(0), email(""), nick(""), desc(""), sex(0), icon(""),
          back("") {}
    int uid;
    std::string user;
    std::string email;
    std::string nick;
    std::string desc;
    int sex;
    std::string icon;
    std::string back;
};

struct ApplyInfo {
    ApplyInfo(int uid, std::string name, std::string desc, std::string icon,
              std::string nick, int sex, int status)
        : _uid(uid), _name(name), _desc(desc), _icon(icon), _nick(nick),
          _sex(sex), _status(status) {}

    int _uid;
    std::string _name;
    std::string _desc;
    std::string _icon;
    std::string _nick;
    int _sex;
    int _status;
};

#endif // __USER_DATA_H_