// #include "App.hpp"

// #include <QGuiApplication>
// #include <QQmlApplicationEngine>
// #include <QQuickWindow>
// #include <QFontDatabase>
// #include <QQmlContext>

// #include "../qt/AVPlayerUtils.hpp"
// #include "../qt/AVPlayerCore.hpp"

// int SetFont()
// {
//     //设置字体
//        QString fontsFile = "";
//        fontsFile = "/opt/homebrew/Caskroom/monaco-nerd-fonts/fonts/MonacoNerdFont-Regular.ttf"; //linux
//        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

//        int nIndex = QFontDatabase::addApplicationFont(fontsFile);
//        QString msyh =QFontDatabase::applicationFontFamilies(nIndex).at(0);
//        QFont font(msyh,0);
//        font.setPointSize(14);
//        QGuiApplication::setFont(font);
//        return 0;
// }

// /**
//  * 注册class到qml
//  *
//  * @brief RegisterClass
//  */
// void RegisterClassToQml(){
//     //注册
//     qmlRegisterType<QPlayer>("com.jingbx.QPlayer.QPlayer", 1, 0, "QPlayer");
//     qmlRegisterType<QVideoOutput>("com.jingbx.QPlayer.QPlayer", 1, 0, "QVideoOutput");
//     // qmlRegisterType<AVRecordConnect>("cn.truedei.UPlayer.UPlayer", 1, 0, "AVRecordConnect");
//     // qmlRegisterType<AVRecordPainted>("cn.truedei.UPlayer.UPlayer", 1, 0, "AVRecordPainted");
// }

// /* -----------------------------------Main----------------------------------- */
// int run_main(int argc, char *argv[])
// {
//     // 1.`QGuiApplication` 实例作为应用程序的入口点
//     QGuiApplication app(argc, argv);

//     // 2. 设置应用图标
//     app.setWindowIcon(QIcon(":/images/icons/install.icns"));

//     // 3. set font
//     SetFont();

//     // 4. 注册cpp class to qml
//     RegisterClassToQml();

//     // 5. 加载和编译QML文件， 并实例化为对象树， 并可以与Cpp交互
//     QQmlApplicationEngine engine;

//     //Set the main entrance of qml
//     //engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
//     //engine.load(QUrl(QStringLiteral("qrc:/qml/mediaplay/MediaPlayMainPage.qml")));
//     engine.load(QUrl(QStringLiteral("qrc:/qml/MainPage.qml")));
//     if (engine.rootObjects().isEmpty()) {
//         std::cout << "engin.rootObjects() is empty..." << std::endl;
//         return -1;
//     }
//     std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
//     return app.exec();
// }
