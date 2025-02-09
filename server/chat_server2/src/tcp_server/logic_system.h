/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/chat_server/src/tcp_server/logic_system.h
 * @date    : 2024-12-21 16:46:09
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2025-01-09 20:46:10
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#ifndef __CHAT_LOGIC_SYSTEM_H_
#define __CHAT_LOGIC_SYSTEM_H_

#include "../../server_base/src/include/base.h"
#include "../../server_base/src/include/circle_que_lk.h"
#include "../../server_base/src/include/singleton.h"
#include "../user_mgr/user_data.h"
class LogicNode;
class CSession;
/* std::string &msg_data包括data和当前位置_curr_node拼接在尾部 */
typedef std::function<void(std::shared_ptr<CSession>, const short msg_id,
                           const std::string &msg_data)> FuncCallback;

class LogicSystem : public SingleTon<LogicSystem>,
                    public std::enable_shared_from_this<LogicSystem> {
                      
friend class SingleTon<LogicSystem>;

public:
    ~LogicSystem();

    // @brief 生产队列 important: recv_que队列从空变为非空，通知消费者
    void pushToQue(std::shared_ptr<LogicNode> msg);

private:
    LogicSystem();
    // @brief 初始化处理器表
    void _initFuncCallbakes();
    // @brief 处理器callbacks
    void _handleChatLogin(std::shared_ptr<CSession> session, const short msg_id,
                          const std::string &msg_data);
    void _handleSearchUser(std::shared_ptr<CSession> session, const short msg_id,
                          const std::string &msg_data); // note: 根据(search edit中的)uid/name查找
    void _handleAddFriendApply(std::shared_ptr<CSession> session, const short msg_id,
                               const std::string &msg_data); // note: 用户存在发送给对端添加好友请求等待认证  
    void _handleAuthFriendApply(std::shared_ptr<CSession> session, const short msg_id,
                                const std::string &msg_data); // note: 认证好友并给对端回复
    void _handleDealChatTextMsg(std::shared_ptr<CSession> session, 
                                const short &msg_id, const std::string &msg_data);    // note: msg_id是判断是否送达的
    //
    bool getFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list);
	bool getFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>> & user_list);
                            
    // @brief 从Redis和Mysql中查找数据，首先从redis中查找如果没有容sql中查找并存入redis缓存
    bool GetUserInfoFromDataBase(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
    bool GetUserInfoFromDataBase(std::string base_key, std::string name, std::shared_ptr<UserInfo>& userinfo);
    // @brief 消费队列 function表 std::bind
    void _consumeFromQue();

private:
    std::atomic<bool>                                       _b_stop;
    std::condition_variable                                 _produce_cv;
    std::condition_variable                                 _consume_cv;
    std::mutex                                              _produce_mtx;
    std::mutex                                              _consume_mtx;
    CircleQueLk<std::shared_ptr<LogicNode>, MAX_RECVQUE>    _msg_que;
    std::unordered_map<short, FuncCallback>                 _func_callbacks;
    std::thread                                             _consume_thread;
};

#endif //  __CHAT_LOGIC_SYSTEM_H_