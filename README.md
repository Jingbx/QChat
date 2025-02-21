# QChat💬

## 功能
- 注册&登陆&修改密码
- 1v1好友聊天
- 气泡框视频播放(包含快进快退暂停播放等功能)

## 技术实现
- 前端：Qt
  - 视频播放：FFmpeg SDL QML 

- 后端：
  - GateServer: 实现Http服务，处理验证、登陆、聊天请求的分发处理
  - VerifyServer: node.js邮箱验证服务
  - StatusServer: 状态服务，查询分布式聊天服务器状态，分发对应聊天服务endpoint以及session token
  - ChatServer: 分布式聊天，Tcp长连接处理聊天请求，处理好友添加、认证请求。获取好友列表，好友认证信息供前端加载。

## 编译
- 客户端配置pro文件，服务端配置所有服务中CMakeLists.txt中依赖库路径
- 服务端编译
  ```shell
  cd Qchat/server
  ./build_all.sh 
  ```
  我将Mysql、Redis、Grpc模版连接池、protobuf文件统一打包至server_base，编译成静态库统一链接到其他服务，减少代码冗余。

## 运行
  - conf目录下完善服务配置，项目通过YAML-CPP实现配置管理
  - 运行所有服务
  - Qt启动客户端

