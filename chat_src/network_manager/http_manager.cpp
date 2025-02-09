/*
 * @File        http_manager.cpp
 * @brief
 *
 * @Author      Jingbx
 * @Data        2024/12/10
 * @History
 */

#include "http_manager.h"

HttpManager::HttpManager() {
    //连接http请求和完成信号，信号槽机制保证队列消费
    connect(this, &HttpManager::signal_http_finished, this, &HttpManager::slot_http_finished);
}

HttpManager::~HttpManager() {

}

void HttpManager::postHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod) {
    // 构造HTTP Req
    QNetworkRequest request(url);
    QByteArray data = QJsonDocument(json).toJson(); // ⚠️ 序列化  从内存中的 JSON 数据结构 → JSON 字符串（字节流）；JSON 数据的字符串形式（可以存储或传输）
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    // 发送HTTP Req
    QNetworkReply *reply = _qnetwork_manager.post(request, data);
    // 接收reply信号 等待处理
    auto self = shared_from_this(); // ⚠️ 继承enbale_shared_from_this创建weak_ptr关联该对象，shared_from_this通过weak_ptr创建新shared_ptr
    connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod] () {
        // 错误处理
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Send Http Req error" << reply->errorString();
            emit self->signal_http_finished("", ErrorCodes::ERR_NETWORK, req_id, mod);   // ⚠️
            return;
        }
        // 成功处理
        // fixed: 读取reply
        QString resp = reply->readAll();
        emit self->signal_http_finished(resp, ErrorCodes::SUCCESS, req_id, mod);

        reply->deleteLater();
        return;
    });
}

void HttpManager::slot_http_finished(QString resp, ErrorCodes err, ReqId id, Modules mod) {
    // 1. Register模块
    if (Modules::REGISTERMOD == mod) {
        emit signal_register_mod_finished(resp, err, id);
    }
    // 2.重置模块
    if (Modules::RESETMOD == mod) {
        emit signal_reset_mod_finished(resp, err, id);
    }
    // 3.登录模块
    if (Modules::LOGINMOD == mod) {
        emit signal_login_mod_finished(resp, err, id);
    }
}
