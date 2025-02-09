cmake_minimum_required(VERSION 3.10)
project(ServerBase)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)  # 使用 GNU 扩展标准

# 包含第三方库的路径
set(BOOST_ROOT /opt/homebrew/Cellar/boost@1.85/1.85.0_2)
set(GRPC_ROOT /opt/homebrew/Cellar/grpc/1.68.2_1)
set(ABSEIL_ROOT /opt/homebrew/Cellar/abseil/20240722.0)
set(PROTOBUF_ROOT /opt/homebrew/Cellar/protobuf/29.1)
set(HIREDIS_ROOT /opt/homebrew/Cellar/hiredis/1.2.0)
set(MYSQL_CONNECTOR_CPP_ROOT /usr/local/mysql-connector-c++)

# 设置包含目录，供其他项目使用
include_directories(
    # server_base 源代码目录
    ${CMAKE_SOURCE_DIR}/src/include             
    # third party
    ${BOOST_ROOT}/include
    ${GRPC_ROOT}/include
    ${ABSEIL_ROOT}/include
    ${PROTOBUF_ROOT}/include
    ${HIREDIS_ROOT}/include
    ${MYSQL_CONNECTOR_CPP_ROOT}/include
)

# 设置链接目录
link_directories(
    ${BOOST_ROOT}/lib
    ${GRPC_ROOT}/lib
    ${ABSEIL_ROOT}/lib
    ${PROTOBUF_ROOT}/lib
    ${HIREDIS_ROOT}/lib
    ${MYSQL_CONNECTOR_CPP_ROOT}/lib64
)

# 搜索源文件
file(GLOB ALL_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"       # 服务器代码
    "${CMAKE_SOURCE_DIR}/src/*.tpp"       # 如果有模板类需要处理
    "${CMAKE_SOURCE_DIR}/src/*.cc"  # gRPC 相关文件
)

# 创建静态库
add_library(server_base STATIC ${ALL_SOURCES})

# 链接依赖库
target_link_libraries(server_base PRIVATE
    boost_system       # Boost 系统库
    pthread            # 线程库
    grpc++             # gRPC++
    grpc               # gRPC C 库
    protobuf           # Protobuf 库
    absl_base          # Abseil 基础库
    hiredis            # Hiredis 库（如果用到 Redis）
    mysqlcppconn       # MySQL Connector C++（如果用到 MySQL）
    crypto             # 加密库（如果用到）
    ssl                # SSL 库（如果用到）
)