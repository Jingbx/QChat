#!/bin/bash

# 设置 CMake 构建目录
BUILD_DIR=build

# 如果构建目录已经存在，清空它
if [ -d "$BUILD_DIR" ]; then
  rm -rf $BUILD_DIR
fi

# 创建构建目录
mkdir $BUILD_DIR
cd $BUILD_DIR

# 运行 CMake 配置
cmake ..

# 编译项目
make

# 如果你想安装库到系统，可以添加以下命令（可选）
# sudo make install

# 打印成功消息
echo "ServerBase has been built successfully!"