/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/mysql_manager.h
 * @date    : 2024-12-17 17:40:58
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : 管理MysqlDao的操作 单例模式
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
#include "mysql_dao.h"

class MysqlManager : public SingleTon<MysqlManager> {
    friend class SingleTon<MysqlManager>;

  public:
    ~MysqlManager();
    int registerUser(const std::string &user, const std::string &email,
                     const std::string &pass);

    bool checkEmail(const std::string &user, const std::string &email);
    bool updatePwd(const std::string &user, const std::string &new_pwd);

    int checkPwd(const std::string &email, std::string &password,
                 UserInfo &user_info);

  private:
    MysqlManager();

  private:
    std::unique_ptr<MysqlDao> _dao;
};