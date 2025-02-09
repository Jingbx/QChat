import QtQuick 2.0
// 导入APP中注册的类
import com.jingbx.QPlayer.QPlayer 1.0
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls.Basic
import QtQuick.Controls 2.2
import QtQuick.Controls.macOS



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
            if(qVideoOutput.currAVTitle.length !== 0)
            {
                Qt.application.title = titleStr + qVideoOutput.currAVTitle + titleStr;
                // mediaPlayMain.title = titleStr + qVideoOutput.currAVTitle + titleStr;
            }else{
                mediaPlayMain.title = titleStr;
            }

        }
    }

    //中，内容区域
    Label {
        id: contentArea
        x: 8
        y: 46  // ?
        width: mediaPlayMain.width
        height: mediaPlayMain.height - controlBar.height - 10
        horizontalAlignment: Text.AlignLeft
        anchors.rightMargin: 5
        anchors.leftMargin: 5
        anchors.bottomMargin: 5
    }


    //下,控制栏
    Rectangle {
        id : controlBar
        x: 0
        y: 576
        width: mediaPlayMain.width
        height: 70
        color: "#262626";
//        anchors.bottomMargin: 500
        anchors.top: qVideoOutput.bottom  // 并没有bottmo属性？
//        anchors.bottom: qVideoOutput.bottom
//        anchors.bottomMargin: -18
        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom  // 对准：靠左靠底
        transformOrigin: Item.Bottom

//        playProgressBar
        Slider {
            id: playProgressBar
            x: 0
            y: 2
            value: 0.0
            padding: 2
            width: controlBar.width   //进度条控件占用长度
            height: 10  //进度条控件占用宽度
            // visible: qVideoOutput.getPlayState() === 1  // 播放时才能看到进度条
            enabled: qVideoOutput.getPlayState() === 1 // 当 play_state 为 1 时启用滑动
            // 自定义滑块的样式
            // 自定义滑块的样式
            // handle: Item {
            //     x: -20
            //     y: -16.5
            //     width: 40
            //     height: 33
            //     Image {
            //         source: ":/player_images/media_icons/rainbow_handle.png"
            //         anchors.centerIn: parent
            //     }
            // }

            anchors.rightMargin: -20
            anchors.leftMargin: -20
            background: Rectangle {
               implicitWidth: 200
               implicitHeight: 6
               color: "#e6e6e6"
               radius: 3
            }
            // 滑动程度：内容部分
            contentItem: Item {
                y: -4
                implicitWidth: 200
                implicitHeight: 4

               Rectangle {
                   width: playProgressBar.visualPosition * parent.width
                   height: parent.height
                   radius: 2
                   color: "#00CCFF"
               }
            }
            onValueChanged: {
                console.log("qVideoOutput.getPlayState()-->", qVideoOutput.getPlayState())
                if (qVideoOutput.currPlayTime !== playProgressBar.value) {
                    // 调用slots，实现通过ui进行seek,到seek_time位置
                    console.log("video ProgressBar val-->" + playProgressBar.value)
                    qVideoOutput.playSeek(playProgressBar.value)

                    // 更新进度条contentItem
                    // playProgressBar.value = playProgressBar.visualPosition * playProgressBar.contentItem.width
                }
            }

            // Connections {
            //     target: qVideoOutput
            //     onCurrPlayTimeChanged: {  // 监听qVideoOutput的变化
            //         playProgressBar.value = qVideoOutput.currPlayTime
            //         progressBarContent.width = playProgressBar.visualPosition * playProgressBar.contentItem.width
            //     }
            // }
        }


        GroupBox {
            id: timeGroupBox
            x: 166
            y: 30
            width: 399
            height: 40  // 增加高度以适应文本元素
            title: qsTr("")
            anchors.top: playProgressBar.bottom

            Row {
                anchors.left: parent.left  // 将Row靠左对齐
                anchors.leftMargin: -160  // 调整左边距
                spacing: 10  // 调整元素之间的间距
                anchors.verticalCenter: parent.verticalCenter  // 垂直居中对齐

                Text {
                    id: currDuration
                    color: "#f9f9f9"
                    text: qsTr("00:00:00")
                    font.pixelSize: 12
                }

                Text {
                    color: "#f2f1f1"
                    text: qsTr("/")
                    font.pixelSize: 12
                }

                Text {
                    id: totalDuration
                    color: "#FF9999"
                    text: qsTr("00:00:00")
                    font.pixelSize: 12
                }
            }
        }


        Label {
            id: groupBox
            x: 166
            y: 30
            width: 399
            height: 44
            anchors.horizontalCenterOffset: -13
            scale: 1
            font.pointSize: 0.10 * width
            //垂直居中
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 4
            //水平居中
            anchors.horizontalCenter:  parent.horizontalCenter
            anchors.topMargin: 15
            anchors.top: parent.top

            //停止按钮
            Button {
                id: btnStopPlay
                x: 16
                y: 14
                width: toolBarCloseW
                height: toolBarCloseH
                text: qsTr(" ")
                contentItem: Image {
                    width: 96
                    height: 96  // todo
                    transformOrigin: Item.Bottom
                    source: ":/player_images/media_icons/replay.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
// 偷鸡
                onClicked: {
                    console.log("停止播放，如需重播请双击")
                    //1、关闭正在播放的视频
                    qVideoOutput.stopPlay();

                    //2、重置ui
                    reset_player_ui();
                }
                onDoubleClicked: {
                    console.log("重播")
                    qVideoOutput.stopPlay();
                    //2、重置ui
                    reset_replayer_ui();
                    //3、重新开始播放视频
                    rePlay();
                }
            }

            //快退按钮
            Button {
                id: btnFastReversePlay
                x: 97
                y: 13
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.left: btnStopPlay.right
                text: qsTr(" ")
                anchors.leftMargin: 27
                contentItem: Image {
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: ":/player_images/media_icons/fast_backward.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {  // slots
                    console.log("快退")
                    qVideoOutput.playDown();
                }
            }



            //开始播放
            Button {
                id: btnStartPlay
                x: 97
                y: 13
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.left: btnFastReversePlay.right
                text: qsTr(" ")
                anchors.leftMargin: 27
                contentItem: Image {
                    id:btnStartPlayIcon
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: ":/player_images/media_icons/play.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    console.log("Playing...")
// 开始播放
                    console.log("play_state1="+play_state)
                    play_state = qVideoOutput.startPlay();
                    console.log("play_state2="+play_state)
// 切换播放暂停的图片
                    if(play_state === 1)
                    {
                        btnStartPlayIcon.source = ":/player_images/media_icons/play.png"
                    }else if(play_state === 2){
                        btnStartPlayIcon.source = ":/player_images/media_icons/pause.png"
                    }

                }
            }

            //快进播放
            Button {
                id: btnFastForwardPlay
                x: 97
                y: 13
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.left: btnStartPlay.right
                text: qsTr(" ")
                anchors.leftMargin: 27
                contentItem: Image {
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: "qrc:images/media_icons/fast_forward.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    console.log("快进")
                    qVideoOutput.playUp();
                }
            }

            //声音图标
            Button {
                id: btnAudioIcon
                x: 97
                y: 9
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.left: btnFastForwardPlay.right
                text: qsTr(" ")
                anchors.leftMargin: 27
                contentItem: Image {
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: getVolumeIcon(volumeCtrl.value)
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    btnAudioIcon_source = btnAudioIcon.contentItem.source;

                    if(btnAudioIcon_source.indexOf("play_volume_close.png") !== -1)
                    {
                        //get curr audio volume value
                        btnAudioIcon.contentItem.source = getVolumeIcon(qVideoOutput.getVolume())
                    }

                    if(btnAudioIcon_source.indexOf("play_volume_low.png") !== -1 || btnAudioIcon_source.indexOf("play_volume_high.png") !== -1)
                    {
                        qVideoOutput.updateVolume(0);
                        btnAudioIcon.contentItem.source = ":/player_images/media_icons/play_volume_close.png"
                    }
                }
            }

            //音量控制条
            //自定义进度条
            Slider {
                id: volumeCtrl
                y: 19
                from: 0
                value: qPlayer.getPlayerVolume()
                to: 128
                width: 100
                height: 10
                anchors.leftMargin: 16
                anchors.left: btnAudioIcon.right

                // 自定义滑块的样式
                handle: Rectangle {
                  width: 0
                  height: 0
                  opacity: 0
                }

                background: Rectangle {
                    color: "#333333"
                    implicitWidth: 100
                    implicitHeight: 10
                    radius: 5

                    // 自定义进度条的样式
                    Rectangle {
                        width: volumeCtrl.visualPosition * volumeCtrl.width
                        height: parent.height
                        color: "#CCCCCC"
                        radius: 5
                    }
                }

                onMoved: {
                    var volume = this.value;
                    qVideoOutput.updateVolume(volume);
                    btnAudioIcon.contentItem.source = getVolumeIcon(volume)
                }
            }


            Button {
                id: btn_theme
                x: 458
                y: 8
                width: 32
                height: 30
                anchors.left: volumeCtrl.right
                text: qsTr(" ")
                anchors.leftMargin: 50
                anchors.bottomMargin: 10
                contentItem: Image {
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: ":/player_images/ui_icons/theme.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    console.log("Change theme...")
                    chooseTheme.open();
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

    /**
      *
      **/
    function getSelectFileUrl(chooseFileDialog){
        var fileUrl = "/Users/jingbx/Downloads/ffmpeg命令行测试/Jingyue_10s.mp4";
        if(chooseFileDialog.fileUrls !== undefined){
            fileUrl = chooseFileDialog.fileUrls;
        }else if(chooseFileDialog.currentFile !== undefined){
            fileUrl = chooseFileDialog.currentFile;
        }else if(chooseFileDialog.file !== undefined){
            fileUrl = chooseFileDialog.file;
        }
        console.log("GetSelectFileUrl You chose: 测试/Users/jingbx/Downloads/ffmpeg命令行测试/Jingyue_10s.mp4" + fileUrl);

        if(fileUrl === ""){
            return "";
        }

        return fileUrl.toString().substring(7,fileUrl.toString().length);
    }

    function reset_player_ui(){
        //2、显示选择视频文件的提示
        rectangle.visible = true

        //恢复背景
        qVideoOutput.updateBackgroupImage(!qPlayer.getStrBackgroundPath() ? "" : qPlayer.getStrBackgroundPath())

        //恢复时间
        totalDuration.text = "00:00:00";
        currDuration.text = "00:00:00";

        //reset playProgressBar
        playProgressBar.enabled = 0;
        playProgressBar.value = 0;
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

    Rectangle {
        id: rectangle
        x: 1
        y: 1
        width: 48
        height: 48
        color: "transparent" // 设置背景颜色为透明
        border.color: "#00000000"


        MouseArea {
            id: mouseId
            anchors.fill: parent
            hoverEnabled: true
            onEntered: { //监听鼠标进入
                mouseEnter(true);
            }
            onExited: { //监听鼠标移出
                 mouseEnter(false);
            }
            onClicked: {
                console.log("Open file...")
                chooseVideo.open();
            }
        }

        Image {
            id: image1
            x: 1
            y: 1
            width: 48
            height: 48
            fillMode: Image.PreserveAspectFit
            source: ":/player_images/ui_icons/menu_icon.png"
        }

        Text {
            id: element
            x: 1
            y: 1
            width: 1
            height: 1
            color: "#f7f7f7"
            // text: qsTr("打开文件")
            // font.pixelSize: 37
        }
    }

    //打开视频文件框
    FileDialog {
        id: chooseVideo
        title: qsTr("Chose your video file...")
        nameFilters: [qsTr("Video files(*.mp4 *.flv)"),qsTr("All files(*)")]
        onAccepted: {
            var fileUrl = getSelectFileUrl(chooseVideo);
            console.log("The path of file : " + fileUrl);

            if(fileUrl !== ""){
                //隐藏选择视频文件的提示
                rectangle.visible = false;
                saveFileUrl(fileUrl);
//                var fileUrl2 = fileUrl.toString().substring(7,fileUrl.length);
                qVideoOutput.urlPass(fileUrl);
    //            uVideoOutput.urlPass("/dev/video0");
            }
        }
        onRejected: {
            console.log("Canceled");
        }
    }

    //换皮肤
    FileDialog {
        id: chooseTheme
        title: qsTr("Chose background image...")
        nameFilters: [qsTr("Image files(*.png *.jpg *jpeg)"),qsTr("All files(*)")]
        onAccepted: {

            var fileUrl = getSelectFileUrl(chooseTheme);
            if(fileUrl !== ""){
                console.log("You chose: " + fileUrl);
                qVideoOutput.updateBackgroupImage(fileUrl);
            }

        }
        onRejected: {
            console.log("Canceled");
        }
    }


    //内容区域


    //下部区域，开始、暂停，快进，快退，声音大小，进度条等。

}
