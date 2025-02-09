#include "AVPlayerCore.hpp"
#include <QPainter>
#include <QImage>
#include <unistd.h>
#include "../include/common.h"
#include "iostream"
#include "../utils/configutils.h"

/* MediaState 和 QML 的接口类 */

static ConfigUtils config;

QVideoOutput::QVideoOutput(QQuickItem *parent) : QQuickPaintedItem(parent) ,mWidth(0), mHeight(0),mZoom(1.0f)
{
    qDebug()<<__FUNCTION__;

    qPlayer = new QPlayer();
        // 刷新界面
    qDebug() << "QVideoOutput接口界面刷新";
    update();
        // QObject::connect 用于连接信号和槽， QVideoOutput 的 requestUpdate 信号连接到其 procUpdate 槽。当 requestUpdate 信号被发射时，会自动调用 procUpdate 槽函数。
    QObject::connect(this, &QVideoOutput::requestUpdate, this, &QVideoOutput::procUpdate);
}

QVideoOutput::~QVideoOutput()
{
    // if (qPlayer) {
    //     delete qPlayer;
    // }
    // if (ms) {
    //     delete ms;
    // }
}


/* ---------------------------------------Q_INVOKABLE--------------------------------------- */
void QVideoOutput::zoomDraw(float z)
{
    mZoom = z;
    this->update();
}



/* ---------------------------------------slots function--------------------------------------- */
void QVideoOutput::updateBackgroupImage(QString fileUrl)
{
    status = 0;
    setFileUrl(fileUrl);
    this->update();
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
void *QVideoOutput::playVideoThr(std::string filepath, QVideoOutput *uVO){
    // 打开流并设置音量
    if (!uVO) {
        qDebug() << QtFatalMsg << "Invalid QVideoOutput pointer...";
        return nullptr;
    }

    QVideoOutput *qVideoOutput = uVO;
    qDebug() << "filepath : " << QString::fromStdString(filepath);

    // 打开媒体流
    qVideoOutput->ms = new MediaState(MEDIA_TYPE_MEDIA_PLAY, filepath, qVideoOutput);
    if (qVideoOutput->ms && qVideoOutput->ms->fmt_ctx) {
        qDebug() << "------------qtCore : MediaStreamOpen成功------------";
        // 设置音量
        qVideoOutput->ms->sdl_volume = config.getPlayerVolume();
    } else {
        qDebug() << QtFatalMsg << "------------qtCore : MediaStreamOpen失败, ms->fmt不存在------------";
        return 0;
    }
    return 0;
}

void QVideoOutput::urlPass(QString url)
{
    // 开始播放，并创建播放线程

    this->status = 1;

    //通知UI，更新正在播放的视频名字
    this->setCurrAVTitle(url);  // 触发signal
    std::cout << "urlPass doing..." << std::endl;

    pid_t process_id = getpid();
    std::cout << "Process ID (play thread): " << process_id << std::endl;

    this->play_thr = std::thread(playVideoThr,url.toStdString(),(QVideoOutput*)this);   // ⚠️ invoke函数+他所需要的参数
    std::cout << "play thread id: " << this->play_thr.get_id() << std::endl;

    this->play_thr.detach();
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/


void QVideoOutput::stopPlay()
{
    qDebug() <<__FUNCTION__ ;
    //恢复当前播放的媒体文件名字
    if(this->ms == NULL)
    {
        return ;
    }
    this->ms->exit_request = 1;

    //修改播放状态
    this->status = 0;
    this->setCurrAVTitle("");
    this->setCurrPlayTime(0);
    this->setCurrPlayTimeStr("00:00:00");
}


void QVideoOutput::updateVolume(int volume)
{
    // 这两部分都要更新！
    config.setPlayerVolume(volume);

    if(this->ms)
    {
        ms->sdl_volume = volume;
    }
}

int QVideoOutput::getVolume()
{
    qDebug() <<__FUNCTION__ ;
    if(this->ms)
    {
        return ms->sdl_volume;
    }

    return 0;
}
// 指定位置seek
void QVideoOutput::playSeek(int seek_time)
{
    if(this->ms)
    {
        ms->seek_req = 1;
        ms->seek_time = seek_time;
    }
}

// 快进
void QVideoOutput::playUp()
{
    // 从ui到media core
    if(this->ms)
    {
        // ms->seek_req = 2;
        // ms->seek_time = 10;
        playSeek(ms->qVideoOutput->getCurrPlayTime() + 10);
    }
}

// 快退
void QVideoOutput::playDown()
{
    if(this->ms)
    {
        // ms->seek_req = 3;
        // ms->seek_time = 10;
        playSeek(ms->qVideoOutput->getCurrPlayTime() - 10);
    } else {
        qDebug() << __FUNCTION__ << " : playDown出错...\n";
    }
}


int QVideoOutput::startPlay() {
    if (this->ms) {
        // 状态切换
        this->ms->paused = !(this->ms->paused);
        if (this->ms->paused) {
            emit playStateChanged(); // 发出通知信号
            return 2;  // paused = 1 return 2 暂停状态
        } else {
            emit playStateChanged(); // 发出通知信号
            return 1;  // paused = 0 return 1 播放状态
        }
    }
    return 0;          // ms不存在无效状态
}

int QVideoOutput::getPlayState() {
    if (this->ms) {
        if (this->ms->paused) {
            emit playStateChanged(); // 发出通知信号
            return 2;  // 暂停状态
        } else {
            emit playStateChanged(); // 发出通知信号
            return 1;  // 播放状态
        }
    }
    return 0;         // ms不存在无效状态
}


void QVideoOutput::procUpdate()
{
    /* 参数：
     * const QRect &rect：表示需要更新的区域。如果不指定区域，则默认更新整个窗口。
     * 使用方式：
     * 在窗口中某些部分发生变化时，调用update函数来请求重新绘制窗口。
     * update函数会在Qt事件循环结束后自动调用paintEvent函数，进行重绘操作。
    */
    update();
}

/* ---------------------------------------paint--------------------------------------- */
void QVideoOutput::paint(QPainter *painter)
{
    if (painter == nullptr)
    {
        return;
    }

    // 0:设置背景, 1:播放视频
    if (status == 0)
    {
        if (!fileUrl.isEmpty())
        {
            if (!mImage.load(fileUrl))
            {
                qDebug("load image file failed!");
                return;
            }
        }
    }
    else if (status == 1)
    {
        if (mImage.isNull())
        {
            qDebug("image is null!");
            return;
        }
    }

    // 自适应
    QPixmap pixmap = QPixmap::fromImage(mImage);

    double widgetWidth = this->width();  // parent的
    double widgetHeight = this->height();
    double imageWidth = pixmap.width();
    double imageHeight = pixmap.height();

    // 计算图像的缩放比例以保持纵横比
    double widthScale = widgetWidth / imageWidth;
    double heightScale = widgetHeight / imageHeight;
    double scale = qMin(widthScale, heightScale);

    double newImageWidth = imageWidth * scale;
    double newImageHeight = imageHeight * scale;

    // 计算图像在小部件上的位置以居中显示
    double xOffset = (widgetWidth - newImageWidth) / 2;
    double yOffset = (widgetHeight - newImageHeight) / 2;

    QRectF targetRect(xOffset, yOffset, newImageWidth, newImageHeight);
    QRectF sourceRect(0, 0, imageWidth, imageHeight);

    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->drawPixmap(targetRect, pixmap, sourceRect);

    // 传递新窗口大小
    // emit windowSizeChanged(newImageWidth, newImageHeight);
}


/* ---------------------------------------READ & WRITE--------------------------------------- */
QImage QVideoOutput::getImage() const
{
    return mImage;
}

void QVideoOutput::setImage(QImage img)
{
    mImage = img;
    //刷新显示 : 表示“发出”，后面所带的信号以类似广播形式发送出去。感兴趣的接受者会关注这个信号，信号函数无需被定义,moc会为其生成代码
    emit requestUpdate();
}

QString QVideoOutput::getFileUrl() const
{
    return fileUrl;
}

void QVideoOutput::setFileUrl(QString url)
{
    fileUrl = url;
    // replay_fileUrl = fileUrl;
    qPlayer->setStrBackgroundPath(url);
}

int QVideoOutput::getWidth() const
{
    return mWidth;
}

void QVideoOutput::setWidth(int w)
{
    mWidth = w;
}

int QVideoOutput::getHeight() const
{
    return mHeight;
}

void QVideoOutput::setHeight(int h)
{
    mHeight = h;
}

int QVideoOutput::getShowType() const{
    return mShowType;
}

void QVideoOutput::setShowType(int st)
{
    this->mShowType = st;
    // qDebug() 函数将调试信息输出到控制台或调试器控制台，用于开发和调试过程中查看变量值和程序状态。
    // qDebug()类似cout; __FUNCTION__ 是一个预定义的预处理宏，它在被编译时会被替换为当前所在的函数名
    qDebug()<<__FUNCTION__ << "showtype=" << this->mShowType;
}





void QVideoOutput::setTotalDuration(int totalDuration)
{

    if(this->_totalDuration != totalDuration)
    {
        this->_totalDuration = totalDuration;
        emit totalDurationChanged();
    }
}

int QVideoOutput::getTotalDuration()
{
    return this->_totalDuration;
}

void QVideoOutput::setTotalDurationStr(QString totalDurationStr)
{
    this->_totalDurationStr = totalDurationStr;
    emit totalDurationStrChanged();
}

QString QVideoOutput::getTotalDurationStr()
{
    return this->_totalDurationStr;
}





// 加mutex

void QVideoOutput::setCurrPlayTime(int currPlayTime)
{
    if(currPlayTime > this->_currPlayTime)
    {
        this->_currPlayTime = currPlayTime;
        // currPlayTime 频繁更新，没有必要每次都触发信号
        //        emit currPlayTimeChanged();
    }

    if(currPlayTime == 0){
        this->_currPlayTime = currPlayTime;
        emit currPlayTimeChanged();
    }
}

int QVideoOutput::getCurrPlayTime()
{
    return this->_currPlayTime;
}


void QVideoOutput::setCurrPlayTimeStr(QString currPlayTimeStr)
{
    if(this->_currPlayTimeStr != currPlayTimeStr)
    {
        this->_currPlayTimeStr = currPlayTimeStr;
        emit currPlayTimeStrChanged();
        emit currPlayTimeChanged();
    }
}

QString QVideoOutput::getCurrPlayTimeStr()
{
    return this->_currPlayTimeStr;
}

void QVideoOutput::setCurrAVTitle(QString url)
{
    //通知UI，更新正在播放的视频名字
    QStringList tmpList = url.split("/");
    this->_currAVTitle = tmpList[tmpList.length() - 1];
    emit updateCurrAVTitleChanged();
}

QString QVideoOutput::getCurrAVTitle()
{
    return this->_currAVTitle;
}




























