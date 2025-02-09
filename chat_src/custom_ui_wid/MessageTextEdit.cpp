#include "MessageTextEdit.h"
#include <QDebug>
#include <QMessageBox>
#include <QVideoWidget>
#include <QDesktopServices>
#include <QTextCharFormat>
#include <QMediaPlayer>
#include <QVideoSink>
#include <QVideoFrame>
MessageTextEdit::MessageTextEdit(QWidget *parent)
    : QTextEdit(parent)
{

    //this->setStyleSheet("border: none;");
    this->setMaximumHeight(60);

//    connect(this,SIGNAL(textChanged()),this,SLOT(textEditChanged()));

}

MessageTextEdit::~MessageTextEdit()
{

}

QVector<MsgInfo> MessageTextEdit::getMsgList()
{
    mGetMsgList.clear();

    QString doc = this->document()->toPlainText();
    QString text="";//存储文本信息
    int indexUrl = 0;
    int count = mMsgList.size();

    for(int index=0; index<doc.size(); index++)
    {
        if(doc[index]==QChar::ObjectReplacementCharacter)
        {
            if(!text.isEmpty())
            {
                QPixmap pix;
                insertMsgList(mGetMsgList,"text",text,pix);
                text.clear();
            }
            while(indexUrl<count)
            {
                MsgInfo msg =  mMsgList[indexUrl];
                if(this->document()->toHtml().contains(msg.content,Qt::CaseSensitive))
                {
                    indexUrl++;
                    mGetMsgList.append(msg);
                    break;
                }
                indexUrl++;
            }
        }
        else
        {
            text.append(doc[index]);
        }
    }
    if(!text.isEmpty())
    {
        QPixmap pix;
        insertMsgList(mGetMsgList,"text",text,pix);
        text.clear();
    }
    mMsgList.clear();
    this->clear();
    return mGetMsgList;
}

void MessageTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->source()==this)
        event->ignore();
    else
        event->accept();
}

void MessageTextEdit::dropEvent(QDropEvent *event)
{
    insertFromMimeData(event->mimeData());
    event->accept();
}

void MessageTextEdit::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) &&
        !(e->modifiers() & Qt::ShiftModifier)) {
        emit send(); // 发出发送信号
        return;      // 事件已处理，直接返回
    }

    QTextEdit::keyPressEvent(e); // 其他键交给父类处理
}

/**
 * @brief 包括图片和视频和文件
 */
void MessageTextEdit::insertFileFromUrl(const QStringList &urls)
{
    if(urls.isEmpty())
        return;

    foreach (QString url, urls){
        if(isImage(url))
                insertImages(url);
        else if (isVideo(url))
                insertVideo(url);
        else
            insertTextFile(url);
    }
}

void MessageTextEdit::insertImages(const QString &url)
{
    QImage image(url);
    if (image.isNull()) {
        qDebug() << "Failed to load image from:" << url;
        return; // 直接返回
    }

    // 按比例缩放图片
    if (image.width() > image.height()) {
        if (image.width() > 120 || image.height() > 80) {
            if (image.width() > image.height()) {
                image = image.scaledToWidth(360, Qt::SmoothTransformation);
            } else {
                image = image.scaledToHeight(202, Qt::SmoothTransformation);
            }
        }
    } else if (image.width() < image.height()) {
        if (image.width() > 80 || image.height() > 120) {
            if (image.width() > image.height()) {
                image = image.scaledToWidth(202, Qt::SmoothTransformation);
            } else {
                image = image.scaledToHeight(360, Qt::SmoothTransformation);
            }
        }
    } else {
        if (image.width() > 200 || image.height() > 200) {
            if (image.width() > image.height()) {
                image = image.scaledToWidth(200, Qt::SmoothTransformation);
            } else {
                image = image.scaledToHeight(200, Qt::SmoothTransformation);
            }
        }
    }

    QTextCursor cursor = this->textCursor();
    cursor.insertImage(image, url);

    // 插入图片消息到消息列表
    insertMsgList(mMsgList, "image", url, QPixmap::fromImage(image));
}

// void MessageTextEdit::insertTextFile(const QString &url)
// {
//     QFileInfo fileInfo(url);
//     if(fileInfo.isDir())
//     {
//         QMessageBox::information(this,"提示","只允许拖拽单个文件!");
//         return;
//     }

//     if(fileInfo.size()>100*1024*1024)
//     {
//         QMessageBox::information(this,"提示","发送的文件大小不能大于100M");
//         return;
//     }

//     QPixmap pix = getFileIconPixmap(url);
//     QTextCursor cursor = this->textCursor();
//     cursor.insertImage(pix.toImage(),url);
//     insertMsgList(mMsgList,"file",url,pix);
// }
void MessageTextEdit::insertTextFile(const QString &url)
{
    QFileInfo fileInfo(url);
    if (fileInfo.isDir()) {
        QMessageBox::information(this, "提示", "只允许拖拽单个文件!");
        return;
    }

    if (fileInfo.size() > 100 * 1024 * 1024) {
        QMessageBox::information(this, "提示", "发送的文件大小不能大于100M");
        return;
    }

    if (isImage(url)) {
        QPixmap pix(url);
        QTextCursor cursor = this->textCursor();
        cursor.insertImage(pix.toImage(), url);
        insertMsgList(mMsgList, "image", url, pix);
    }
    else if (isVideo(url)) {
        insertVideo(url); // 处理视频
    }
    else {
        QPixmap pix = getFileIconPixmap(url); // 普通文件用图标表示
        QTextCursor cursor = this->textCursor();
        cursor.insertImage(pix.toImage(), url);
        insertMsgList(mMsgList, "file", url, pix);
    }
}

bool MessageTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QTextEdit::canInsertFromMimeData(source);
}

void MessageTextEdit::insertFromMimeData(const QMimeData *source)
{
    QStringList urls = getUrl(source->text());

    if(urls.isEmpty())
        return;

    foreach (QString url, urls)
    {
        if(isImage(url))
            insertImages(url);
        else if (isVideo(url))
            insertVideo(url);
        else
            insertTextFile(url);
    }
}

bool MessageTextEdit::isImage(QString url)
{
    QString imageFormat = "bmp,jpg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp,jpeg";
    QStringList imageFormatList = imageFormat.split(",");
    QFileInfo fileInfo(url);
    QString suffix = fileInfo.suffix();
    if(imageFormatList.contains(suffix,Qt::CaseInsensitive)){
        return true;
    }
    return false;
}

void MessageTextEdit::insertMsgList(QVector<MsgInfo> &list, QString flag, QString text, QPixmap pix)
{
    MsgInfo msg;
    msg.msgFlag=flag;
    msg.content = text;
    msg.pixmap = pix;
    list.append(msg);
}

QStringList MessageTextEdit::getUrl(QString text)
{
    QStringList urls;
    if (text.isEmpty()) return urls;

    // 分割文件路径行
    QStringList list = text.split("\n");

    foreach (QString url, list) {
        if (!url.isEmpty()) {
            // 处理不同系统的文件路径
#if defined(Q_OS_WIN)
            // 处理 Windows 系统下可能存在的 file:// 或 file:/// 的情况
            if (url.startsWith("file:///")) {
                url = url.mid(8); // 去掉 "file:///" 前缀
            } else if (url.startsWith("file://")) {
                url = url.mid(7); // 去掉 "file://" 前缀
            }
#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
            // macOS 和 Linux 可能使用 file:// 开头的路径
            if (url.startsWith("file://")) {
                url = url.mid(7); // 去掉 "file://" 前缀
            }
#endif
            urls.append(url);
        }
    }
    return urls;
}

QPixmap MessageTextEdit::getFileIconPixmap(const QString &url)
{
    QFileIconProvider provder;
    QFileInfo fileinfo(url);
    QIcon icon = provder.icon(fileinfo);

    QString strFileSize = getFileSize(fileinfo.size());
    //qDebug() << "FileSize=" << fileinfo.size();

    QFont font(QString("宋体"),10,QFont::Normal,false);
    QFontMetrics fontMetrics(font);
    QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileinfo.fileName());

    QSize FileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);
    int maxWidth = textSize.width() > FileSize.width() ? textSize.width() :FileSize.width();
    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();

    QPainter painter;
   // painter.setRenderHint(QPainter::Antialiasing, true);
    //painter.setFont(font);
    painter.begin(&pix);
    // 文件图标
    QRect rect(0, 0, 50, 50);
    painter.drawPixmap(rect, icon.pixmap(40,40));
    painter.setPen(Qt::black);
    // 文件名称
    QRect rectText(50+10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileinfo.fileName());
    // 文件大小
    QRect rectFile(50+10, textSize.height()+5, FileSize.width(), FileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();
    return pix;
}

QString MessageTextEdit::getFileSize(qint64 size)
{
    QString Unit;
    double num;
    if(size < 1024){
        num = size;
        Unit = "B";
    }
    else if(size < 1024 * 1224){
        num = size / 1024.0;
        Unit = "KB";
    }
    else if(size <  1024 * 1024 * 1024){
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else{
        num = size / 1024.0 / 1024.0/ 1024.0;
        Unit = "GB";
    }
    return QString::number(num,'f',2) + " " + Unit;
}

void MessageTextEdit::textEditChanged()
{
    //qDebug() << "text changed!" << endl;
}


void MessageTextEdit::clearMessages() {
    // 清空 QTextEdit 中的内容
    this->clear();

    // 清空存储的消息列表
    mMsgList.clear();
    mGetMsgList.clear();
}

/**
 * @brief TODO: 视频操作
 */
bool MessageTextEdit::isVideo(QString url)
{
    QString videoFormat = "mov,mp4,flv,ts,avi,mkv,webm";
    QStringList videoFormatList = videoFormat.split(",");
    QFileInfo fileInfo(url);
    QString suffix = fileInfo.suffix();
    if(videoFormatList.contains(suffix,Qt::CaseInsensitive)){
        return true;
    }
    return false;
}



// 插入视频方法
void MessageTextEdit::insertVideo(const QString &url)
{
    qDebug() << "上传视频";
    QFileInfo fileInfo(url);

    // 检查文件是否存在且为有效的视频文件
    if (!fileInfo.exists() || !isVideo(url)) {
        QMessageBox::warning(this, "无效的视频", "文件不是有效的视频格式或不存在！");
        return;
    }

    // 获取视频缩略图
    QPixmap videoThumbnail = getVideoThumbnail(url);

    if (videoThumbnail.isNull()) {
        QMessageBox::warning(this, "获取缩略图失败", "无法提取视频的缩略图！");
        return;
    }

    // 在 QTextEdit 中插入缩略图
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(videoThumbnail.toImage(), url);

    // 将视频路径加入消息列表
    insertMsgList(mMsgList, "video", url, videoThumbnail);

    qDebug() << "Video inserted:" << url;
}

// 获取视频缩略图
QPixmap MessageTextEdit::getVideoThumbnail(const QString &url)
{
    qDebug() << "获取视频缩略图";
    QMediaPlayer mediaPlayer;
    QVideoSink videoSink; // 替代 QVideoWidget
    mediaPlayer.setVideoOutput(&videoSink);
    mediaPlayer.setSource(QUrl::fromLocalFile(url));

    QImage thumbnail;
    QEventLoop loop;

    // 连接视频帧捕获信号
    QObject::connect(&videoSink, &QVideoSink::videoFrameChanged, [&videoSink, &thumbnail, &loop]() {
        if (videoSink.videoFrame().isValid()) {
            thumbnail = videoSink.videoFrame().toImage(); // 转换成 QImage
            loop.quit(); // 获取第一帧后退出循环
        }
    });

    // 播放器状态变化
    QObject::connect(&mediaPlayer, &QMediaPlayer::mediaStatusChanged, [&loop](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::InvalidMedia) {
            qDebug() << "无法加载媒体";
            loop.quit();
        }
    });

    mediaPlayer.play(); // 开始播放以生成帧
    loop.exec();

    mediaPlayer.stop();

    // 缩放图像
    if (!thumbnail.isNull()) {
        qDebug() << "获取视频缩略图成功";
        return QPixmap::fromImage(thumbnail.scaled(QSize(360, 202), Qt::KeepAspectRatio));
    } else {
        qDebug() << "无法获取视频帧";
        return QPixmap(); // 返回空图像
    }
}

// 双击打开视频
void MessageTextEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    QTextCursor cursor = cursorForPosition(event->pos());
    QTextCharFormat charFormat = cursor.charFormat();

    // 确保字符格式是图片格式
    if (charFormat.isImageFormat()) {
        QTextImageFormat imageFormat = charFormat.toImageFormat();
        QString anchor = imageFormat.name();  // 获取图片资源的名称（即文件路径）

        if (!anchor.isEmpty() && isVideo(anchor)) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(anchor));  // 打开视频文件
            return;
        }
    }

    QTextEdit::mouseDoubleClickEvent(event);
}
