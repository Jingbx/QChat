/*
 * @File        global.h
 * @brief
 *
 * @Author      Jingbx
 * @Data        2024/12/10
 * @History
 */
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <QString>
#include <QDebug>
#include <QUrl>
#include <QDir>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QMovie>
#include <QSettings>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QRegularExpression>
#include <QSocketDescriptor>
#include <QObject>
#include "QStyle"

#include <iostream>
#include <memory.h>
#include <mutex>
#include <functional>

/**
 * XOR password length加密
 */
extern std::function<QString(QString)> xorString;
extern std::function<void(QLabel *, QString)> setMovLabel;
extern QString gate_url_prefix;
extern int TCP_HEAD_LEN;

/**
 * @brief repolish刷新qss样式(err_tip)
 */
extern std::function<void(QWidget *)> repolish;

/**
 * @brief 请求类型
 */
enum ReqId {
    ID_GET_VERICODE     = 1001,
    ID_REGISTER_USER    = 1002,
    ID_RESET_PWD        = 1003,
    ID_USER_LOGIN       = 1004,
    ID_CHAT_LOGIN       = 1005,
    ID_CHAT_LOGIN_RSP   = 1006,             // user_login后发送的tcp长连接请求的回包
    ID_SEARCH_USER_REQ  = 1007,
    ID_SEARCH_USER_RSP  = 1008,             //搜索用户回包
    ID_ADD_FRIEND_REQ           = 1009,     //添加好友申请
    ID_ADD_FRIEND_RSP           = 1010,     //申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ    = 1011,     //通知用户添加好友申请
    ID_AUTH_FRIEND_REQ          = 1013,     //认证好友请求
    ID_AUTH_FRIEND_RSP          = 1014,     //认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ   = 1015,     //通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ        = 1017,     //文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP        = 1018,     //文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019,     //通知用户文本聊天信息
};

/**
 * @brief Qt前端内部错误码
 */
enum ErrorCodes {
    SUCCESS             = 0,
    ERR_JSON            = 1,
    ERR_NETWORK         = 2,
};

/**
 * @brief 用于前端内部判断输入错误判断,用于前端提示
 *        通过Map来映射错误打印，如果该功能正确则从Map中移除该错误
 */
enum TipErrorCode {
    TIP_ERR_SUCCESS         = 0,
    TIP_ERR_EMAIL_ERR       = 1,
    TIP_ERR_USER_ERR        = 2,
    TIP_ERR_PWD_ERR         = 3,
    TIP_ERR_CONFIRM_ERR     = 4,
    TIP_ERR_PWD_CONFIRM     = 5,
    TIP_ERR_VERIFY_ERR      = 6      // 仅判断验证码为空情况
};

/**
 * @brief 后端resp返回错误码,用于前端提示
 *        通过Map来映射错误打印，如果该功能正确则从Map中移除该错误
 */
enum BackendErrorCodes{
    BE_ERR_TOOMANYREQ                       = 101,   // TODO
    BE_ERR_VERIFY_EXPIRED                   = 102,
    BE_ERR_VERIFY_ERR                       = 103,
    BE_ERR_USER_OR_EMAIL_EXISTED            = 104,
    BE_ERR_NO_MATCHED_EMAIL                 = 105,
    BE_ERR_UPDATE_PWD                       = 106,
    BE_ERR_USER_LOGIN                       = 107,
    BE_ERR_EMAIL_NOT_EXITED                 = 108,
    BE_ERR_PWD_NOT_MATCH                    = 109,
    BE_ERR_GET_CAHT_SERVER                  = 110,
    BE_ERR_UID_INVALID                      = 111,
    BE_ERR_TOKEN_INVALID                    = 112,
    BE_ERR_TOKEN_NOT_MATCH                  = 113
};

/**
 * @brief 发起请求的功能模块
 */
enum Modules {
    REGISTERMOD = 0,
    RESETMOD    = 1,
    LOGINMOD    = 2
};


/**
 * @brief 正则表达式定义
 */
namespace RegexPatterns {
inline constexpr auto EMAIL = R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)";
inline constexpr auto PASSWORD = R"(^[a-zA-Z0-9!@#$%^&*.]{6,15}$)";
}

/**
 * @brief 可点击QLable状态
 */
enum ClickLabelState {
    NORMAL = 0,
    SELECTED = 1
};

enum ClickBtnState {
    BTN_NORMAL = 0,
    BTN_SELECTED = 1
};


struct ServerInfo {
    QString     host;
    QString     port;
    QString     token;
    int         uid;
};


//聊天界面几种模式
enum ChatUIMode{
    SearchMode, //搜索模式
    ChatMode, //聊天模式
    ContactMode, //联系模式
};

//自定义QListWidgetItem的几种类型
enum ListItemType{
    CHAT_USER_ITEM, //聊天用户
    CONTACT_USER_ITEM, //联系人用户
    SEARCH_USER_ITEM, //搜索到的用户
    ADD_USER_TIP_ITEM, //提示添加用户
    INVALID_ITEM,  //不可点击条目
    GROUP_TIP_ITEM, //分组提示条目
    LINE_ITEM,  //分割线
    APPLY_FRIEND_ITEM, //好友申请
};

// 气泡
enum class ChatRole
{
    Self,
    Other
};
// MessageTextEdit
struct MsgInfo{
    QString msgFlag;//"text,image,video,file"
    QString content;//表示文件和图像的url,文本信息
    QPixmap pixmap;//文件和图片的缩略图
};


// 好友
//申请好友标签输入框最低长度
const int MIN_APPLY_LABEL_ED_LEN = 40;

const QString applyname_prefix = "Hello, I am ";

const QString add_prefix = "添加标签 ";

const int  tip_offset = 5;

const int CHAT_COUNT_PER_PAGE = 13;

#endif // GLOBAL_H
