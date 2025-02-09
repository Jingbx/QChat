/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/gate_server/src/mysql_server/mysql_dao.h
 * @date    : 2024-12-16 16:48:08
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-23 15:30:17
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#ifndef __MYSQL_DAO_H_
#define __MYSQL_DAO_H_

#include "../../server_base/src/include/base.h"
#include "../../server_base/src/include/mysql_connection_pool.h"

struct UserInfo {
    int uid;
    std::string name;
    std::string pwd;
    std::string email;
};

class MysqlDao : public SingleTon<MysqlDao> {

  public:
    MysqlDao();
    ~MysqlDao();
    /**
     * @brief 注册：返回是uid/错误码0已存在，-1Mysql指令执行错误
     */
    int registerUser(const std::string &user, const std::string &email,
                     const std::string &pass);
    /**
     * @brief 重置密码
     */
    bool checkEmail(const std::string &user, const std::string &email);
    bool updatePwd(const std::string &user, const std::string &new_pwd);

    /**
     * @brief 登录验证
     */
    int checkPwd(const std::string &email, const std::string &password,
                 UserInfo &user_info);

  private:
    std::unique_ptr<MysqlConnectionPool> _conns_pool;
};

#endif // __MYSQL_DAO_H_