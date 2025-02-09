/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.1
import QtQuick.Controls 2.0
import QtQuick.Templates 2.12 as T     // T.报错的解决方案

Rectangle {
    id: root
    color: "#151515"
    signal openCamera
    signal openImage
    signal openVideo
    signal close


    Rectangle {
        id: menuField
        height: itemHeight
        width: itemHeight
        color: "transparent"
        anchors.right: parent.right  // == root.right
        anchors.left: parent.left
        Image {
            id: menu
            source: "rc:/player_images/ui_icons/menu_icons.png"
            anchors {
                right: parent.right  // parent就指上一层级的属性 此处为menuField
                top: parent.top
                margins: scaledMargin
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: fileOpen.state == "expanded" ? fileOpen.state = "collapsed" : fileOpen.state = "expanded"
        }
    }

    Column {
        width : 30
        height : 300
        anchors {
            top: menuField.bottom
            right: parent.right
            left: parent.left
            bottom: parent.bottom
            topMargin: 10
        }

        spacing: 10
        visible: fileOpen.state == "expanded"

        Rectangle {
            width: 0.9 * parent.width  // 这里的 parent 指的是包含它的 Column 元素。虽然 Column 本身没有显式设置 width，但它的宽度由它的父级（在本例中是 root）决定。Column 的宽度默认填满其父元素的宽度，因为它的 anchors.left 和 anchors.right 绑定到了 root。
            height: 1
            color: "#353535"
            anchors.left: parent.left
        }
        Button {
            text: "Start camera"
            height: itemHeight
            width: parent.width
            onClicked: {
                fileOpen.state = "collapsed"
                root.openCamera()
            }
        }
        Rectangle {
            width: 0.9 * parent.width
            height: 1
            color: "#353535"
            anchors.left: parent.left
        }
        Button {
            text: "Open image"
            height: itemHeight
            width: parent.width
            onClicked: {
                fileOpen.state = "collapsed"
                root.openImage()
            }
        }
        Rectangle {
            width: 0.9 * parent.width
            height: 1
            color: "#353535"
            anchors.left: parent.left
        }
        Button {
            text: "Open video"
            height: itemHeight
            width: parent.width
            onClicked: {
                fileOpen.state = "collapsed"
                root.openVideo()
            }
        }
        Rectangle {
            width: 0.9 * parent.width
            height: 1
            color: "#353535"
            anchors.left: parent.left
        }
        Button {
            text: "Reset"
            height: itemHeight
            width: parent.width
            onClicked: {
                fileOpen.state = "collapsed"
                root.close()
            }
        }
        Rectangle {
            width: 0.9 * parent.width
            height: 1
            color: "#353535"
            anchors.left: parent.left
        }

        Button {
            text: "111111.mp4"
            height: itemHeight
            width: parent.width
            onClicked: {
                fileOpen.state = "collapsed"
                root.close()
            }
        }
        Rectangle {
            width: 0.9 * parent.width
            height: 1
            color: "#353535"
            anchors.left: parent.left
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
