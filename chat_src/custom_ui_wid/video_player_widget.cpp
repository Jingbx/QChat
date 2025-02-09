#include "video_player_widget.h"
#include <QQmlApplicationEngine>  // 替换 QQuickView 为 QQmlApplicationEngine
#include <QGuiApplication>
#include <QQmlContext>
#include <QFontDatabase>
#include <QQuickWindow>
#include <QQuickItem>

#include "../../player_src/qt/AVPlayerUtils.hpp"
#include "../../player_src/qt/AVPlayerCore.hpp"



int SetFont()
{
    //设置字体
    QString fontsFile = "";
    fontsFile = "/opt/homebrew/Caskroom/monaco-nerd-fonts/fonts/MonacoNerdFont-Regular.ttf"; //linux
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    int nIndex = QFontDatabase::addApplicationFont(fontsFile);
    QString msyh =QFontDatabase::applicationFontFamilies(nIndex).at(0);
    QFont font(msyh,0);
    font.setPointSize(14);
    QGuiApplication::setFont(font);
    return 0;
}



void RegisterClassToQml(){
    // 注册 QML 类型
    qmlRegisterType<QPlayer>("com.jingbx.QPlayer.QPlayer", 1, 0, "QPlayer");
    qmlRegisterType<QVideoOutput>("com.jingbx.QPlayer.QPlayer", 1, 0, "QVideoOutput");
}

VideoPlayerWidget::VideoPlayerWidget(QObject *parent)
    : QObject(parent), view(new QQuickView())
{
    // 设置窗口属性
    SetFont();
    RegisterClassToQml();  // 注册 QML 类型
    view->setTitle("Video Player");
    view->resize(1000, 850);

    // 加载 QML 文件
    view->setSource(QUrl::fromLocalFile(":/qml/MainPage.qml"));
    if (view->status() != QQuickView::Ready) {
        qDebug() << "QML file not loaded correctly, status:" << view->status();
    }
    // 让 QML 窗口作为独立的窗口显示
    view->show();

    // 连接View销毁信号，确保播放器后台实例被销毁
    connect(view, &QQuickView::closing, this, &VideoPlayerWidget::stopQVideoOutput);
}

VideoPlayerWidget::VideoPlayerWidget(QObject *parent, const QString &videoPath)
    : QObject(parent), view(new QQuickView())
{
    // 设置窗口属性
    SetFont();
    RegisterClassToQml();  // 注册 QML 类型
    view->setTitle("Video Player");
    view->resize(1000, 850);

    // ⚠️在加载QML文件之前设置文件路径，不然显示无此变量
    qDebug() << "a" << videoPath;
    this->setVideoSource(videoPath);

    // 加载 QML 文件
    view->setSource(QUrl::fromLocalFile(":/qml/MainPage.qml"));
    if (view->status() != QQuickView::Ready) {
        qDebug() << "QML file not loaded correctly, status:" << view->status();
    }

    // 让 QML 窗口作为独立的窗口显示
    view->show();
    // 连接View销毁信号，确保播放器后台实例被销毁
    connect(view, &QQuickView::closing, this, &VideoPlayerWidget::stopQVideoOutput);
}

void VideoPlayerWidget::stopQVideoOutput()
{
    // 获取 QML 根对象
    QObject *rootObject = view->rootObject();
    QObjectList children = rootObject->children();
    for (QObject *child : children) {
        qDebug() << "Child object:" << child->objectName();
    }
    if (rootObject) {
        QObject *videoOutput = rootObject->findChild<QObject*>("qVideoOutput");
        if (videoOutput) {
            qDebug() << "ok";
            QMetaObject::invokeMethod(videoOutput, "stopPlay");
        } else {
            qDebug() << "fall";
        }
    }
}

VideoPlayerWidget::~VideoPlayerWidget()
{
    qDebug() << "VideoPlayerWidget析构";
    if (view) {
        delete view;
    }
}

void VideoPlayerWidget::setVideoSource(const QString &videoPath)
{
    // 获取 QML 视图中的根项，设置视频源 通过QQmlContext从cpp中注册qml的全局属性
    QQmlContext *context = view->engine()->rootContext();
    context->setContextProperty("videoPath", videoPath);  // 将视频路径传递给 QML
}
