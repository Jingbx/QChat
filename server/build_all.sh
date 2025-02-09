#!/bin/bash


SERVER_BASE_BUILD_DIR="./server_base"
echo "🙏🙏🙏🙏🙏🙏     Building server_base    🙏🙏🙏🙏🙏🙏"
cd $SERVER_BASE_BUILD_DIR || exit 1
./build.sh
echo "🙏🙏🙏🙏🙏🙏 Server base build completed 🙏🙏🙏🙏🙏🙏"
cd ..


GATE_SERVER_BUILD_DIR="./gate_server"
echo "🙏🙏🙏🙏🙏🙏     Building gate_server    🙏🙏🙏🙏🙏🙏"
cd $GATE_SERVER_BUILD_DIR || exit 1
./build.sh
echo "🙏🙏🙏🙏🙏🙏 Gate server build completed 🙏🙏🙏🙏🙏🙏"
cd ..


STATUS_SERVER_BUILD_DIR="./status_server"
echo "🙏🙏🙏🙏🙏🙏     Building status_server    🙏🙏🙏🙏🙏🙏"
cd $STATUS_SERVER_BUILD_DIR || exit 1
./build.sh
echo "🙏🙏🙏🙏🙏🙏 Status server build completed 🙏🙏🙏🙏🙏🙏"
cd ..

STATUS_SERVER_BUILD_DIR="./chat_server"
echo "🙏🙏🙏🙏🙏🙏     Building chat_server    🙏🙏🙏🙏🙏🙏"
cd $STATUS_SERVER_BUILD_DIR || exit 1
./build.sh
echo "🙏🙏🙏🙏🙏🙏 Status server build completed 🙏🙏🙏🙏🙏🙏"
cd ..

STATUS_SERVER_BUILD_DIR="./chat_server2"
echo "🙏🙏🙏🙏🙏🙏     Building chat_server2    🙏🙏🙏🙏🙏🙏"
cd $STATUS_SERVER_BUILD_DIR || exit 1
./build.sh
echo "🙏🙏🙏🙏🙏🙏 Status server build completed 🙏🙏🙏🙏🙏🙏"
cd ..

echo "🎉🎉🎉 All builds completed 🎉🎉🎉"