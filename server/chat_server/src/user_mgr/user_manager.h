/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/user_mgr/user_manager.h
 * @date    : 2024-12-23 14:04:51
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : 管理Uid和对应的Session的映射，
 * 			  方便分布式服务首先通过查找redis中uid与ip的对应获取peer连接的服务器，
 * 			  然后peer server通过对应uid得到session与client2通信
 * -----
 * Last Modified: 2024-12-31 18:26:32
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/

#ifndef __USER_MANAGER_H_
#define __USER_MANAGER_H_

#include <unordered_map>
#include <memory>
#include "../tcp_server/csession.h"
#include "../../server_base/src/include/singleton.h"

class UserManager : public SingleTon<UserManager> {
friend class SingleTon<UserManager>;
public:
	~UserManager();	

	void bindUidAndSession(const int &uid, std::shared_ptr<CSession> session);
	std::shared_ptr<CSession> getSessionFromUid(const int &uid);
	// @brief 删除用户/用户离线
	void rmvUserSession(const int &uid);
	
private:
	UserManager();
private:
	std::mutex	_mtx;
	std::unordered_map<int, std::shared_ptr<CSession>> _uid2map;
};


#endif 	// __USER_MANAGER_H_