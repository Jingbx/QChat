#include "VideoBubble.h"
#include <QVBoxLayout>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QUrl>
#include <QDesktopServices>
#include <QEvent>
#include <QMouseEvent>
#include <QThread>
#include "../custom_ui_wid/video_player_widget.h"

#define VIDEO_MAX_WIDTH 360
#define VIDEO_MAX_HEIGHT 202

VideoBubble::VideoBubble(const QString &videoPath, ChatRole role, QWidget *parent)
    : BubbleFrame(role, parent), m_videoPath(videoPath)
{
    // 创建播放器和视频显示控件
    m_mediaPlayer = new QMediaPlayer(this);
    m_videoWidget = new QVideoWidget(this);

    // 设置视频路径
    m_mediaPlayer->setSource(QUrl::fromLocalFile(videoPath));
    m_mediaPlayer->setVideoOutput(m_videoWidget);

    // 设置显示视频的尺寸（保持宽高比例）
    m_videoWidget->resize(VIDEO_MAX_WIDTH, VIDEO_MAX_HEIGHT);
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ⚠️TODO: 设置循环播放
    m_mediaPlayer->setLoops(QMediaPlayer::Infinite); // 无限循环播放

    // 使用布局管理器添加控件
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_videoWidget);

    QWidget *container = new QWidget();
    container->setLayout(layout);

    this->setWidget(container);

    // 设置气泡框的固定大小
    int left_margin = this->layout()->contentsMargins().left();
    int right_margin = this->layout()->contentsMargins().right();
    int v_margin = this->layout()->contentsMargins().bottom();
    setFixedSize(VIDEO_MAX_WIDTH + left_margin + right_margin, VIDEO_MAX_HEIGHT + v_margin * 2);

    // 启动视频播放并延迟暂停
    m_mediaPlayer->play(); // 开始播放

    // 延迟暂停视频
    QTimer::singleShot(2000, this, [this]() {
        m_mediaPlayer->pause(); // 500ms 后暂停视频
        qDebug() << "Video paused after 100ms";
    });

    // connect video play
    connect(this, &VideoBubble::signal_video_play, this, &VideoBubble::slot_video_play);

    // 事件过滤器
    m_videoWidget->installEventFilter(this); // 安装事件过滤器
}

// 事件过滤器实现
bool VideoBubble::eventFilter(QObject *watched, QEvent *event)
{
    // 处理双击事件
    if (watched == m_videoWidget && event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            // 双击左键事件
            qDebug() << "VideoBubble double-clicked, singal to player!";

            // 发送信号供外部处理 TODO:
            emit signal_video_play(m_videoPath);

            // ⚠️如果需要，也可以直接调用本地播放器展示视频
            // QDesktopServices::openUrl(QUrl::fromLocalFile(m_videoPath));  // 外部播放器
            return true;  // 事件被处理
        }
    }

    // 如果事件不是双击，则继续默认处理
    return QWidget::eventFilter(watched, event);
}

void VideoBubble::slot_video_play(const QString &videoPath) {
    // 新开一个ui，ui构造函数开始启动播放器线程
    // TODO:
    // 创建并显示视频播放器窗口
    VideoPlayerWidget *playerWid = new VideoPlayerWidget(this, videoPath);
}







// #include "VideoBubble.h"
// #include <QVBoxLayout>
// #include <QMediaPlayer>
// #include <QVideoWidget>
// #include <QUrl>
// #include <QDesktopServices>
// #include <QEvent>
// #include <QMouseEvent>
// #include <QVideoSink>
// #include <QVideoFrame>
// #define VIDEO_MAX_WIDTH 360
// #define VIDEO_MAX_HEIGHT 202

// VideoBubble::VideoBubble(const QString &videoPath, ChatRole role, QWidget *parent)
//     : BubbleFrame(role, parent), m_videoPath(videoPath)
// {
//     // 创建播放器和控件
//     m_mediaPlayer = new QMediaPlayer(this);
//     m_videoWidget = new QVideoWidget(this);
//     m_thumbnailLabel = new QLabel(this);

//     // 设置播放器和视频路径
//     m_mediaPlayer->setSource(QUrl::fromLocalFile(videoPath));
//     m_mediaPlayer->setVideoOutput(m_videoWidget);

//     // 获取第一帧图像并设置到 QLabel
//     QPixmap firstFrame = grabFirstFrame(videoPath);
//     m_thumbnailLabel->setPixmap(firstFrame.scaled(VIDEO_MAX_WIDTH, VIDEO_MAX_HEIGHT, Qt::KeepAspectRatio));
//     m_thumbnailLabel->setAlignment(Qt::AlignCenter);

//     // 使用布局管理器
//     QVBoxLayout *layout = new QVBoxLayout();
//     layout->setSpacing(0);
//     layout->setContentsMargins(0, 0, 0, 0);
//     layout->addWidget(m_thumbnailLabel);

//     QWidget *container = new QWidget();
//     container->setLayout(layout);

//     this->setWidget(container);

//     // 设置气泡大小
//     setFixedSize(VIDEO_MAX_WIDTH, VIDEO_MAX_HEIGHT);

//     // 安装事件过滤器
//     m_videoWidget->installEventFilter(this);
// }

// // 获取视频的第一帧
// QPixmap VideoBubble::grabFirstFrame(const QString &videoPath)
// {
//     qDebug() << "获取视频缩略图";
//     QMediaPlayer mediaPlayer;
//     QVideoSink videoSink; // 替代 QVideoWidget
//     mediaPlayer.setVideoOutput(&videoSink);
//     mediaPlayer.setSource(QUrl::fromLocalFile(videoPath));

//     QImage thumbnail;
//     QEventLoop loop;

//     // 连接视频帧捕获信号
//     QObject::connect(&videoSink, &QVideoSink::videoFrameChanged, [&videoSink, &thumbnail, &loop]() {
//         if (videoSink.videoFrame().isValid()) {
//             thumbnail = videoSink.videoFrame().toImage(); // 转换成 QImage
//             loop.quit(); // 获取第一帧后退出循环
//         }
//     });

//     // 播放器状态变化
//     QObject::connect(&mediaPlayer, &QMediaPlayer::mediaStatusChanged, [&loop](QMediaPlayer::MediaStatus status) {
//         if (status == QMediaPlayer::InvalidMedia) {
//             qDebug() << "无法加载媒体";
//             loop.quit();
//         }
//     });

//     mediaPlayer.play(); // 开始播放以生成帧
//     loop.exec();

//     mediaPlayer.stop();

//     // 缩放图像
//     if (!thumbnail.isNull()) {
//         qDebug() << "获取视频缩略图成功";
//         return QPixmap::fromImage(thumbnail.scaled(QSize(360, 202), Qt::KeepAspectRatio));
//     } else {
//         qDebug() << "无法获取视频帧";
//         return QPixmap(); // 返回空图像
//     }
// }

// bool VideoBubble::eventFilter(QObject *watched, QEvent *event)
// {
//     if (watched == m_videoWidget && event->type() == QEvent::MouseButtonDblClick) {
//         QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//         if (mouseEvent->button() == Qt::LeftButton) {
//             qDebug() << "VideoBubble double-clicked, singal to player!";

//             // 发出双击信号供外部处理
//             emit videoDoubleClicked(m_videoPath);

//             // 如果需要，也可以直接调用本地播放器展示视频
//             QDesktopServices::openUrl(QUrl::fromLocalFile(m_videoPath));
//             return true;
//         }
//     }
//     return QWidget::eventFilter(watched, event);
// }
