/**
 * @file    :/Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/src_base/mysql_connection_pool.h
 * @date    : 2024-12-17 15:40:59
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : 封装MySQL连接池，提高并发
 *            Enhanced: 增加心跳机制，detach一个线程每分钟检测一次连接池中所有链接，封装新链接保存其最后一次操作时间
 *            Defer就是传入一个可调用对象，当作用域结束Defer对象就会析构，析构函数里写这个传入的可调用对象，实现在结束作用域的时候回收一些东西
 *            RAII
 *            由于 Defer 类是基于 RAII 模式的，它会在 Defer 对象生命周期结束时自动执行资源清理操作。
 *            你把它放在了每个循环的开始部分，这样每次循环都会创建一个 Defer 对象，确保连接在循环结束时被回收。
 *            因此，Defer 在每次循环中都会执行一次清理操作，并且清理操作会在每次循环迭代结束时执行。
 *            如果你把 Defer 放在 for 循环外面，那么清理操作将只会在整个 pingConnection 函数执行完之后进行。
 *            所以，Defer 是在每次循环结束时执行清理操作的，这是符合你需求的。这样每次处理完一个连接之后，
 *            你就可以确保该连接被及时回收，避免内存泄漏或连接池中的连接被遗忘。
 * -----
 * Last Modified: 2024-12-17 22:52:31
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#include "base.h"
#include "circle_que_lk.h"
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <thread>

class SqlConnection {
public:
	SqlConnection(sql::Connection* conn, int64_t last_oper_time): _conn(conn), 
                                                                  _last_oper_time(last_oper_time){}
	std::unique_ptr<sql::Connection> _conn;
	int64_t _last_oper_time;
};

class MysqlConnectionPool {
public:
    MysqlConnectionPool(size_t pool_size, const std::string &host,
                        const std::string &port, const std::string &user,
                        const std::string &pass, const std::string &schema);
    ~MysqlConnectionPool();

    void pingConnection();

    std::unique_ptr<SqlConnection> getConnection();
    
    void recycleConnection(std::unique_ptr<SqlConnection> conn);

    void close();

private:
    size_t          _pool_size;
    std::string     _url;
    std::string     _user;
    std::string     _pass;
    std::string     _schema;

    CircleQueLk<std::unique_ptr<SqlConnection>, 10>   _conns_que;
    
    std::mutex                  _mtx;
    std::condition_variable     _cv;
    std::atomic<bool>           _b_stop;
    
    std::thread                 _ping_thread;
};