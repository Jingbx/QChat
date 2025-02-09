/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/status_grpc_client.h
 * @date    : 2024-12-19 16:54:45
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-20 16:02:38
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#ifndef __STATUS_GRPC_CLIENT_H_
#define __STATUS_GRPC_CLIENT_H_

#include "../../server_base/src/include/base.h"
#include "../../server_base/src/include/grpc_connection_pool.h"
#include "../../server_base/src/include/message.grpc.pb.h"
#include <atomic>
#include <condition_variable>
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

/**
 * @brief gateserver 作为grcp验证码请求客户端
 */
class StatusGrpcClient : public SingleTon<StatusGrpcClient> {
    friend class SingleTon<StatusGrpcClient>;

  public:
    ~StatusGrpcClient();
    GetChatServerRsp getChatServer(int uid);

  private:
    StatusGrpcClient();

  private:
    // 客户端存根(信使)池
    std::unique_ptr<RPCConnectionPool<StatusService, StatusService::Stub>>
        _conns_pool;
};

#endif // __VERIFY_GRPC_CLIENT_H_
