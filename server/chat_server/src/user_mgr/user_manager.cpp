/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/user_mgr/user_manager.cpp
 * @date    : 2024-12-23 14:04:59
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-31 18:26:18
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#include "user_manager.h"

UserManager::UserManager() {
    
}

UserManager::~UserManager() {
    
}

void UserManager::bindUidAndSession(const int &uid,
                                    std::shared_ptr<CSession> session) {
    std::lock_guard<std::mutex> lock(_mtx);
    _uid2map[uid] = session;                        
}

std::shared_ptr<CSession> UserManager::getSessionFromUid(const int &uid) {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_uid2map.find(uid) != _uid2map.end()) {
        return _uid2map[uid];
    }
    return nullptr;
}

void UserManager::rmvUserSession(const int &uid) {
    std::lock_guard<std::mutex> lock(_mtx);
    _uid2map.erase(uid);
}
