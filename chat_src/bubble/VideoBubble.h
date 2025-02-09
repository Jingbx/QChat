/*
 * @File        VideoBubble.h
 * @brief       Enhanced: 事件过滤增加双击事件监听，callback视频播放和图片放大
 *
 * @Author      Jingbx
 * @Data        2024/12/24
 * @History
 */


/*
 * @File        VideoBubble.h
 * @brief       Enhanced: 事件过滤增加双击事件监听，callback视频播放和图片放大
 *
 * @Author      Jingbx
 * @Data        2024/12/24
 * @History
 */
#ifndef VIDEOBUBBLE_H
#define VIDEOBUBBLE_H

#include "BubbleFrame.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QUrl>

class VideoBubble : public BubbleFrame
{
    Q_OBJECT
public:
    VideoBubble(const QString &videoPath, ChatRole role, QWidget *parent = nullptr);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override; // 事件过滤器
private:
    QString m_videoPath;  // 视频路径
    QMediaPlayer *m_mediaPlayer;
    QVideoWidget *m_videoWidget;
    QLabel *m_thumbnailLabel; // 图片控件用于显示缩略图
signals:
    void signal_video_play(QString videoPath);
private slots:
    void slot_video_play(const QString &videoPath);
};

#endif  // VIDEOBUBBLE_H




















// #ifndef VIDEOBUBBLE_H
// #define VIDEOBUBBLE_H

// #include "BubbleFrame.h"
// #include <QWidget>
// #include <QHBoxLayout>
// #include <QMediaPlayer>
// #include <QVideoWidget>
// #include <QUrl>

// class VideoBubble : public BubbleFrame
// {
//     Q_OBJECT
// public:
//     VideoBubble(const QString &videoPath, ChatRole role, QWidget *parent = nullptr);
// protected:
//     bool eventFilter(QObject *watched, QEvent *event) override; // 事件过滤器
// private:
//     QPixmap grabFirstFrame(const QString &videoPath);
// private:
//     QString m_videoPath;  // 视频路径
//     QMediaPlayer *m_mediaPlayer;
//     QVideoWidget *m_videoWidget;
//     QLabel *m_thumbnailLabel; // 图片控件用于显示缩略图
//     QLabel *m_playIcon;  // 中心显示暂停按钮图标
// signals:
//     void videoDoubleClicked(QString videoPath);
// };

// #endif  // VIDEOBUBBLE_H
