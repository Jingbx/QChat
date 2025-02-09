/*
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/verify_server/email.js
 * @date    : 2024-12-13 15:35:12
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : 发送邮件接口
 * -----
 * Last Modified: 2024-12-13 16:43:47
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */

const nodemailer = require('nodemailer');
const config_module = require('./config')

/**
 * @brief 封装发送代理
 */
let transport = nodemailer.createTransport({
   host: 'smtp.163.com',
   port: 465,
   secure: true,
   auth: {
        user: config_module.email_user, // 发送者邮箱以及授权码
        pass: config_module.email_pass
   }
});

/**
 * @brief 发送接口
 */
function sendMail(mailOptions_) {
    // 类似future，希望从异步事件变成同步事件，在main中await确保收到grpc验证码回复
    return new Promise(function(resolve, reject) {
        // transport.sendMail默认异步
        transport.sendMail(mailOptions_, function(error, info) {
            if (error) {
                console.log(error);
                reject(error);
            } else {
                console.log("Email has been sent success!");
                resolve(info.response);
            }
        });
    });
}

module.exports.sendMail = sendMail