#include "userinfo_group.h"

UserinfoGroup::UserinfoGroup() {

}

UserinfoGroup::~UserinfoGroup() {

}

QString UserinfoGroup::getIcon()
{
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 默认头像显示";
        return QString(":/resouce/Joy.png");
    }
    auto last = std::prev(_user_info_group.end());
    qDebug() << "获取当前用户头像: " << last->second.icon;
    return last->second.icon;
}

void UserinfoGroup::addUserInfo(const int &uid, const QString &user,
                                const QString &email, const QString &token,
                                const QString &nick, const QString &icon,
                                const int sex) {
    SRUserInfo info;
    info.uid = uid; info.user = user; info.email = email; info.token = token;
    info.nick = nick; info.icon = icon; info.sex = sex;
    std::lock_guard<std::mutex> lock(_mtx);
    _user_info_group[uid] = info;
    qDebug() << "Userinfo has been added to info_group: "
             << "<uid:" << uid
             << "> <user:" << user
             << "> <email:" << email
             << "> <token:" << token << ">";
}

void UserinfoGroup::rmUserInfo(const int &uid) {
    if (_user_info_group.find(uid) != _user_info_group.end()) {
        _user_info_group.erase(uid);
    } else {
        qDebug() << "User <uid=" << uid << "> is not exited!";
    }
}

SRUserInfo *UserinfoGroup::getUserInfo(const int &uid) {
    auto iter = _user_info_group.find(uid);
    if (iter != _user_info_group.end()) {
        return &iter->second;
    } else {
        qDebug() << "User <uid=" << uid << "> is not exited!";
        return nullptr;
    }
}

QString UserinfoGroup::getCurrUserName() {
    // 获取 map 的最后一个元素
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 默认username=null user";
        return "null user";
    }
    auto item = std::prev(_user_info_group.end());
    return item->second.user;
}

int UserinfoGroup::getCurrUid() {
    // 获取 map 的最后一个元素
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 默认uid=0";
        return 0;
    }
    // 获取 map 的最后一个元素
    auto item = std::prev(_user_info_group.end());
    return item->second.uid;
}

std::vector<std::shared_ptr<FriendInfo> > UserinfoGroup::getConListPerPage() {
    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟PerPage...";
        return friend_list;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_list = last->second.friend_list;
    size_t begin = _contact_loaded;
    size_t end = _contact_loaded + CHAT_COUNT_PER_PAGE;
    // 1. gt_friend_list空
    if (begin >= gt_friend_list.size()) {
        return friend_list;
    }
    // 2. gt_friend_list小于一页的长度
    if (end >= gt_friend_list.size()) {
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(
                gt_friend_list.begin() + begin,
                gt_friend_list.end()
            );
        return friend_list;
    }
    // 3. gt_friend_list大于一页的长度
    friend_list = std::vector<std::shared_ptr<FriendInfo>>(
            gt_friend_list.begin() + begin,
            gt_friend_list.begin() + end
        );
    return friend_list;
}

void UserinfoGroup::updateContactLoadedCount() {
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟update contact_loaded";
        return ;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_list = last->second.friend_list;
    size_t begin = _contact_loaded;
    size_t end = _contact_loaded + CHAT_COUNT_PER_PAGE;
    // 1. gt_friend_list空
    if (begin >= gt_friend_list.size()) {
        return ;
    }
    // 2. gt_friend_list小于一页的长度
    if (end >= gt_friend_list.size()) {
        _contact_loaded = gt_friend_list.size();
        return;
    }
    // 3. gt_friend_list大于一页的长度
    _contact_loaded = end;
}

bool UserinfoGroup::checkFriendById(const int uid) {
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟friend map查找不到该用户为已有用户";
        return false;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_map = last->second.friend_map;
    auto item  = gt_friend_map.find(uid);
    if (item == gt_friend_map.end()) {
        return false;
    }
    return true;
}

void UserinfoGroup::addFriend(std::shared_ptr<AuthInfo> auth_info) {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟不添加";
        return ;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_map = last->second.friend_map;

    auto friend_info = std::make_shared<FriendInfo>(auth_info);
    gt_friend_map[friend_info->_uid] = friend_info;
}

void UserinfoGroup::addFriend(std::shared_ptr<AuthRsp> auth_rsp) {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟不添加";
        return ;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_map = last->second.friend_map;

    auto friend_info = std::make_shared<FriendInfo>(auth_rsp);
    gt_friend_map[friend_info->_uid] = friend_info;
}

std::vector<std::shared_ptr<FriendInfo> > UserinfoGroup::getChatListPerPage()
{
    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟PerPage...";
        return friend_list;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_list = last->second.friend_list;
    size_t begin = _chat_loaded;
    size_t end = _chat_loaded + CHAT_COUNT_PER_PAGE;
    // 1. gt_friend_list空
    if (begin >= gt_friend_list.size()) {
        return friend_list;
    }
    // 2. gt_friend_list小于一页的长度
    if (end >= gt_friend_list.size()) {
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(
            gt_friend_list.begin() + begin,
            gt_friend_list.end()
            );
        return friend_list;
    }
    // 3. gt_friend_list大于一页的长度
    friend_list = std::vector<std::shared_ptr<FriendInfo>>(
        gt_friend_list.begin() + begin,
        gt_friend_list.begin() + end
        );
    return friend_list;
}

void UserinfoGroup::updateChatLoadedCount()
{
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟update contact_loaded";
        return ;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_list = last->second.friend_list;
    size_t begin = _chat_loaded;
    size_t end = _chat_loaded + CHAT_COUNT_PER_PAGE;
    // 1. gt_friend_list空
    if (begin >= gt_friend_list.size()) {
        return ;
    }
    // 2. gt_friend_list小于一页的长度
    if (end >= gt_friend_list.size()) {
        _contact_loaded = gt_friend_list.size();
        return;
    }
    // 3. gt_friend_list大于一页的长度
    _chat_loaded = end;
}

std::shared_ptr<FriendInfo> UserinfoGroup::getFriendById(int uid)
{
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟无该好友";
        return nullptr;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_map = last->second.friend_map;
    auto item  = gt_friend_map.find(uid);
    if (item == gt_friend_map.end()) {
        return nullptr;
    }
    return *item;
}






// 申请好友列表
std::vector<std::shared_ptr<ApplyInfo>> UserinfoGroup::getApplyList() {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟update apply list";
        return std::vector<std::shared_ptr<ApplyInfo>>();
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_apply_list = last->second.apply_list;              //⚠️引用！！！！！
    qDebug() << "当前用户 gt_apply_list size: " << gt_apply_list.size();
    return gt_apply_list;
}

bool UserinfoGroup::alreadyApply(int from_uid) {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟已经添加";
        return true;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_apply_list = last->second.apply_list;
    for (const auto &gt_apply_info : gt_apply_list) {
        if (gt_apply_info->_uid == from_uid) {
            return true;
        }
    }
    return false;
}

void UserinfoGroup::addApplyList(std::shared_ptr<ApplyInfo> apply_info) {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟不添加到申请列表中";
        return ;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_apply_list = last->second.apply_list;

    gt_apply_list.push_back(apply_info);
}


void UserinfoGroup::appendApplyList(QJsonArray array) {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟不加载后端申请列表";
        return ;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_apply_list = last->second.apply_list;

    // 遍历 QJsonArray 并输出每个元素
    for (const QJsonValue &value : array) {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto status = value["status"].toInt();
        auto info = std::make_shared<ApplyInfo>(uid, name,
                                                        desc, icon, nick, sex, status);
        gt_apply_list.push_back(info);
    }
    qDebug() << "当前用户添加sql中申请列表 gt_apply_list size: " << gt_apply_list.size();
}

void UserinfoGroup::appendFriendList(QJsonArray array)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟不加载后端好友列表";
        return ;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_list = last->second.friend_list;
    auto &gt_friend_map = last->second.friend_map;

    // 遍历 QJsonArray 并输出每个元素
    for (const QJsonValue& value : array) {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto back = value["back"].toString();

        auto info = std::make_shared<FriendInfo>(uid, name,
                                                 nick, icon, sex, desc, back);
        gt_friend_list.push_back(info);
        gt_friend_map.insert(uid, info);
    }
    qDebug() << "当前用户添加sql中好友列表 gt_apply_list size: " << gt_friend_list.size();
}

void UserinfoGroup::appendFriendChatMsg(int friend_id,
                                        std::vector<std::shared_ptr<TextChatData> > msgs)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟不加载朋友消息列表";
        return ;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_map = last->second.friend_map;
    auto find_iter = gt_friend_map.find(friend_id);
    if(find_iter == gt_friend_map.end()){
        qDebug()<<"append friend uid  " << friend_id << " not found";
        return;
    }

    find_iter.value()->AppendChatMsgs(msgs);
}

bool UserinfoGroup::isLoadConFin()
{
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟isLoadChatFin=true";
        return true;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_list = last->second.friend_list;

    if (_contact_loaded >= gt_friend_list.size()) {
        return true;
    }

    return false;
}



// friend

bool UserinfoGroup::isLoadChatFin() {
    if (_user_info_group.size() == 0) {
        qDebug() << "无用户登录, 模拟isLoadChatFin=true";
        return true;
    }
    auto last = std::prev(_user_info_group.end());
    auto &gt_friend_list = last->second.friend_list;

    if (_chat_loaded >= gt_friend_list.size()) {
        return true;
    }

    return false;
}


