/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/verify_grpc_client.h
 * @date    : 2024-12-12 18:10:53
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * GateServer收到qt客户端请求后，向grpc服务器发送验证码请求，此时GateServer是客户端角色
 * -----
 * Last Modified: 2024-12-20 16:02:28
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#ifndef __VERIFY_GRPC_CLIENT_H_
#define __VERIFY_GRPC_CLIENT_H_

#include "../../server_base/src/include/base.h"
#include "../../server_base/src/include/grpc_connection_pool.h"
#include "../../server_base/src/include/message.grpc.pb.h"
#include <atomic>
#include <condition_variable>
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

/**
 * @brief gateserver 作为grcp验证码请求客户端
 */
class VerifyCodeClient : public SingleTon<VerifyCodeClient> {
    friend class SingleTon<VerifyCodeClient>;

  public:
    ~VerifyCodeClient();
    GetVerifyRsp getVerifyCode(std::string email);

  private:
    VerifyCodeClient();

  private:
    // 客户端存根(信使)池
    std::unique_ptr<RPCConnectionPool<VerifyService, VerifyService::Stub>>
        _conns_pool;
    // std::unique_ptr<VerifyService::Stub> _stub;
};

#endif // __VERIFY_GRPC_CLIENT_H_
