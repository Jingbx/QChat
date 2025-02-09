/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/mysql_server/mysql_dao.cpp
 * @date    : 2024-12-16 16:48:11
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-08 23:47:39
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2025 All Right Reserved, JBX
 */

#include "mysql_dao.h"
#include "config/conf.h"

extern GeneralConf *chat_config;

MysqlDao::MysqlDao() {

    _conns_pool.reset(new MysqlConnectionPool(
        chat_config->mysql_conn_pool_size, chat_config->mysql_host,
        chat_config->mysql_port, chat_config->mysql_user,
        chat_config->mysql_pass, chat_config->mysql_schema));
}

MysqlDao::~MysqlDao() {}

int MysqlDao::registerUser(const std::string &user, const std::string &email,
                           const std::string &pass) {
    auto conn = _conns_pool->getConnection();
    try {
        if (conn == nullptr) {
            return false;
        }
        // 准备调用存储过程
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->_conn->prepareStatement("CALL reg_user(?,?,?,@result)"));
        // 设置输入参数
        stmt->setString(1, user);
        stmt->setString(2, email);
        stmt->setString(3, pass);

        // 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值
        // 执行存储过程
        stmt->execute();
        // 如果存储过程设置了会话变量或有其他方式获取输出参数的值，你可以在这里执行SELECT查询来获取它们
        // 例如，如果存储过程设置了一个会话变量@result来存储输出结果，可以这样获取：
        std::unique_ptr<sql::Statement> stmtResult(
            conn->_conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            stmtResult->executeQuery("SELECT @result AS result"));
        if (res->next()) {
            int result = res->getInt("result");
            std::cout << "Result: " << result << std::endl;
            _conns_pool->recycleConnection(std::move(conn));
            return result;
        }
        _conns_pool->recycleConnection(std::move(conn));
        return -1;
    } catch (sql::SQLException &e) {
        _conns_pool->recycleConnection(std::move(conn));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return -1;
    }
}

bool MysqlDao::checkEmail(const std::string &user, const std::string &email) {
    auto conn = _conns_pool->getConnection();
    try {
        if (conn == nullptr) {
            return false;
        }

        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->_conn->prepareStatement(
                "SELECT email FROM user WHERE name = ?"));

        stmt->setString(1, user);

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        while (res->next()) {
            if (email != res->getString("email")) {
                std::cout << "Email is not match" << std::endl;
                _conns_pool->recycleConnection(std::move(conn));
                return false;
            }
            _conns_pool->recycleConnection(std::move(conn));
            return true;
        }
        return false;
    } catch (sql::SQLException &e) {
        _conns_pool->recycleConnection(std::move(conn));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::updatePwd(const std::string &user, const std::string &new_pwd) {
    auto conn = _conns_pool->getConnection();
    try {
        if (conn == nullptr) {
            return false;
        }

        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->_conn->prepareStatement(
                "UPDATE user SET pwd = ? WHERE name = ?"));

        stmt->setString(1, new_pwd);
        stmt->setString(2, user);

        int update_count = stmt->executeUpdate();
        std::cout << "Update row: " << update_count << std::endl;
        _conns_pool->recycleConnection(std::move(conn));
        return true;

    } catch (sql::SQLException &e) {
        _conns_pool->recycleConnection(std::move(conn));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

int MysqlDao::checkPwd(const std::string &email, const std::string &password,
                       MysqlUserInfo &user_info) {
    auto conn = _conns_pool->getConnection();
    Defer defer([this, &conn]() {
        this->_conns_pool->recycleConnection(std::move(conn));
    });
    try {
        if (conn == nullptr) {
            return -1;
        }

        // 扩展查询字段，除了密码还需要获取其他用户信息
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->_conn->prepareStatement(
                "SELECT uid, name, pwd FROM user WHERE email = ?"));
        stmt->setString(1, email);

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        if (!res->next()) {
            std::cout << "Mysql select " << email << " error" << std::endl;
            return MysqlDaoErrorCodes::SQL_ERR_EMAIL_NOT_EXITED;
        }

        // 只需要检查密码
        if (password != res->getString("pwd")) {
            std::cout << "Password does not match" << std::endl;
            return MysqlDaoErrorCodes::SQL_ERR_PWD_NOT_MATCH;
        } else {
            // 填充用户信息
            user_info.uid = res->getInt("uid");
            user_info.name = res->getString("name");
            user_info.email = email;
            user_info.pwd = res->getString("pwd");

            return MysqlDaoErrorCodes::SQL_SUCCESS; // 成功
        }

    } catch (sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return -1; // 使用定义的错误码
    }
}

int MysqlDao::getMysqlUserInfo(int uid, MysqlUserInfo &user_info) {
    // TODO: 查询并返回
    auto conn = _conns_pool->getConnection();
    Defer defer([this, &conn]() {
        this->_conns_pool->recycleConnection(std::move(conn));
    });
    try {
        if (conn == nullptr) {
            return -1;
        }
        std::cout << "======= Mysql select uid=" << uid
                  << "=======" << std::endl;
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->_conn->prepareStatement("SELECT * FROM user WHERE uid = ?"));
        pstmt->setInt(1, uid);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (!res->next()) {
            std::cout << "Mysql select " << uid << " uid not found"
                      << std::endl;
            return MysqlDaoErrorCodes::SQL_ERR_UID_NOT_EXITED;
        }

        // 填充用户信息
        user_info.uid = res->getInt("uid");
        user_info.name = res->getString("name");
        user_info.email = res->getString("email");

        // 成功返回
        std::cout << "======= Mysql select user=" << user_info.name
                  << "=======" << std::endl;

        return MysqlDaoErrorCodes::SQL_SUCCESS; // 成功
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return -1; // 使用定义的错误码
    }
}

std::shared_ptr<UserInfo> MysqlDao::getUser(int uid) {
    auto conn = _conns_pool->getConnection();
    Defer defer([this, &conn]() {
        this->_conns_pool->recycleConnection(std::move(conn));
    });

    try {
        if (conn == nullptr) {
            return nullptr;
        }
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->_conn->prepareStatement("SELECT * FROM user WHERE uid = ?"));
        pstmt->setInt(1, uid);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        std::shared_ptr<UserInfo> user_ptr = nullptr;
        while (res->next()) { // 换行
            user_ptr.reset(new UserInfo);
            user_ptr->email = res->getString("email");
            user_ptr->user = res->getString("name");
            user_ptr->nick = res->getString("nick");
            user_ptr->desc = res->getString("desc");
            user_ptr->sex = res->getInt("sex");
            user_ptr->icon = res->getString("icon");
            user_ptr->uid = uid;
            break;
        }
        return user_ptr;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return nullptr; // 使用定义的错误码
    }
}

std::shared_ptr<UserInfo> MysqlDao::getUser(std::string name) {
    auto conn = _conns_pool->getConnection();
    Defer defer([this, &conn]() {
        this->_conns_pool->recycleConnection(std::move(conn));
    });

    try {
        if (conn == nullptr) {
            return nullptr;
        }
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->_conn->prepareStatement("SELECT * FROM user WHERE name = ?"));
        pstmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        std::shared_ptr<UserInfo> user_ptr = nullptr;
        while (res->next()) { // 换行
            user_ptr.reset(new UserInfo);
            user_ptr->uid = res->getInt("uid");
            user_ptr->user = name;
            user_ptr->email = res->getString("email");
            user_ptr->nick = res->getString("nick");
            user_ptr->desc = res->getString("desc");
            user_ptr->sex = res->getInt("sex");
            user_ptr->icon = res->getString("icon");
            break;
        }
        return user_ptr;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return nullptr; // 使用定义的错误码
    }
}

bool MysqlDao::addFriendApply(const int &from_uid, const int &to_uid) {
    auto conn = _conns_pool->getConnection();
    if (!conn) {
        return false;
    }
    Defer defer([this, &conn]() {
        this->_conns_pool->recycleConnection(std::move(conn));
    });

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->_conn->prepareStatement(
                "INSERT INTO friend_apply (from_uid, to_uid) values (?,?) "
                "ON DUPLICATE KEY UPDATE from_uid = from_uid, to_uid = "
                "to_uid"));
        pstmt->setInt(1, from_uid);
        pstmt->setInt(2, to_uid);

        // 执行更新
        int rowAffected = pstmt->executeUpdate();
        if (rowAffected < 0) {
            return false;
        }
        return true;
    } catch (const sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
    return true;
}

bool MysqlDao::authFriendApply(const int &from_uid, const int &to_uid) {
    auto con = _conns_pool->getConnection();
    if (con == nullptr) {
        return false;
    }

    Defer defer(
        [this, &con]() { _conns_pool->recycleConnection(std::move(con)); });

    try {
        // 准备SQL语句
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->_conn->prepareStatement("UPDATE friend_apply SET status = 1 "
                                         "WHERE from_uid = ? AND to_uid = ?"));
        // 反过来的申请时from，验证时to
        pstmt->setInt(1, to_uid); // from id
        pstmt->setInt(2, from_uid);
        // 执行更新
        int rowAffected = pstmt->executeUpdate();
        if (rowAffected < 0) {
            return false;
        }
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }

    return true;
}

bool MysqlDao::addFriend(const int &from, const int &to,
                         std::string back_name) {
    auto con = _conns_pool->getConnection();
    if (con == nullptr) {
        return false;
    }

    Defer defer(
        [this, &con]() { _conns_pool->recycleConnection(std::move(con)); });

    try {
        // 开始事务
        con->_conn->setAutoCommit(false);

        // 准备第一个SQL语句, 插入认证方好友数据
        // 给 好友请求发起者（from）
        // 增加一条好友记录，表明对方（to）是其好友，同时备注（back）为
        // back_name。
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->_conn->prepareStatement(
                "INSERT IGNORE INTO friend(self_id, friend_id, back) "
                "VALUES (?, ?, ?) "));
        // 反过来的申请时from，验证时to
        pstmt->setInt(1, from); // from id
        pstmt->setInt(2, to);
        pstmt->setString(3, back_name);
        // 执行更新
        int rowAffected = pstmt->executeUpdate();
        if (rowAffected < 0) {
            con->_conn->rollback();
            return false;
        }

        // 准备第二个SQL语句，插入申请方好友数据
        //  给 好友请求接收者（to）
        //  增加一条记录，表明对方（from）是其好友，备注（back）为空。
        std::unique_ptr<sql::PreparedStatement> pstmt2(
            con->_conn->prepareStatement(
                "INSERT IGNORE INTO friend(self_id, friend_id, back) "
                "VALUES (?, ?, ?) "));
        // 反过来的申请时from，验证时to
        pstmt2->setInt(1, to); // from id
        pstmt2->setInt(2, from);
        pstmt2->setString(3, "");
        // 执行更新
        int rowAffected2 = pstmt2->executeUpdate();
        if (rowAffected2 < 0) {
            con->_conn->rollback();
            return false;
        }

        // 提交事务
        con->_conn->commit();
        std::cout << "addfriend insert friends success" << std::endl;

        return true;
    } catch (sql::SQLException &e) {
        // 如果发生错误，回滚事务
        if (con) {
            con->_conn->rollback();
        }
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
    return true;
}

bool MysqlDao::getApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit) {
    auto con = _conns_pool->getConnection();
    if (con == nullptr) {
        return false;
    }

    Defer defer([this, &con]() { _conns_pool->recycleConnection(std::move(con)); });

    try {
        // 准备SQL语句, 根据起始id和限制条数返回列表
        std::unique_ptr<sql::PreparedStatement> pstmt(con->_conn->prepareStatement(
            "select apply.from_uid, apply.status, user.name, user.nick, user.sex "
            "from friend_apply as apply join user on apply.from_uid = user.uid "
            "where apply.to_uid = ? and apply.id > ? order by apply.id ASC LIMIT ?"
        ));

        pstmt->setInt(1, touid); // 将uid替换为你要查询的uid
        pstmt->setInt(2, begin); // 起始id
        pstmt->setInt(3, limit); // 偏移量
        // 执行查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        // 遍历结果集
        while (res->next()) {
            auto name = res->getString("name");
            auto uid = res->getInt("from_uid");
            auto status = res->getInt("status");
            auto nick = res->getString("nick");
            auto sex = res->getInt("sex");
            auto apply_ptr = std::make_shared<ApplyInfo>(uid, name, "", "",
                                                         nick, sex, status);
            applyList.push_back(apply_ptr);
        }
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}



bool MysqlDao::getFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info_list) {

    auto con = _conns_pool->getConnection();
    if (con == nullptr) {
        return false;
    }

    Defer defer(
        [this, &con]() { _conns_pool->recycleConnection(std::move(con)); });

    try {
        // 准备SQL语句, 根据起始id和限制条数返回列表
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->_conn->prepareStatement(
                "select * from friend where self_id = ? "));

        pstmt->setInt(1, self_id); // 将uid替换为你要查询的uid

        // 执行查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        // 遍历结果集
        while (res->next()) {
            auto friend_id = res->getInt("friend_id");
            auto back = res->getString("back");
            // 再一次查询friend_id对应的信息
            auto user_info = getUser(friend_id);
            if (user_info == nullptr) {
                continue;
            }

            user_info->back = user_info->user;
            user_info_list.push_back(user_info);
        }
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }

    return true;
}
