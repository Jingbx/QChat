import QtQuick 2.9

Rectangle {
    id: directorMain
    visible: true
    width: 800+100
    height: 650+100
    color: "black"
    Component.onCompleted:  // Component组件
    {
        console.log("DirectorMain start...")
    }
}
