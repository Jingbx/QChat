#ifndef USERINFO_GROUP_H
#define USERINFO_GROUP_H

#include "user_data.h"
#include "../base/global.h"
#include "../base/singleton.h"

struct SRUserInfo { // Server return user info jingbx: 每个用户的属性
    int             uid     = 0;
    QString         user    = "";
    QString         email   = "";
    QString         token   = "";
    QString         nick    = "";
    QString         icon    = "";
    int             sex     = 0;

    std::vector<std::shared_ptr<FriendInfo>>    friend_list;
    std::vector<std::shared_ptr<ApplyInfo>>     apply_list;
    QMap<int, std::shared_ptr<FriendInfo>>      friend_map;
};

class UserinfoGroup : public Singleton<UserinfoGroup> {
    friend class Singleton<UserinfoGroup>;
public:
    ~UserinfoGroup();

    QString getIcon();

    void addUserInfo(const int &uid, const QString &user,
                     const QString &email, const QString &token,
                     const QString &nick, const QString &icon,
                     const int sex);
    void rmUserInfo(const int &uid);
    SRUserInfo *getUserInfo(const int &uid);
    /**
     * @brief 获取当前登录用户的信息
     */
    QString getCurrUserName();
    int getCurrUid();
    /**
     * @brief 逐页获取联系人
     */
    std::vector<std::shared_ptr<FriendInfo>> getConListPerPage();
    void updateContactLoadedCount();

    /**
     * @brief 根据uid检查是否已经是我的好友了
     */
    bool checkFriendById(const int uid);
    /**
     * @brief 在当前用户的内存信息中添加好友用于显示
     */
    void addFriend(std::shared_ptr<AuthInfo> auth_info); // 自己作为添加好友等待验证端
    void addFriend(std::shared_ptr<AuthRsp> auth_rsp);   // 自己作为验证端
    /**
     * @brief 逐页获取好友人
     */
    std::vector<std::shared_ptr<FriendInfo>> getChatListPerPage();
    /**
     * @brief 根据uid获取好友信息
     */
    std::shared_ptr<FriendInfo> getFriendById(int uid);
    /**
     * @brief 更新当前好友聊天加载位置
     */
    void updateChatLoadedCount();

    /**
     * @brief 返回好友/联系人列表是否全部加载完毕
     */
    bool isLoadChatFin();
    bool isLoadConFin();
    /**
     * @brief 从sql中加载好友列表
     */
    void appendFriendList(QJsonArray array);

    /**
     * @brief appendFriendChatMsg
     * @param friend_id
     */
    void appendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData>>msgs);

    /**
     * @brief 获取最近添加联系人列表
     */
    std::vector<std::shared_ptr<ApplyInfo>> getApplyList();

    /**
     * @brief 防止对方多次发送好友请求，客户端疯狂显示
     */
    bool alreadyApply(int from_uid);
    /**
     * @brief 将对方的用户信息添加到内存中，便于前端显示
     */
    void addApplyList(std::shared_ptr<ApplyInfo> apply_info);
    /**
     * @brief 添加后端Login得到的申请好友列表
     */
    void appendApplyList(QJsonArray array);

private:
    UserinfoGroup();
private:
    std::mutex                    _mtx;
    std::map<int, SRUserInfo>     _user_info_group;
    // 记录联系人列表加载到什么位置了
    int                           _contact_loaded;
    // ???
    int                           _chat_loaded;
};

#endif // USERINFO_GROUP_H
