/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/status_grpc_client.cpp
 * @date    : 2024-12-19 16:54:48
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-20 17:22:28
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#include "status_grpc_client.h"
#include "config/conf.h"

extern GeneralConf *gate_config;

StatusGrpcClient::StatusGrpcClient() {
    _conns_pool.reset(new RPCConnectionPool<StatusService, StatusService::Stub>(
        gate_config->grpc_conn_pool_size, gate_config->status_server_host,
        std::to_string(gate_config->status_server_port)));
}

StatusGrpcClient::~StatusGrpcClient() {}

GetChatServerRsp StatusGrpcClient::getChatServer(int uid) {
    ClientContext ctx;
    GetChatServerReq req;
    GetChatServerRsp resp;

    req.set_uid(uid);
    auto stub = _conns_pool->getConnection();
    Status status = stub->GetChatServer(&ctx, req, &resp);

    Defer defer([this, &stub]() {
        this->_conns_pool->recycleConnection(std::move(stub));
    });

    if (!status.ok()) {
        resp.set_error(ErrorCodes::ERROR_RPC);
        return resp;
    }
    return resp;
};
