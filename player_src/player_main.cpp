// #include <QGuiApplication>
// #include <QQmlApplicationEngine>
// #include <QIcon>

// int main(int argc, char *argv[]) {
//     QGuiApplication app(argc, argv);

//     // 设置应用程序图标
//     app.setWindowIcon(QIcon(":/images/icons/install.icns"));

//     QQmlApplicationEngine engine;
//     const QUrl url(QStringLiteral("qrc:qml/MainPage.qml"));
//     QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
//         &app, [url](QObject *obj, const QUrl &objUrl) {
//             if (!obj && url == objUrl)
//                 QCoreApplication::exit(-1);
//         }, Qt::QueuedConnection);
//     engine.load(url);

//     return app.exec();
// }


#include "qt/App.hpp"
#include <iostream>
#include <stdio.h>

int main(int argc, char *argv[])
{
    //printf log
    std::cout << "开始" << std::endl;
    setvbuf(stdout, 0, _IONBF, 0);

    return run_main(argc, argv);
}
