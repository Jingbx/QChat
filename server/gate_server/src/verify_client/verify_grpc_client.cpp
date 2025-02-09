/**
 * @file    :
 * /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/gate_server/verify_grpc_client.cpp
 * @date    : 2024-12-12 18:11:56
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-20 10:32:09
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	--------- ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

#include "verify_grpc_client.h"
#include "config/conf.h"

extern GeneralConf *gate_config;

VerifyCodeClient::VerifyCodeClient() {
    _conns_pool.reset(new RPCConnectionPool<VerifyService, VerifyService::Stub>(
        gate_config->grpc_conn_pool_size, gate_config->verify_server_host,
        std::to_string(gate_config->verify_server_port)));
}

VerifyCodeClient::~VerifyCodeClient() {}

GetVerifyRsp VerifyCodeClient::getVerifyCode(std::string email) {
    ClientContext ctx;
    GetVerifyReq req;
    GetVerifyRsp resp;
    // fixed: 客户端设置请求的peer邮件内容
    req.set_email(email);
    auto stub = _conns_pool->getConnection();
    Status status = stub->GetVerifyCode(&ctx, req, &resp);

    Defer defer([this, &stub]() {
        this->_conns_pool->recycleConnection(std::move(stub));
    });

    if (!status.ok()) {
        resp.set_error(ErrorCodes::ERROR_RPC);
        return resp;
    }
    return resp;
}
