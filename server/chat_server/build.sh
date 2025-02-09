#!/bin/bash

# 设置构建目录
BUILD_DIR="build"

# 如果构建目录不存在，则创建
if [ ! -d "$BUILD_DIR" ]; then
    mkdir $BUILD_DIR
fi

cd $BUILD_DIR

# 检查是否传入 "clean" 参数
if [ $# -gt 0 ] && [ "$1" = "clean" ]; then
    echo "Cleaning build directory..."
    make clean
    exit 0
fi

# 根据参数设置 CMake 配置
if [ $# -gt 0 ] && [ "$1" = "core" ]; then
    echo "Building with core dump support..."
    cmake -DCMAKE_CXX_FLAGS="-g" ../  # 添加调试标志
else
    echo "Building without core dump support..."
    cmake ../
fi

# 执行 make
echo "Building project with make -j8..."
make -j8
# make VERBOSE=1