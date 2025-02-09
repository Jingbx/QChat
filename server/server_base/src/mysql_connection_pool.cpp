/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/src_base/mysql_connection_pool.cpp
 * @date    : 2024-12-17 15:41:01
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-20 15:53:25
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/
#include "include/mysql_connection_pool.h"


MysqlConnectionPool::MysqlConnectionPool(size_t pool_size, const std::string &host,
                                         const std::string &port, const std::string &user,
                                         const std::string &pass, const std::string &schema) 
                                       : _pool_size(pool_size),
                                         _url(host + ":" + port),
                                         _user(user), _pass(pass), _schema(schema),
                                         _b_stop(false) {
    try {
        for (size_t i = 0; i < _pool_size; ++i) {
            /*conn*/
            // 设备
            sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
            // 连接+配置数据库
            sql::Connection *conn(driver->connect(_url, _user, _pass));     // fixed: conn是普通指针，但是在_conn构造时成为智能指针
            conn->setSchema(_schema);
            /*last operate time*/
			auto currentTime = std::chrono::system_clock::now().time_since_epoch();
			long long timeStamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
            // 入池
            _conns_que.push(std::make_unique<SqlConnection>(conn, timeStamp));      
        }
        // Enhanced:
        _ping_thread = std::thread([this] () {
            while (!this->_b_stop) {
                pingConnection();
                std::this_thread::sleep_for(std::chrono::seconds(60));
            }
        });
        _ping_thread.detach();
        
    } catch (const sql::SQLException &e) {
        std::cout << "MysqlConnectionPool init error: " << e.what() << std::endl;
    }
}

MysqlConnectionPool::~MysqlConnectionPool() {
    close();
    while (!_conns_que.empty()) {
        _conns_que.pop();
    }
}

// 发送SELET命令 ping
void MysqlConnectionPool::pingConnection() {
    size_t curr_size = _conns_que.size();               // note: 线程安全

    std::lock_guard<std::mutex> lock(_mtx);             // fixed: 加锁
    
    for (size_t i = 0; i < curr_size; ++i) {            // fixed: 要获取当前池子大小 可能有些被取走了
        auto conn = _conns_que.pop_return();
        Defer defer ([this, &conn] () {
            this->recycleConnection(std::move(conn));   // fixed: unique_ptr不能拷贝
        });
        // ping
        auto currentTime = std::chrono::system_clock::now().time_since_epoch();
        long long timeStamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
        if (timeStamp - conn->_last_oper_time < 600) {
            continue;
        }
        try {
			std::unique_ptr<sql::Statement> stmt(conn->_conn->createStatement());
			stmt->executeQuery("SELECT 1");
			conn->_last_oper_time = timeStamp;
			// std::cout << "execute timer alive query , cur is " << timeStamp << std::endl;   // 🎉测试通过	            
        } catch (const sql::SQLException &e) {
            // note: executeQuery失败链接有问题，所以建立新链接替换结构体里的旧链接
            sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
            sql::Connection *new_conn(driver->connect(_url, _user, _pass));
            new_conn->setSchema(_schema);
            conn->_conn.reset(new_conn);
            conn->_last_oper_time = timeStamp;
        }
    }
}

std::unique_ptr<SqlConnection> MysqlConnectionPool::getConnection() {
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _cv.wait(lock, [this] () {
            if (this->_b_stop) {
                return true;
            } 
            return !this->_conns_que.empty();
        });
    }
    if (_b_stop) {
        return nullptr;
    }
    return _conns_que.pop_return();
}

void MysqlConnectionPool::recycleConnection(std::unique_ptr<SqlConnection> conn) {
    if (_b_stop) {
        return;
    }
    _conns_que.push(std::move(conn));
    _cv.notify_one();
}

void MysqlConnectionPool::close() {
    _b_stop = true;
    _cv.notify_all();
}
