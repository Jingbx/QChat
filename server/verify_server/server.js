/*
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/verify_server/server.js
 * @date    : 2024-12-13 15:49:20
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : Main
 * -----
 * Last Modified: 2024-12-16 11:19:04
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
const grpc = require('@grpc/grpc-js')
const { v4: uuidv4 } = require('uuid')
const base_moudle = require('./base')
const proto_module = require('./proto')
const email_module = require('./email')
const redis_module = require('./redis')
const config_module = require('./config')


async function GetVerifyCode(call, callback) {
    console.log("peer email is ", call.request.email);
    try {
        /** 查询(当前邮箱)是否满足15s间隔期: 1级<veriCodeWait, 1>键，设置过期时间10s，
            如果能查询到说明需要等待，如果查询不到继续发送
            发送验证码后，立即添加该键
        */
        let wait = await redis_module.GetRedis(base_moudle.code_need_wait + call.request.email);
        if (wait !== null) {
            // 等待
            console.log("Request denied: Code requested within 15 second.");    // TODO: 错误返回frontend
            callback(null, {
                error: base_moudle.Errors.ERROR_TOOMANYREQ
            });
            return;
        }
        /**
         * 发送验证码，设置验证码超时时间3min
         * 设置veriCodeWait=1
         * 重新发送则覆盖旧验证码
         */
        veriCode = uuidv4().substring(0, 4);
        console.log("veriCode is ", veriCode);
        let set_kveprie_ret = await redis_module.SetRedisWithExpireTime(base_moudle.code_prefix + call.request.email,
            veriCode,
            180);   // config_module.vcode_expired
        let set_wait_ret = await redis_module.SetRedisWithExpireTime(base_moudle.code_need_wait + call.request.email,
            1,    // config_module.vcode_wait_label
            15);         // config_module.vcode_wait
        if (!set_kveprie_ret || !set_wait_ret) {
            callback(null, {
                email: call.request.email,
                error: base_moudle.Errors.ERROR_REDIS
            });
            return;
        }
        let email_text = '🎉欢迎注册QChat, 您的验证码是「' + veriCode + '」' + ', 有效期为3分钟🎉'
        // email option
        let emailOptions = {
            from: '18804328557@163.com',
            to: call.request.email,
            subject: 'QChat💬', // email主题
            text: email_text
        }
        // send email sync
        let resp = await email_module.sendMail(emailOptions);

        // callback
        console.log("Send email success, resp is ", resp);
        callback(null, {
            email: call.request.email,
            error: base_moudle.Errors.SUCCESS
        });

    } catch (error) {   // resp出现异常这里会自动捕获
        console.log("grpc server catch error is ", error);
        // callback参数见.proto
        callback(null, {
            email: call.request.email,
            error: base_moudle.Errors.ERROR_EXCEPTION
        });
    }
}

async function main() {
    try {
        console.log('Starting server and clearing Redis...');
        await redis_module.ClearRedis('need_wait_*'); // 清理测试相关 Redis 数据
    } catch (error) {
        console.error('Failed to clear Redis keys:', error);
    }
    var server = new grpc.Server()
    server.addService(proto_module.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        console.log('grpc server started')
    })
}

main()