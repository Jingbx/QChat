/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/mysql_manager.cpp
 * @date    : 2024-12-17 17:41:00
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-08 23:37:49
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#include "mysql_manager.h"

MysqlManager::MysqlManager() { _dao.reset(new MysqlDao()); }

MysqlManager::~MysqlManager() {}

int MysqlManager::registerUser(const std::string &user,
                               const std::string &email,
                               const std::string &pass) {
    return _dao->registerUser(user, email, pass);
}

bool MysqlManager::checkEmail(const std::string &user,
                              const std::string &email) {
    return _dao->checkEmail(user, email);
}

bool MysqlManager::updatePwd(const std::string &user,
                             const std::string &new_pwd) {
    return _dao->updatePwd(user, new_pwd);
}

int MysqlManager::checkPwd(const std::string &email, std::string &password,
                           MysqlUserInfo &user_info) {
    return _dao->checkPwd(email, password, user_info);
}

int MysqlManager::getMysqlUserInfo(const int &uid, MysqlUserInfo &user_info) {
    return _dao->getMysqlUserInfo(uid, user_info);
}

std::shared_ptr<UserInfo> MysqlManager::getUser(int uid) {
    return _dao->getUser(uid);
}

std::shared_ptr<UserInfo> MysqlManager::getUser(std::string name) {
    return _dao->getUser(name);
}

bool MysqlManager::addFriendApply(const int &from_uid, const int &to_uid) {
    return _dao->addFriendApply(from_uid, to_uid);
}

bool MysqlManager::authFriendApply(const int &from_uid, const int &to_uid) {
    return _dao->authFriendApply(from_uid, to_uid);
}

bool MysqlManager::addFriend(const int& from, const int& to, std::string back_name) {
	return _dao->addFriend(from, to, back_name);
}

bool MysqlManager::getApplyList(int touid, 
                                std::vector<std::shared_ptr<ApplyInfo>> &applyList, 
                                int begin,int limit) {
    return _dao->getApplyList(touid, applyList, begin, limit);
}

bool MysqlManager::getFriendList(int self_id, 
                                 std::vector<std::shared_ptr<UserInfo>> &user_info) {
    return _dao->getFriendList(self_id, user_info);
}
