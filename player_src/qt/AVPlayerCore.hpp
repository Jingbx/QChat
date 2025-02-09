#ifndef AVPLAYERCORE_H
#define AVPLAYERCORE_H

#include <QObject>
#include <QImage>
#include <QQuickPaintedItem>
#include <QTime>
#include "AVPlayerUtils.hpp"
#include "../media/MediaCore.hpp"

enum SHOW_TYPE{
    SHOW_TYPE_MEDIAPLAY,
    SHOW_TYPE_RECORD,
};

/* 自定义的 Qt 组件，继承自 QQuickPaintedItem，用于视频播放和展示，允许我们在 QML 中使用并自定义绘制 */
class QPainter;
class QVideoOutput : public QQuickPaintedItem
{
    // 1. Q_OBJECT 宏使得这个类可以使用信号和槽机制
    Q_OBJECT
    Q_PROPERTY(int playState READ getPlayState NOTIFY playStateChanged)

    //  2. Q_PROPERTY 宏声明属性，使得这些属性可以在 QML 中使用
    /* type 属性名称
     * Read+获取对象的函数
     * WRITE+设置对象的函数
     * NOTIFY+属性值发生改变时发出信号函数
    */
    Q_PROPERTY(QImage mImage READ getImage WRITE setImage)
    Q_PROPERTY(QString fileUrl READ getFileUrl WRITE setFileUrl)
    Q_PROPERTY(int mWidth READ getWidth WRITE setWidth)
    Q_PROPERTY(int mHeight READ getHeight WRITE setHeight)
    Q_PROPERTY(int mShowType READ getShowType WRITE setShowType)

    Q_PROPERTY(int totalDuration READ getTotalDuration WRITE setTotalDuration NOTIFY totalDurationChanged)
    Q_PROPERTY(QString currAVTitle READ getCurrAVTitle WRITE setCurrAVTitle NOTIFY updateCurrAVTitleChanged)

    Q_PROPERTY(QString totalDurationStr READ getTotalDurationStr WRITE setTotalDurationStr NOTIFY totalDurationStrChanged)
    Q_PROPERTY(int currPlayTime READ getCurrPlayTime WRITE setCurrPlayTime NOTIFY currPlayTimeChanged)
    Q_PROPERTY(QString currPlayTimeStr READ getCurrPlayTimeStr WRITE setCurrPlayTimeStr NOTIFY currPlayTimeStrChanged)



public:
    QVideoOutput(QQuickItem *parent = 0);
    virtual ~QVideoOutput();

    // paint 函数是 QQuickPaintedItem 的纯虚函数，需要实现以自定义绘制逻辑(抽象基类，没法实例化)
    virtual void paint(QPainter *painter);  // 继承自QQuickPaintedItem的都要实现这个纯虚函数

    /*
    QImage 是 Qt 中用于处理图像的类。它是一个高效的图像表示形式，支持多种图像格式和颜色深度。QImage 提供了丰富的接口，用于加载、保存、访问和操作图像数据。
        主要功能包括：
        图像加载和保存：支持多种常见的图像格式，如 PNG、JPEG、BMP 等。
        像素访问和修改：可以直接访问和修改图像的像素数据。
        图像变换：支持旋转、缩放、裁剪等图像变换操作。
        绘制支持：可以与 QPainter 一起使用，将 QImage 绘制到各种绘制设备（如窗口、小部件、打印机等）上。
    */
    QImage getImage() const;
    void setImage(QImage img);

    QString getFileUrl() const;
    void setFileUrl(QString url);

    int getWidth() const;
    void setWidth(int w);

    int getHeight() const;
    void setHeight(int h);

    int getShowType() const;
    void setShowType(int st);

    int getTotalDuration();
    void setTotalDuration(int totalDuration);

    QString getTotalDurationStr();
    void setTotalDurationStr(QString totalDurationStr);

    //当前当前播放时间
    int getCurrPlayTime();
    void setCurrPlayTime(int currPlayTime);

    QString getCurrPlayTimeStr();
    void setCurrPlayTimeStr(QString currPlayTime);

    QString getCurrAVTitle();
    void setCurrAVTitle(QString currAVTitle);

    // Q_INVOKABLE : 用于标记类中的成员函数，使其可以从 QML 中调用
    // 通常情况下，只有声明为 public slots 的函数和 Q_INVOKABLE 函数可以从 QML 中调用。
    // zoomDraw 函数可以从 QML 中调用，用于调整绘制的缩放比例
    Q_INVOKABLE void zoomDraw(float z = 1.0f);


    static void* playVideoThr(std::string filepath, QVideoOutput *uVO);

    QPlayer *qPlayer;
    // std::shared_ptr<MediaState> ms = nullptr;
    MediaState *ms = nullptr;


/*  在属性值变化时发射，用于通知 QML 层进行相应的更新 ---> NOTOFY
    当一个信号被发射时，与其相关联的槽将被立刻执行，就象一个正常的函数调用一样。
    🍊信号函数不需要显式定义
*/
// 信号是用于对象之间通信的一种机制。一个对象可以发出信号，其他对象可以连接到这个信号并响应它。信号通常用于通知其他对象某些事件的发生。
signals:
    void requestUpdate();
    void totalDurationChanged();
    void totalDurationStrChanged();
    void currPlayTimeChanged();
    void currPlayTimeStrChanged();
    void updateCurrAVTitleChanged();
    void playStateChanged(); // 通知 QML 绑定状态发生变化
    // add
    // void windowSizeChanged(double width, double height);



/* 槽函数 ：
 * 槽函数是可以与信号连接的普通成员函数。槽函数在信号发出时被调用 ---> callback。
 * 槽函数可以是任何访问权限（`public`、`protected` 或 `private`）的成员函数。
 * 提供了各种功能，包括更新背景图像、处理 URL、开始和停止播放、更新播放状态和音量等。 */
public slots:
    void updateBackgroupImage(QString fileUrl);
    // 核心：开启播放线程
    void urlPass(QString url);

    void stopPlay();
    int startPlay();

    int getPlayState();

    void procUpdate();

    void updateVolume(int volume);
    int getVolume();

    void playSeek(int seek_time);

    void playUp();
    void playDown();



private:

public:
    QImage mImage;
    QString fileUrl;
    QString replay_fileUrl;
    int mWidth;
    int mHeight;
    float mZoom;

    //记录当前播放的视频文件的总时长
    int _totalDuration; // s
    QString _totalDurationStr; //xx:xx:xx

    //记录当前当前播放时间 xx:xx:xx
    int _currPlayTime = 0; // s
    QString _currPlayTimeStr; //xx:xx:xx

    //记录当前播放的视频名称
    QString _currAVTitle;

    //播放状态,0:设置背景,1:播放视频,-1:暂停播放
    int status=0;

    //plat thread
    std::thread play_thr;

    //type mode
    int mShowType;

};

#endif // AVPLAYERCORE_H
