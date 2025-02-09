/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/mysql_manager.cpp
 * @date    : 2024-12-17 17:41:00
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-23 16:15:43
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
                           UserInfo &user_info) {
    return _dao->checkPwd(email, password, user_info);
}
