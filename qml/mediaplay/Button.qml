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

import QtQuick 2.2

Rectangle {
    id: root
    color: "transparent"  // 透明
    height: itemHeight
    width: itemWidth
// 属性  property声明的属性更灵活，在元素内外部都可以访问，作用域更大
    property string text
    // property Font fontsize: "14"
    property color bgColor: "transparent"  // 按钮的背景颜色，默认为透明
    property color bgColorSelected: "#14aaff"  // 按钮被点击时的背景颜色
    property color textColor: "white"
    property alias enabled: mouseArea.enabled  // 使 root 的 enabled 属性与 mouseArea.enabled 属性绑定
    property bool active: true  // 按钮是否激活，默认值为 true。
    property alias horizontalAlign: text.horizontalAlignment  // 使 root 的 horizontalAlign 属性与text.horizontalAlignment 属性绑定。

    signal clicked  // 定义点击信号。

    Rectangle {  // 内部的矩形元素，用于显示按钮的背景颜色。
        anchors { fill: parent; margins: 1 }  // 将此矩形填充到父元素（root），并设置边距为 1。
        color: mouseArea.pressed ? bgColorSelected : bgColor

        Text {
            id: text
            clip: true  // 文本剪裁，防止文本超出边界。
            text: root.text  // 文本内容绑定到 root 的 text 属性。   “.”就是绑定？
            anchors { fill: parent; margins: scaledMargin }  // 将文本填充到父元素（内部矩形），并设置边距为 scaledMargin（假设在上层组件中定义）。
            font.pixelSize: fontSize
            color: textColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent  // 将鼠标区域填充到父元素（内部矩形）, 整个矩形可以点击。
            onClicked: {
                root.clicked()  // 定义点击事件处理，当点击时，发射 root 的 clicked 信号。
            }
            enabled: active  // 使 mouseArea 的 enabled 属性绑定到 active 属性。
        }
    }
}
