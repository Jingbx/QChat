#!/bin/bash

# SERVER_BASE_BUILD_DIR="./server_base"
# cd $SERVER_BASE_BUILD_DIR || exit 1
# rm -rf build
# cd ..


# Building Gate Server
GATE_SERVER_BUILD_DIR="./gate_server"
cd $GATE_SERVER_BUILD_DIR || { echo "Failed to enter Gate Server directory"; exit 1; }
rm -rf build
echo "Gate Server build directory cleaned"
cd ..

# Building Status Server
STATUS_SERVER_BUILD_DIR="./status_server"
cd $STATUS_SERVER_BUILD_DIR || { echo "Failed to enter Status Server directory"; exit 1; }
rm -rf build
echo "Status Server build directory cleaned"
cd ..

# Building Chat Server 1
STATUS_SERVER_BUILD_DIR="./chat_server"
cd $STATUS_SERVER_BUILD_DIR || { echo "Failed to enter Chat Server 1 directory"; exit 1; }
rm -rf build
echo "Chat Server build directory cleaned"
cd ..

# Building Chat Server 2
STATUS_SERVER_BUILD_DIR="./chat_server2"
cd $STATUS_SERVER_BUILD_DIR || { echo "Failed to enter Chat Server 2 directory"; exit 1; }
rm -rf build
echo "Chat Server 2 build directory cleaned"
cd ..

echo "🎉🎉🎉 All build directories cleaned successfully 🎉🎉🎉"