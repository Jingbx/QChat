/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/tcp_server/msg_node.h
 * @date    : 2024-12-22 13:02:28
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : MsgNode基类char数组存储_data，RecvNode仅包含Body，SendNode包含完整数据
 * -----
 * Last Modified: 2024-12-23 12:56:48
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#ifndef __CHAT_MSG_NODE_H_
#define __CHAT_MSG_NODE_H_

#include "../../server_base/src/include/base.h"

class MsgNode
{
public:
	MsgNode(short max_len) :_total_len(max_len), _curr_len(0) {
		_data = new char[_total_len + 1]();
		_data[_total_len] = '\0';
	}

	virtual ~MsgNode() {
		std::cout << "destruct MsgNode" << std::endl;
		delete[] _data;
	}

	void clear() {
		::memset(_data, 0, _total_len);
		_curr_len = 0;
	}

	short _curr_len;
	short _total_len;
	char* _data;
};

/**
 * @brief RecvNode: 仅存储Body大小的数据
 */
class RecvNode : public MsgNode {
public:
    RecvNode(short msg_id, short msg_len) : MsgNode(msg_len), _msg_id(msg_id) {}
    ~RecvNode() override = default;

public:
    short _msg_id;
};

/**
 * @brief SendNode: 存储要发出的完整数据
 */
class SendNode : public MsgNode {
public:
    SendNode(short msg_id, short msg_len, const char* msg_data)
        : MsgNode(HEAD_TOTAL_LEN + msg_len), _msg_id(msg_id) {

        if (msg_data == nullptr || msg_len <= 0) {
            throw std::invalid_argument("Invalid msg_data or msg_len");
        }

        if (msg_len > (_total_len - HEAD_TOTAL_LEN)) {
            throw std::overflow_error("Message length exceeds available space");
        }

        short msg_id_bg = boost::asio::detail::socket_ops::host_to_network_short(msg_id);
        short msg_len_bg = boost::asio::detail::socket_ops::host_to_network_short(msg_len);
        
        memcpy(_data, &msg_id_bg, HEAD_ID_LEN);
        memcpy(_data + HEAD_ID_LEN, &msg_len_bg, HEAD_DATA_LEN);
        memcpy(_data + HEAD_TOTAL_LEN, msg_data, msg_len);
    }

    ~SendNode() override = default;

private:
    short _msg_id;
};

#endif  //  __CHAT_MSG_NODE_H_