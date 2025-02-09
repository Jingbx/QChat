/*
 * @File        video_player_widget.h
 * @brief       新窗口创建播放器线程，根据url播放
 *
 * @Author      Jingbx
 * @Data        2024/12/26
 * @History
 */
#ifndef VIDEO_PLAYER_WIDGET_H
#define VIDEO_PLAYER_WIDGET_H

#include <QWidget>
#include <QQuickView>  // 使用 QQuickView
#include <QQmlContext>

class VideoPlayerWidget : public QObject
{
    Q_OBJECT

public:
    explicit VideoPlayerWidget(QObject *parent = nullptr);
    explicit VideoPlayerWidget(QObject *parent = nullptr, const QString &videoPath = "");
    ~VideoPlayerWidget();

    void setVideoSource(const QString &videoPath);
    void stopQVideoOutput();

private:
    QQuickView *view;  // 使用 QQuickView 而不是 QQuickWidget
};

#endif // VIDEO_PLAYER_WIDGET_H
