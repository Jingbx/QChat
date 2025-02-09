/*
 * @File        http_manager.h
 * @brief       单例类 负责http请求发送与处理 和golang流程一样
 *
 * @Author      Jingbx
 * @Data        2024/12/10
 * @History
 */
#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H

#include "../base/singleton.h"

class HttpManager : public QObject,
                    public Singleton<HttpManager>,
                    public std::enable_shared_from_this<HttpManager> {
    Q_OBJECT
public:
    friend class Singleton<HttpManager>;
    ~HttpManager(); // ⚠️必须public 父类SingleTon的智能指针析构需要访问HttpManager的析构函数

    /**
     * @brief postHttpReq 发送HTTP请求
     * @param url         发送地址
     * @param json        json data
     * @param req_id      请求类型：验证码/用户注册/密码注册...
     * @param mod         发起请求的模块
     */
    void postHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

private:
    HttpManager();

signals:
    /**
     * @brief signal_http_finish notify mod完成请求发送，返回错误消息以及错误码
     *        总信号
     */
    void signal_http_finished(QString resp, ErrorCodes err_code, ReqId req_id, Modules mod);
    /**
     * @brief 不同mod的分信号
     */
    void signal_register_mod_finished(QString resp, ErrorCodes err, ReqId id);
    void signal_reset_mod_finished(QString resp, ErrorCodes err, ReqId id);
    void signal_login_mod_finished(QString resp, ErrorCodes err, ReqId id);

public slots:
    /**
     * @brief 根据不同的模块分发不同的signal
     */
    void slot_http_finished(QString res, ErrorCodes err, ReqId id, Modules mod);

private:
    QNetworkAccessManager _qnetwork_manager;
};

#endif // HTTP_MANAGER_H
