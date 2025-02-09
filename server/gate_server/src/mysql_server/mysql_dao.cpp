/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/mysql_dao.cpp
 * @date    : 2024-12-16 16:48:11
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-20 10:32:09
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#include "mysql_dao.h"
#include "config/conf.h"

extern GeneralConf *gate_config;

MysqlDao::MysqlDao() {

    _conns_pool.reset(new MysqlConnectionPool(
        gate_config->mysql_conn_pool_size, gate_config->mysql_host,
        gate_config->mysql_port, gate_config->mysql_user,
        gate_config->mysql_pass, gate_config->mysql_schema));
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
                       UserInfo &user_info) {
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
