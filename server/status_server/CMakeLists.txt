cmake_minimum_required(VERSION 3.10)
project(status_server)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)  # 使用 GNU 扩展标准

# 构建类型设置（可以通过 -DCMAKE_BUILD_TYPE=RelWithDebInfo 传入）
# 默认情况下支持 Debug、Release、RelWithDebInfo 等
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 包含目录
include_directories(
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/config"
    "${CMAKE_SOURCE_DIR}/src/grpc_server"
    "${CMAKE_SOURCE_DIR}/src/mysql_server"
    "${CMAKE_SOURCE_DIR}/src/redis_server"
)

# 第三方库路径
set(BOOST_ROOT /opt/homebrew/Cellar/boost@1.85/1.85.0_2)
set(JSONCPP_ROOT /opt/homebrew/Cellar/jsoncpp/1.9.6)
set(GRPC_ROOT /opt/homebrew/Cellar/grpc/1.68.2_1)
set(ABSEIL_ROOT /opt/homebrew/Cellar/abseil/20240722.0)
set(PROTOBUF_ROOT /opt/homebrew/Cellar/protobuf/29.1)
set(YAMLCPP_ROOT /opt/homebrew/Cellar/yaml-cpp/0.8.0)
set(HIREDIS_ROOT /opt/homebrew/Cellar/hiredis/1.2.0)
set(MYSQL_CONNECTOR_CPP_ROOT /usr/local/mysql-connector-c++)
set(SERVER_BASE /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/server_base)

# 添加 server_base 目录，并指定二进制目录
# add_subdirectory(${CMAKE_SOURCE_DIR}/../server_base ${CMAKE_BINARY_DIR}/server_base_build)

# 添加第三方库的 include 和 link 目录
include_directories(
    ${BOOST_ROOT}/include
    ${JSONCPP_ROOT}/include
    ${GRPC_ROOT}/include
    ${ABSEIL_ROOT}/include
    ${PROTOBUF_ROOT}/include
    ${YAMLCPP_ROOT}/include
    ${HIREDIS_ROOT}/include
    ${MYSQL_CONNECTOR_CPP_ROOT}/include
    ${SERVER_BASE}/src/include  # 包含 server_base 的头文件
)

link_directories(
    ${BOOST_ROOT}/lib
    ${JSONCPP_ROOT}/lib
    ${GRPC_ROOT}/lib
    ${ABSEIL_ROOT}/lib
    ${PROTOBUF_ROOT}/lib
    ${YAMLCPP_ROOT}/lib
    ${HIREDIS_ROOT}/lib
    ${MYSQL_CONNECTOR_CPP_ROOT}/lib64
    ${SERVER_BASE}/build
)

# 搜索源文件
file(GLOB all_src
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/config/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/grpc_server/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/mysql_server/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/redis_server/*.cpp"
)

# 创建可执行文件
add_executable(status_server main.cpp ${all_src})

target_link_libraries(status_server PRIVATE
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
    jsoncpp        # JsonCpp
    yaml-cpp
    absl_bad_any_cast_impl
    absl_bad_optional_access
    absl_bad_variant_access
    absl_city
    absl_civil_time
    absl_cord
    absl_cord_internal
    absl_cordz_functions
    absl_cordz_handle
    absl_cordz_info
    absl_cordz_sample_token
    absl_crc32c
    absl_crc_cord_state
    absl_crc_cpu_detect
    absl_crc_internal
    absl_debugging_internal
    absl_decode_rust_punycode
    absl_demangle_internal
    absl_demangle_rust
    absl_die_if_null
    absl_examine_stack
    absl_exponential_biased
    absl_failure_signal_handler
    absl_flags_commandlineflag
    absl_flags_commandlineflag_internal
    absl_flags_config
    absl_flags_internal
    absl_flags_marshalling
    absl_flags_parse
    absl_flags_private_handle_accessor
    absl_flags_program_name
    absl_flags_reflection
    absl_flags_usage
    absl_flags_usage_internal
    absl_graphcycles_internal
    absl_hash
    absl_hashtablez_sampler
    absl_int128
    absl_kernel_timeout_internal
    absl_leak_check
    absl_log_entry
    absl_log_flags
    absl_log_globals
    absl_log_initialize
    absl_log_internal_check_op
    absl_log_internal_conditions
    absl_log_internal_fnmatch
    absl_log_internal_format
    absl_log_internal_globals
    absl_log_internal_log_sink_set
    absl_log_internal_message
    absl_log_internal_nullguard
    absl_log_internal_proto
    absl_log_severity
    absl_log_sink
    absl_low_level_hash
    absl_malloc_internal
    absl_periodic_sampler
    absl_poison
    absl_random_distributions
    absl_random_internal_distribution_test_util
    absl_random_internal_platform
    absl_random_internal_pool_urbg
    absl_random_internal_randen
    absl_random_internal_randen_hwaes
    absl_random_internal_randen_hwaes_impl
    absl_random_internal_randen_slow
    absl_random_internal_seed_material
    absl_random_seed_gen_exception
    absl_random_seed_sequences
    absl_raw_hash_set
    absl_raw_logging_internal
    absl_scoped_mock_log
    absl_scoped_set_env
    absl_spinlock_wait
    absl_stacktrace
    absl_status
    absl_status_matchers
    absl_statusor
    absl_str_format_internal
    absl_strerror
    absl_string_view
    absl_strings
    absl_strings_internal
    absl_symbolize
    absl_synchronization
    absl_throw_delegate
    absl_time
    absl_time_zone
    absl_utf8_for_code_point
    absl_vlog_config_internal
    gpr
    server_base  # 确保链接 server_base
)