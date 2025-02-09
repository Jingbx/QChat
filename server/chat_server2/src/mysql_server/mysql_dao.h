/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/mysql_dao.h
 * @date    : 2024-12-16 16:48:08
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-08 23:01:55
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#ifndef __MYSQL_DAO_H_
#define __MYSQL_DAO_H_

#include "../../server_base/src/include/base.h"
#include "../../server_base/src/include/mysql_connection_pool.h"
#include "user_mgr/user_data.h"

struct MysqlUserInfo {
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
                 MysqlUserInfo &user_info);

    int getMysqlUserInfo(int uid, MysqlUserInfo &user_info);

    std::shared_ptr<UserInfo> getUser(int uid);
    std::shared_ptr<UserInfo> getUser(std::string name);

    bool addFriendApply(const int &from_uid, const int &to_uid);
    /**
     * @brief 更新friend_apply中的状态
     */
    bool authFriendApply(const int &from_uid, const int &to_uid);

    bool addFriend(const int& from, const int& to, std::string back_name);

    // 加好友的过程添加到sql中，state表示是否同意状态
	  bool getApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit);
	  bool getFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info_list);

  private:
    std::unique_ptr<MysqlConnectionPool> _conns_pool;
};

#endif // __MYSQL_DAO_H_