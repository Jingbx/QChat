/*
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/verify_server/proto.js
 * @date    : 2024-12-13 14:22:14
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : 解析.proto
 * -----
 * Last Modified: 2024-12-13 15:53:10
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */


const path = require('path')
const grpc = require('@grpc/grpc-js')
const protoLoader = require('@grpc/proto-loader')

const PROTO_PATH = path.join(__dirname, 'message.proto')
const packageDefinition = protoLoader.loadSync(PROTO_PATH, { keepCase: true, longs: String, enums: String, defaults: true, oneofs: true })
const protoDescriptor = grpc.loadPackageDefinition(packageDefinition)

const message_proto = protoDescriptor.message

module.exports = message_proto