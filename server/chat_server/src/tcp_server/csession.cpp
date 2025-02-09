/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/tcp_server/csession.cpp
 * @date    : 2024-12-21 16:46:27
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-31 16:19:56
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#include "csession.h"
#include "cserver.h"
#include "logic_system.h"

CSession::CSession(boost::asio::io_context &ioc, CServer *server)
    : _socket(ioc), _server(server) {
    // fixed: sessionid初始化
    _session_id = _generateUuid();
    // fixed: 头要先初始化node
    _recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);    
}



void CSession::start() {
    asyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::close() {
    _socket.close();
    _b_stop = true;
}

/**
 * @brief: const boost::beast::error_code &ec, size_t transfered_bytes都是async_read_some()的返回
 *         这里是后续处理, total_len就是HEAD_TOTAL_LEN
 */
void CSession::asyncReadHead(size_t total_len) {
    auto self = shared_from_this();
    _asyncReadFull(total_len, [self, total_len] (const boost::beast::error_code &ec, size_t transfered_bytes) {
        try {
            if (ec) {
                std::cout << "Read Full error, " << ec.what() << std::endl;
                // close and drop
                self->close();
                self->_server->dropSession(self->_session_id);
                return;
            }
            // fixed:没必要判断 async_read_some中自定义的lambda处理没有小于这种返回
            if (transfered_bytes < total_len) {
                std::cout << "read length not match, read [" << transfered_bytes << "] , head total len ["
                    << total_len << "]" << std::endl;
                // close and drop
                self->close();
                self->_server->dropSession(self->_session_id);
                return;
            }
            // memcpy
            self->_recv_head_node->clear();
            memcpy(self->_recv_head_node->_data, self->_data, transfered_bytes);    // 和 HEAD_TOTOAL_LEN total_len一回事
            // id
            short msg_id = 0;
            memcpy(&msg_id, self->_recv_head_node->_data, HEAD_ID_LEN);
            msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
            std::cout << "Recvive msg: id=" << msg_id << std::endl;
            if (msg_id > MAX_LENGTH) {
                std::cout << "invalid msg_id is " << msg_id << std::endl;
                self->close();  // check:
                self->_server->dropSession(self->_session_id);
                return;
            }
            // len
            short msg_len = 0;
            memcpy(&msg_len, self->_recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
            msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
            std::cout << "Recvive msg: len=" << msg_len << std::endl;
            if (msg_len > MAX_LENGTH) {
                std::cout << "invalid msg_len is " << msg_len << std::endl;
                self->close();  // check:
                self->_server->dropSession(self->_session_id);
                return;
            }
            // recv_body_node
            self->_recv_body_node = std::make_shared<RecvNode>(msg_id, msg_len);
            self->asyncReadBody(msg_len);
        } catch (const std::exception &e) {
            std::cout << "Receive msg head error: " << e.what() << std::endl;
            // check:
            self->close();
            self->_server->dropSession(self->_session_id);
        }
    });
}

void CSession::asyncReadBody(size_t read_len) {
    auto self = shared_from_this();
    _asyncReadFull(read_len, [self, read_len] (const boost::beast::error_code &ec, size_t transfered_bytes) {
        try {
            if (ec) {
                std::cout << "Read Full error, " << ec.what() << std::endl;
                self->close();
                self->_server->dropSession(self->_session_id);
                return;
            }
            // fixed:没必要判断 async_read_some中自定义的lambda处理没有小于这种返回
            if (transfered_bytes < read_len) {
                std::cout << "read length not match, read [" << transfered_bytes << "] , total ["
                    << HEAD_TOTAL_LEN << "]" << std::endl;
                self->close();
                self->_server->dropSession(self->_session_id);
                return;
            }    
            // memcpy
            self->_recv_body_node->clear();
            memcpy(self->_recv_body_node->_data, self->_data, transfered_bytes);
            self->_recv_body_node->_curr_len += transfered_bytes;                       // fixed:
            self->_recv_body_node->_data[self->_recv_body_node->_total_len] = '\0';     // fixed:
            // logic处理
            LogicSystem::GetInstance()->pushToQue(std::make_shared<LogicNode>(self, self->_recv_body_node));
            // fixed: 继续读头
            self->asyncReadHead(HEAD_TOTAL_LEN);       

        } catch (const std::exception &e) {
            std::cout << "Receive msg head error: " << e.what() << std::endl;
            self->close();
            self->_server->dropSession(self->_session_id);
        }
    });
}

void CSession::send(short msg_id, std::string msg) {
    std::cout << "Chat server send msg: id=" << msg_id << ", len=" << msg.length() << std::endl;
    auto self = shared_from_this();
    // 构造并放入队列
    int que_size = getSendQueSize();
    if (que_size >= MAX_SENDQUE) {
        std::cout << "Send queue is full, retry!" << std::endl;
        return;
    }
    _send_msg_que.push(std::make_shared<SendNode>(msg_id, msg.length(), msg.c_str()));
    
    // 取出并发送 important: 确保发送有序性
    if (que_size > 0) {
        return;
    }
    auto send_node = _send_msg_que.pop_return();
    boost::asio::async_write(_socket, boost::asio::buffer(send_node->_data, send_node->_total_len), 
                             std::bind(&CSession::_writeHandler, this, std::placeholders::_1, self));
}

void CSession::send(short msg_id, short msg_len, char *msg_data) {
    
}

/**
 * private:
 */
std::string CSession::_generateUuid() {
    auto uuid = boost::uuids::random_generator()();
    std::string uuid_str = boost::uuids::to_string(uuid);
    return uuid_str;
}

void CSession::_asyncReadFull(
    size_t total_len,
    std::function<void(const boost::beast::error_code &, size_t)> handler) {
    _asyncReadSome(0, total_len, handler);
}

void CSession::_asyncReadSome(
    size_t begin_len, size_t total_len,
    std::function<void(const boost::beast::error_code &, size_t)> handler) {
    auto self = shared_from_this();
    _socket.async_read_some(
        boost::asio::buffer(_data + begin_len, total_len - begin_len),  // 第二个参数是长度
        [self, begin_len, total_len, handler] (const boost::beast::error_code &ec, size_t bytes_transfered) {
            if (ec) {
                handler(ec, begin_len + bytes_transfered);
                return;
            }
            // note: 只允许读够长度才返回
            if (begin_len + bytes_transfered >= total_len) {
                handler(ec, begin_len + bytes_transfered);
                return;
            }
            self->_asyncReadSome(begin_len + bytes_transfered, total_len, handler);
    });
}

void CSession::_writeHandler(const boost::beast::error_code &ec,
                             std::shared_ptr<CSession> session_self) {
    try {
        if (ec) {
            std::cout << "Failed to async_write, err: " << ec.what() << std::endl;
            close();
            _server->dropSession(_session_id);
        } else {
            
            if (!_send_msg_que.empty()) {
                // 继续绑定发送
                auto self = shared_from_this();
                auto send_msg = _send_msg_que.pop_return();
                boost::asio::async_write(_socket, boost::asio::buffer(send_msg->_data, send_msg->_total_len),
                                         std::bind(&CSession::_writeHandler, this, std::placeholders::_1, session_self));
            }
        }
    } catch (const std::exception &e) {
		std::cerr << " Exception code :writeHandler " << e.what() << std::endl;        
    }              
}
