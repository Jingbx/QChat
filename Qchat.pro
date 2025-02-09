# qt
QT       += core gui
QT       += network widgets multimedia multimediawidgets
QT       += quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# logo & output path
ICON = qchat_logo.icns
DESTDIR = ./bin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chat_src/custom_ui/applyfriend.cpp \
    chat_src/custom_ui/applyfrienditem.cpp \
    chat_src/custom_ui/applyfriendpage.cpp \
    chat_src/custom_ui/authenfriend.cpp \
    chat_src/custom_ui/conuser_item.cpp \
    chat_src/custom_ui/empty_page.cpp \
    chat_src/custom_ui/friendinfopage.cpp \
    chat_src/custom_ui/friendlabel.cpp \
    chat_src/custom_ui/group_tip_item.cpp \
    chat_src/custom_ui_wid/apply_friend_list.cpp \
    chat_src/custom_ui_wid/clicked_once_label.cpp \
    chat_src/custom_ui_wid/contact_user_list.cpp \
    main.cpp \
    chat_src/base/global.cpp \
    chat_src/bubble/BubbleFrame.cpp \
    chat_src/bubble/PictureBubble.cpp \
    chat_src/bubble/TextBubble.cpp \
    chat_src/bubble/VideoBubble.cpp \
    chat_src/custom_ui/add_user_item.cpp \
    chat_src/custom_ui/chat_page.cpp \
    chat_src/custom_ui/chat_user_wid.cpp \
    chat_src/custom_ui/chatdialog.cpp \
    chat_src/custom_ui/findfaildlg.cpp \
    chat_src/custom_ui/findsuccessdlg.cpp \
    chat_src/custom_ui/loadingdialog.cpp \
    chat_src/custom_ui/logindialog.cpp \
    chat_src/custom_ui/mainwindow.cpp \
    chat_src/custom_ui/registerdialog.cpp \
    chat_src/custom_ui/resetdialog.cpp \
    chat_src/custom_ui_wid/ChatItemBase.cpp \
    chat_src/custom_ui_wid/ChatView.cpp \
    chat_src/custom_ui_wid/MessageTextEdit.cpp \
    chat_src/custom_ui_wid/chat_user_list.cpp \
    chat_src/custom_ui_wid/clicked_btn.cpp \
    chat_src/custom_ui_wid/clicked_label.cpp \
    chat_src/custom_ui_wid/customize_edit.cpp \
    chat_src/custom_ui_wid/list_item_base.cpp \
    chat_src/custom_ui_wid/search_list.cpp \
    chat_src/custom_ui_wid/state_label.cpp \
    chat_src/custom_ui_wid/state_widget.cpp \
    chat_src/custom_ui_wid/video_player_widget.cpp \
    chat_src/network_manager/http_manager.cpp \
    chat_src/network_manager/tcp_manager.cpp \
    chat_src/user_manager/user_data.cpp \
    chat_src/user_manager/userinfo_group.cpp \
    player_src/media/MediaCamera.cpp \
    player_src/media/MediaCore.cpp \
    player_src/media/MediaFFmpeg.cpp \
    player_src/media/MediaFile.cpp \
    player_src/media/PacketQueue.cpp \
    player_src/qt/AVPlayerCore.cpp \
    player_src/qt/AVPlayerUtils.cpp \
    player_src/qt/App.cpp \
    player_src/qt/PlayMedia.cpp \
    player_src/utils/configutils.cpp \

HEADERS += \
    chat_src/base/global.h \
    chat_src/base/singleton.h \
    chat_src/bubble/BubbleFrame.h \
    chat_src/bubble/PictureBubble.h \
    chat_src/bubble/TextBubble.h \
    chat_src/bubble/VideoBubble.h \
    chat_src/custom_ui/add_user_item.h \
    chat_src/custom_ui/applyfriend.h \
    chat_src/custom_ui/applyfrienditem.h \
    chat_src/custom_ui/applyfriendpage.h \
    chat_src/custom_ui/authenfriend.h \
    chat_src/custom_ui/chat_page.h \
    chat_src/custom_ui/chat_user_wid.h \
    chat_src/custom_ui/chatdialog.h \
    chat_src/custom_ui/conuser_item.h \
    chat_src/custom_ui/empty_page.h \
    chat_src/custom_ui/findfaildlg.h \
    chat_src/custom_ui/findsuccessdlg.h \
    chat_src/custom_ui/friendinfopage.h \
    chat_src/custom_ui/friendlabel.h \
    chat_src/custom_ui/group_tip_item.h \
    chat_src/custom_ui/loadingdialog.h \
    chat_src/custom_ui/logindialog.h \
    chat_src/custom_ui/mainwindow.h \
    chat_src/custom_ui/registerdialog.h \
    chat_src/custom_ui/resetdialog.h \
    chat_src/custom_ui_wid/ChatItemBase.h \
    chat_src/custom_ui_wid/ChatView.h \
    chat_src/custom_ui_wid/MessageTextEdit.h \
    chat_src/custom_ui_wid/apply_friend_list.h \
    chat_src/custom_ui_wid/chat_user_list.h \
    chat_src/custom_ui_wid/clicked_btn.h \
    chat_src/custom_ui_wid/clicked_label.h \
    chat_src/custom_ui_wid/clicked_once_label.h \
    chat_src/custom_ui_wid/contact_user_list.h \
    chat_src/custom_ui_wid/customize_edit.h \
    chat_src/custom_ui_wid/list_item_base.h \
    chat_src/custom_ui_wid/search_list.h \
    chat_src/custom_ui_wid/state_label.h \
    chat_src/custom_ui_wid/state_widget.h \
    chat_src/custom_ui_wid/video_player_widget.h \
    chat_src/network_manager/http_manager.h \
    chat_src/network_manager/tcp_manager.h \
    chat_src/user_manager/user_data.h \
    chat_src/user_manager/userinfo_group.h \
    player_src/include/common.h \
    player_src/include/ffmpeg_common.h \
    player_src/include/nlohmann/json.hpp \
    player_src/include/nlohmann/json_fwd.hpp \
    player_src/media/MediaCamera.hpp \
    player_src/media/MediaCore.hpp \
    player_src/media/MediaFFmpeg.hpp \
    player_src/media/MediaFile.hpp \
    player_src/media/PacketQueue.h \
    player_src/qt/AVPlayerCore.hpp \
    player_src/qt/AVPlayerUtils.hpp \
    player_src/qt/App.hpp \
    player_src/qt/PlayMedia.h \
    player_src/utils/configutils.h \

FORMS += \
    chat_src/custom_ui/adduseritem.ui \
    chat_src/custom_ui/applyfriend.ui \
    chat_src/custom_ui/applyfrienditem.ui \
    chat_src/custom_ui/applyfriendpage.ui \
    chat_src/custom_ui/authenfriend.ui \
    chat_src/custom_ui/chatdialog.ui \
    chat_src/custom_ui/chatpage.ui \
    chat_src/custom_ui/chatuserwid.ui \
    chat_src/custom_ui/conuseritem.ui \
    chat_src/custom_ui/emptypage.ui \
    chat_src/custom_ui/findfaildlg.ui \
    chat_src/custom_ui/findsuccessdlg.ui \
    chat_src/custom_ui/friendinfopage.ui \
    chat_src/custom_ui/friendlabel.ui \
    chat_src/custom_ui/grouptipitem.ui \
    chat_src/custom_ui/loadingdialog.ui \
    chat_src/custom_ui/logindialog.ui \
    chat_src/custom_ui/mainwindow.ui \
    chat_src/custom_ui/registerdialog.ui \
    chat_src/custom_ui/resetdialog.ui \


# Default rules for deployment.
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rc.qrc

DISTFILES += \
    config.ini \
    player_src/utils/configutils.json \
    resources/add_friend.png \
    resources/add_friend_hover.png \
    resources/add_friend_hover1.png \
    resources/add_friend_normal.png \
    resources/add_friend_normal1.png \
    resources/addtip.png \
    resources/arowdown.png \
    resources/chat_icon.png \
    resources/chat_icon_hover.png \
    resources/chat_icon_press.png \
    resources/chat_icon_select_hover.png \
    resources/chat_icon_select_press.png \
    resources/close_search.png \
    resources/close_transparent.png \
    resources/contact_list.png \
    resources/contact_list_hover.png \
    resources/contact_list_press.png \
    resources/female.png \
    resources/filedir.png \
    resources/filedir_hover.png \
    resources/filedir_press.png \
    resources/head_.jpg \
    resources/head_1.jpg \
    resources/head_2.jpg \
    resources/head_3.jpg \
    resources/head_4.jpg \
    resources/head_5.jpg \
    resources/ice.png \
    resources/loading.gif \
    resources/male.png \
    resources/msg_chat_hover.png \
    resources/msg_chat_normal.png \
    resources/msg_chat_press.png \
    resources/plane1.gif \
    resources/plane2.gif \
    resources/qchat_qrcode.png \
    resources/red_point.png \
    resources/right_tip.png \
    resources/search.png \
    resources/smile.png \
    resources/smile_hover.png \
    resources/smile_press.png \
    resources/tipclose.png \
    resources/unvisible.png \
    resources/unvisible_hover.png \
    resources/video_chat_hover.png \
    resources/video_chat_normal.png \
    resources/video_chat_press.png \
    resources/visible.png \
    resources/visible_hover.png \
    resources/voice_chat_hover.png \
    resources/voice_chat_normal.png \
    resources/voice_chat_press.png



BUILD_SOURCE_PATH = /opt/homebrew/Cellar
FFMPEG_PATH = /usr/local/ffmpeg_5.1.2
X265_PATH = /usr/local/x265_3.6

INCLUDEPATH += $${FFMPEG_PATH}/include
DEPENDPATH += $${FFMPEG_PATH}/include

INCLUDEPATH += $${BUILD_SOURCE_PATH}/sdl2/2.30.10/include/SDL2
DEPENDPATH += $${BUILD_SOURCE_PATH}/sdl2/2.30.10/include/SDL2

INCLUDEPATH += $${BUILD_SOURCE_PATH}/fdk-aac/2.0.3/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/fdk-aac/2.0.3/include

INCLUDEPATH += $${BUILD_SOURCE_PATH}/x264/r3108/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/x264/r3108/include

INCLUDEPATH += $${X265_PATH}/include
DEPENDPATH += $${X265_PATH}/include

#openssl
INCLUDEPATH += $${BUILD_SOURCE_PATH}/openssl@3/3.4.0/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/openssl@3/3.4.0/include

#zlib
INCLUDEPATH += $${BUILD_SOURCE_PATH}/zlib/1.3.1/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/zlib/1.3.1/include

#librtmp
INCLUDEPATH += $${BUILD_SOURCE_PATH}/rtmpdump/2.4-20151223_3/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/rtmpdump/2.4-20151223_3/include

LIBS += -L$${FFMPEG_PATH}/lib
LIBS += -lavcodec
LIBS += -lavdevice
LIBS += -lavfilter
LIBS += -lavformat
LIBS += -lavutil
LIBS += -lpostproc
LIBS += -lswresample
LIBS += -lswscale

LIBS += -L$${X265_PATH}/lib -lx265
LIBS += -L$${BUILD_SOURCE_PATH}/sdl2/2.30.10/lib -lSDL2
# LIBS += -lasound

LIBS += -lz
LIBS += -L$${BUILD_SOURCE_PATH}/rtmpdump/2.4-20151223_3/lib -lrtmp

#LIBS += -lssl
#LIBS += -lcrypto



# 将文件复制到构建输出目录
CONFIG(debug, debug | release) {
    # 指定要拷贝的文件路径
    TargetConfig = $$PWD/config.ini
    # 输出目录（目标目录）
    OutputDir = $$OUT_PWD/$$DESTDIR
    message("Output Directory: $$OutputDir")
    # 使用 cp 命令复制文件
    QMAKE_POST_LINK += cp -f \"$$TargetConfig\" \"$$OutputDir\"
    # # # 拷贝HTTP静态目录
    # StaticDir = $$PWD/static
    # QMAKE_POST_LINK += cp -r \"$$StaticDir\" \"$$OutputDir\"
} else {
    # 可选择填充调试或发布配置的其他内容
}
