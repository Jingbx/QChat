// import QtQuick 2.12
// import QtQuick.Window 2.12
// import QtQuick.Controls 2.5
// import com.jingbx.QPlayer.QPlayer 1.0
// import QtQuick.Layouts 1.3
// import QtQuick.Dialogs
// import QtQuick 2.0
// 导入APP中注册的类
/**
  * 1.自动播放正确文件路径 ✅
  * 2.播放结束后返回视频开头处于暂停状态 ✅
  * 3.seek进度条点击没有反应，应该on一下 ✅
  * 4.关了QQuickView后仍然在后台播放 ✅
  * 5.开始时候play_state设置错误 ✅
  */
import com.jingbx.QPlayer.QPlayer 1.0
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls.Basic
import QtQuick.Controls 2.2
import QtQuick.Controls.macOS
import "./mediaplay/"
import "./record/"
import "./director/"

Item {
    width: itemWidth ? itemWidth : 100  // 默认宽度为 100
    height: itemHeight ? itemHeight : 50  // 默认高度为 50

    property int itemWidth: 900
    property int itemHeight: 750
    property int fontSize: 14
    property int scaledMargin: 10

    Rectangle {
        id: mediaPlayMain
        visible: true
        width: 800+100
        height: 650+100
        color: "black"
       // color: "transparent"

        //全局变量
        //标题栏按钮高度
        property int toolBarCloseH: 30
        property int toolBarCloseW: 30

        property string titleStr: "_"

        // pixDens 是一个整数属性，代表屏幕的像素密度，取 Screen.pixelDensity 的向上取整值。Screen.pixelDensity 表示每英寸的像素数（PPI）
        property int pixDens: Math.ceil(Screen.pixelDensity)
        property int itemWidth: 25 * pixDens
        property int itemHeight: 10 * pixDens
        property int scaledMargin: 2 * pixDens
        property int fontSize: 3 * pixDens

        property int play_state: 0

        property string btnAudioIcon_source:""


        // init page
        // 界面启动时信号
        Component.onCompleted:  // Component组件
        {
            console.log("Goto MediaPlayMainPage.qml...")
            qVideoOutput.urlPass(videoPath);
        }

        // 界面销毁时信号
        Component.onDestruction: {
            console.log("Destroy MediaPlayMainPage.qml...")
            // 调用slots ： 关闭正在播放的视频
            qVideoOutput.stopPlay();
        }
    // --------------------------------------
        QPlayer{
            id:qPlayer
        }

        QVideoOutput
        {
            id: qVideoOutput
            objectName: "qVideoOutput"
            mWidth: parent.width  // rectangle的width height
            mHeight: parent.height
            mShowType: 0
            anchors.fill: parent  // 填充整个rectangle

            // 四周间距
            anchors.topMargin:  20
            anchors.bottomMargin: 60
            anchors.rightMargin: 20
            anchors.leftMargin:  20
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.centerIn: parent
            clip: false  // 超出范围裁剪

            // 布局
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: mediaPlayMain.width
            Layout.maximumHeight: mediaPlayMain.height

            // 设置初始背景 -> configure.json
            fileUrl: !qPlayer.getStrBackgroundPath() ? "" : qPlayer.getStrBackgroundPath()

            // singal 处理
            // 1. 进度条 todo : 随seek位置而变化
            onTotalDurationChanged:{
                // 进度条总长度
                console.log("onTotalDurationChanged...");
                playProgressBar.to = qVideoOutput.totalDuration; //s
            }
            onTotalDurationStrChanged:{
                console.log("qVideoOutput.totalDurationStr=",qVideoOutput.totalDurationStr)
                // console.log("Video width x height changed to: " + mediaPlayMain.width + "x"+ mediaPlayMain.height)
                totalDuration.text = qVideoOutput.totalDurationStr; //"00:03:46"
                playProgressBar.enabled = 1;
            }
            onCurrPlayTimeStrChanged:{
                // 进度条当前位置
                playProgressBar.value = qVideoOutput.currPlayTime; //s
            }
            onCurrPlayTimeChanged:{
                // 更新当前播放位置
                currDuration.text = qVideoOutput.currPlayTimeStr; //"00:00:01"
            }
            onUpdateCurrAVTitleChanged:{
                // 更新视频标题
                // if(qVideoOutput.currAVTitle.length !== 0)
                // {
                //     Qt.application.title = titleStr + qVideoOutput.currAVTitle + titleStr;
                //     // mediaPlayMain.title = titleStr + qVideoOutput.currAVTitle + titleStr;
                // }else{
                //     mediaPlayMain.title = titleStr;
                // }
            }
        }

        //中，内容区域
        Label {
            id: contentArea
            x: 8
            y: 46  // ?
            width: mediaPlayMain.width
            height: mediaPlayMain.height - 70 - 10
            horizontalAlignment: Text.AlignLeft
            anchors.rightMargin: 5
            anchors.leftMargin: 5
            anchors.bottomMargin: 5
        }


        //下,控制栏
        Rectangle {
            id: controlBar
            x: 0
            y: mediaPlayMain.height - 70
            width: mediaPlayMain.width
            height: 70
            color: "transparent"
            anchors.top: qVideoOutput.bottom

            Row {
                id: durationRow
                spacing: 5
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    id: currDuration
                    text: qsTr("00:00:00")
                    font.pixelSize: 12
                    color: "#f9f9f9"
                }

                Text {
                    text: qsTr("/")
                    font.pixelSize: 12
                    color: "#f2f1f1"
                }

                Text {
                    id: totalDuration
                    text: qsTr("00:00:00")
                    font.pixelSize: 12
                    color: "#FF9999"
                }
            }

            Slider {
                id: playProgressBar
                width: controlBar.width - 20
                height: 10
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 5
                // ⚠️:
                // visible: qVideoOutput.getPlayState() === 1  // 播放时才能看到进度条
                // enabled: qVideoOutput.getPlayState() === 1 // 当 play_state 为 1 时启用滑动
                onValueChanged: {
                    console.log("qVideoOutput.getPlayState()-->", qVideoOutput.getPlayState())
                    if (qVideoOutput.currPlayTime !== playProgressBar.value) {
                        // 调用slots，实现通过ui进行seek,到seek_time位置
                        console.log("video ProgressBar val-->" + playProgressBar.value)
                        qVideoOutput.playSeek(playProgressBar.value)
                    }
                    // fixed: 当进度条播放完毕
                    if (playProgressBar.value === qVideoOutput.totalDuration) {
                        console.log("播放完毕: " + playProgressBar.value)
                        qVideoOutput.stopPlay();
                        reset_player_ui();
                    }
                }
            }

            Row {
                id: buttonRow
                spacing: 20
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: playProgressBar.bottom
                anchors.topMargin: -5

                Button {
                    id: btnStopPlay
                    width: 48
                    height: 48
                    text: qsTr("退出")
                    onClicked: {
                        qVideoOutput.stopPlay();
                        reset_player_ui();
                    }
                }

                Button {
                    id: btnStartPlay
                    width: 48
                    height: 48
                    text: qVideoOutput.playState === 2 ? qsTr("开始") : qsTr("暂停")
                    onClicked: {
                        console.log("Playing...");

                        qVideoOutput.startPlay(); // Error: Invalid write to global property "play_state"
                        console.log("play_state: ", qVideoOutput.playState);
                    }
                }

                Button {
                    id: btnFastReversePlay
                    width: 48
                    height: 48
                    text: qsTr("快退")
                    onClicked: {
                        qVideoOutput.playDown();
                    }
                }

                Button {
                    id: btnFastForwardPlay
                    width: 48
                    height: 48
                    text: qsTr("快进")
                    onClicked: {
                        qVideoOutput.playUp();
                    }
                }

                Button {
                    id: btnAudioIcon
                    width: 48
                    height: 48
                    text: qsTr("音量")
                    onClicked: {
                        console.log("调整音量");
                    }
                }

                Slider {
                    id: volumeCtrl
                    width: 100
                    height: 10
                    anchors.verticalCenter: parent.verticalCenter
                    from: 0
                    to: 128
                    onMoved: {
                        var volume = this.value;
                        qVideoOutput.updateVolume(volume);
                        btnAudioIcon.contentItem.source = getVolumeIcon(volume)
                    }
                }

            }
        }
    }
    //    打开关闭图标
        function mouseEnter(flag){ //处理鼠标进入或者移出变色
            element.color = flag ? "cyan":"darkGray"
            rectangle.color = flag ? "#292727":"#333232"
        }


        function getVolumeIcon(volume)
        {
            if(volume === 0){
                return ":/player_images/media_icons/play_volume_close.png"
            }
            else if(volume >= 50){
                return ":/player_images/media_icons/play_volume_high.png"
            }else if(volume <= 50){
                return ":/player_images/media_icons/play_volume_low.png"
            }
            return ":/player_images/media_icons/play_volume_close.png"
        }


        function reset_player_ui(){
            //2、显示选择视频文件的提示
            // rectangle.visible = true
            console.log("Resetting UI...");
            //恢复背景与播放路径
            videoPath = ""
            qVideoOutput.updateBackgroupImage(!qPlayer.getStrBackgroundPath() ? "" : qPlayer.getStrBackgroundPath())

            //恢复时间
            totalDuration.text = "00:00:00";
            currDuration.text = "00:00:00";

            //reset playProgressBar
            // controlBar.playProgressBar.enabled = 0;
            // controlBar.playProgressBar.value = 0;
        }


        property string lastFileUrl: ""; // 定义一个全局变量来存储上次播放的文件路径

        function saveFileUrl(fileUrl) {
            lastFileUrl = fileUrl;
            console.log("Saved file URL: " + lastFileUrl);
        }

        function rePlay() {
            if (lastFileUrl !== "") {
                qVideoOutput.urlPass(lastFileUrl);
                console.log("Replaying file: " + lastFileUrl);
            } else {
                console.log("No file URL saved to replay.");
            }
        }
        function reset_replayer_ui(){
            //2、显示选择视频文件的提示
            rectangle.visible = false

            //恢复背景
            // qVideoOutput.updateBackgroupImage(!qPlayer.getStrBackgroundPath() ? "" : qPlayer.getStrBackgroundPath())

            //恢复时间
            totalDuration.text = "00:00:00";
            currDuration.text = "00:00:00";

            //reset playProgressBar
            playProgressBar.enabled = 0;
            playProgressBar.value = 0;
        }
}














































//     Rectangle {
//         id : controlBar
//         x: 0
//         y: 576
//         width: mediaPlayMain.width
//         height: 70
//         // color: "#262626";
//         color: "transparent" // 设置背景颜色为透明
// //        anchors.bottomMargin: 500
//         anchors.top: qVideoOutput.bottom  // 并没有bottmo属性？
// //        anchors.bottom: qVideoOutput.bottom
// //        anchors.bottomMargin: -18
//         Layout.alignment: Qt.AlignLeft | Qt.AlignBottom  // 对准：靠左靠底
//         transformOrigin: Item.Bottom

// // //        playProgressBar
//         Slider {
//             id: playProgressBar
//             x: 0
//             y: 2
//             value: 0.0
//             padding: 2
//             width: controlBar.width   //进度条控件占用长度
//             height: 10  //进度条控件占用宽度
//             // ⚠️:
//             // visible: qVideoOutput.getPlayState() === 1  // 播放时才能看到进度条
//             // enabled: qVideoOutput.getPlayState() === 1 // 当 play_state 为 1 时启用滑动
//             anchors.rightMargin: -20
//             anchors.leftMargin: -20
//             onValueChanged: {
//                 console.log("qVideoOutput.getPlayState()-->", qVideoOutput.getPlayState())
//                 if (qVideoOutput.currPlayTime !== playProgressBar.value) {
//                     // 调用slots，实现通过ui进行seek,到seek_time位置
//                     console.log("video ProgressBar val-->" + playProgressBar.value)
//                     qVideoOutput.playSeek(playProgressBar.value)
//                 }
//             }
//         GroupBox {
//             id: timeGroupBox
//             x: 166
//             y: 30
//             width: 399
//             height: 40  // 增加高度以适应文本元素
//             title: qsTr("")
//             anchors.top: playProgressBar.bottom

//             Row {
//                 anchors.left: parent.left  // 将Row靠左对齐
//                 anchors.leftMargin: -160  // 调整左边距
//                 spacing: 10  // 调整元素之间的间距
//                 anchors.verticalCenter: parent.verticalCenter  // 垂直居中对齐

//                 Text {
//                     id: currDuration
//                     color: "#f9f9f9"
//                     text: qsTr("00:00:00")
//                     font.pixelSize: 12
//                 }

//                 Text {
//                     color: "#f2f1f1"
//                     text: qsTr("/")
//                     font.pixelSize: 12
//                 }

//                 Text {
//                     id: totalDuration
//                     color: "#FF9999"
//                     text: qsTr("00:00:00")
//                     font.pixelSize: 12
//                 }
//             }
//         }

//         Label {
//             id: groupBox
//             x: 166
//             y: 30
//             width: 399
//             height: 44
//             anchors.horizontalCenterOffset: -13
//             scale: 1
//             font.pointSize: 0.10 * width
//             //垂直居中
//             anchors.verticalCenter: parent.verticalCenter
//             anchors.verticalCenterOffset: 4
//             //水平居中
//             anchors.horizontalCenter:  parent.horizontalCenter
//             anchors.topMargin: 15
//             anchors.top: parent.top

//             //停止按钮
//             Button {
//                 id: btnStopPlay
//                 x: 16
//                 y: 0
//                 width: 48
//                 height: 48
//                 text: qsTr("暂停")
// // 偷鸡
//                 onClicked: {
//                     console.log("停止播放，如需重播请双击")
//                     //1、关闭正在播放的视频
//                     qVideoOutput.stopPlay();

//                     //2、重置ui
//                     reset_player_ui();
//                 }
//             }

// //             //快退按钮
//             Item {
//                 id: btnFastReversePlay
//                 x: 67
//                 y: 0
//                 width: 48
//                 height: 48

//                 // 自定义背景
//                 Rectangle {
//                     id: backgroundRect
//                     width: parent.width
//                     height: parent.height
//                     color: "transparent"  // 背景透明以显示图片
//                     // color: "#ffffff"  // 背景颜色
//                     radius: 6  // 圆角效果

//                     // 嵌入图片
//                     Image {
//                         anchors.centerIn: parent
//                         width: 30
//                         height: 30
//                         source: ":/player_images/media_icons/pause.png"
//                         fillMode: Image.PreserveAspectFit
//                     }

//                     // 使用 MouseArea 处理点击事件
//                     MouseArea {
//                         anchors.fill: parent  // 让 MouseArea 填充整个按钮区域
//                         onClicked: {
//                             console.log("快退")
//                             qVideoOutput.playDown();  // 触发相应的操作
//                             console.log("点击");
//                         }

//                         // 鼠标悬停效果
//                         onPressed: {
//                             backgroundRect.color = "#000000";  // 按下时改变颜色
//                             console.log("onPressed");
//                         }

//                         onReleased: {
//                             backgroundRect.color = "#262626";  // 松开时恢复颜色
//                             console.log("onReleased");
//                         }
//                     }
//                 }
//             }

//             //开始播放
//             Button {
//                 id: btnStartPlay
//                 x: 107
//                 y: 0
//                 width: 48
//                 height: 48
//                 anchors.left: btnFastReversePlay.right
//                 text: qsTr("开始")
//                 anchors.leftMargin: 27
//                 // contentItem: Image {
//                 //     id:btnStartPlayIcon
//                 //     width: 48
//                 //     height: 48
//                 //     transformOrigin: Item.Bottom
//                 //     source: ":/player_images/media_icons/play.png"
//                 //     fillMode: Image.PreserveAspectFit
//                 // }
//                 // background: Rectangle {
//                 //     implicitWidth: parent.width
//                 //     implicitHeight: parent.height
//                 //     color: "#262626"
//                 //     radius: 6
//                 // }
//                 onClicked: {
//                     console.log("Playing...")
// // 开始播放
//                     console.log("play_state1="+play_state)
//                     play_state = qVideoOutput.startPlay();
//                     console.log("play_state2="+play_state)
// // 切换播放暂停的图片
//                     if(play_state === 1)
//                     {
//                         btnStartPlayIcon.source = ":/player_images/media_icons/play.png"
//                     }else if(play_state === 2){
//                         btnStartPlayIcon.source = ":/player_images/media_icons/pause.png"
//                     }

//                 }
//             }

// //             //快进播放
//             Button {
//                 id: btnFastForwardPlay
//                 x: 147
//                 y: 0
//                 width: 30
//                 height: 30
//                 anchors.left: btnStartPlay.right
//                 text: qsTr(" ")
//                 anchors.leftMargin: 27
//                 // contentItem: Image {
//                 //     width: 48
//                 //     height: 48
//                 //     transformOrigin: Item.Bottom
//                 //     source: "qrc:images/media_icons/fast_forward.png"
//                 //     fillMode: Image.PreserveAspectFit
//                 // }
//                 // background: Rectangle {
//                 //     implicitWidth: parent.width
//                 //     implicitHeight: parent.height
//                 //     color: "#262626"
//                 //     radius: 6
//                 // }
//                 onClicked: {
//                     console.log("快进")
//                     qVideoOutput.playUp();
//                 }
//             }

//             //声音图标
//             Button {
//                 id: btnAudioIcon
//                 x: 187
//                 y: -4
//                 width: 30
//                 height: 30
//                 anchors.left: btnFastForwardPlay.right
//                 text: qsTr(" ")
//                 anchors.leftMargin: 27
//                 // contentItem: Image {
//                 //     width: 48
//                 //     height: 48
//                 //     transformOrigin: Item.Bottom
//                 //     source: getVolumeIcon(volumeCtrl.value)
//                 //     fillMode: Image.PreserveAspectFit
//                 // }
//                 // background: Rectangle {
//                 //     implicitWidth: parent.width
//                 //     implicitHeight: parent.height
//                 //     color: "#262626"
//                 //     radius: 6
//                 // }
//                 onClicked: {
//                     btnAudioIcon_source = btnAudioIcon.contentItem.source;

//                     if(btnAudioIcon_source.indexOf("play_volume_close.png") !== -1)
//                     {
//                         //get curr audio volume value
//                         btnAudioIcon.contentItem.source = getVolumeIcon(qVideoOutput.getVolume())
//                     }

//                     if(btnAudioIcon_source.indexOf("play_volume_low.png") !== -1 || btnAudioIcon_source.indexOf("play_volume_high.png") !== -1)
//                     {
//                         qVideoOutput.updateVolume(0);
//                         btnAudioIcon.contentItem.source = "qrc:/images/media_icons/play_volume_close.png"
//                     }
//                 }
//             }

//             //音量控制条
//             //自定义进度条
//             Slider {
//                 id: volumeCtrl
//                 y: 6
//                 from: 0
//                 value: qPlayer.getPlayerVolume()
//                 to: 128
//                 width: 100
//                 height: 10
//                 anchors.leftMargin: 16
//                 anchors.left: btnAudioIcon.right

//                 // // 自定义滑块的样式
//                 // handle: Rectangle {
//                 //   width: 0
//                 //   height: 0
//                 //   opacity: 0
//                 // }

//                 // background: Rectangle {
//                 //     color: "#333333"
//                 //     implicitWidth: 100
//                 //     implicitHeight: 10
//                 //     radius: 5

//                     // 自定义进度条的样式
//                     Rectangle {
//                         width: volumeCtrl.visualPosition * volumeCtrl.width
//                         height: parent.height
//                         color: "#CCCCCC"
//                         radius: 5
//                     }
//                 }

//                 // onMoved: {
//                 //     var volume = this.value;
//                 //     qVideoOutput.updateVolume(volume);
//                 //     btnAudioIcon.contentItem.source = getVolumeIcon(volume)
//                 // }
//             }



//             Button {
//                 id: btn_theme
//                 x: 458
//                 y: -5
//                 width: 32
//                 height: 30
//                 anchors.left: volumeCtrl.right
//                 text: qsTr(" ")
//                 anchors.leftMargin: 50
//                 anchors.bottomMargin: 10
//                 // contentItem: Image {
//                 //     width: 48
//                 //     height: 48
//                 //     transformOrigin: Item.Bottom
//                 //     source: ":/player_images/ui_icons/theme.png"
//                 //     fillMode: Image.PreserveAspectFit
//                 // }
//                 // background: Rectangle {
//                 //     implicitWidth: parent.width
//                 //     implicitHeight: parent.height
//                 //     color: "#262626"
//                 //     radius: 6
//                 // }
//                 onClicked: {
//                     console.log("Change theme...")
//                     chooseTheme.open();
//                 }
//             }

//         }
//     }
