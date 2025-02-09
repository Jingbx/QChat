/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/src_base/grpc_connection_pool.h
 * @date    : 2024-12-19 19:16:29
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : grpc stub链接池 模版类
 * -----
 * Last Modified: 2024-12-31 15:40:40
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/
#ifndef __SERVER_GRPC_CONNECTION_POOL_H_
#define __SERVER_GRPC_CONNECTION_POOL_H_

#include "message.grpc.pb.h"
#include "base.h"
#include "circle_que_lk.h"
#include <atomic>
#include <condition_variable>
#include <grpcpp/grpcpp.h>
#include <mutex>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;
/**
 * @brief stub pool 每次向grpc服务器请求验证码服务需要在此取存根
 *        stub个数是池子个数，用完之后需要回收重复利用
 *        Enhanced: 使用list实现的队列队头队尾加锁实现高性能
 */
template <typename GrpcServer, typename GrpcServerStub>
class RPCConnectionPool {
  public:
    RPCConnectionPool(size_t pool_size, std::string host, std::string port);
    ~RPCConnectionPool();

    std::unique_ptr<GrpcServerStub> getConnection();
    void recycleConnection(std::unique_ptr<GrpcServerStub> conn);
    void stop();

  private:
    size_t _pool_size;
    std::string _host;
    std::string _port;
    CircleQueLk<std::unique_ptr<GrpcServerStub>, 10> _conns_que;    // important: 面试 unique_ptr管理

    std::atomic<bool> _b_stop;
    std::condition_variable _cv;
    std::mutex _mtx;
};

#include "../grpc_connection_pool.tpp"  // 包含定义文件

#endif  // _SERVER_RPC_CONNECTION_POOL_H