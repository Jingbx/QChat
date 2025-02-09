#include "chat_src/custom_ui/mainwindow.h"
#include "chat_src/custom_ui_wid/video_player_widget.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 解析qss config
    QFile style_qss(":/style/style_sheet.qss");
    if (style_qss.open(QFile::ReadOnly)) {
        qDebug("Open style sheet success");
        QString style = QLatin1String(style_qss.readAll());
        a.setStyleSheet(style);
        style_qss.close();
    } else {
        qDebug("Open style sheet error");
    }
    // 加载config.ini配置文件
    QString conf_path = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()
                                                 + QDir::separator()
                                                 + "/config.ini");
    if (!QFile::exists(conf_path)) {
        qDebug() << "Config file not found at:" << conf_path;
    } else {
        qDebug() << "Config file found at:" << conf_path;
    }

    QSettings settings(conf_path, QSettings::IniFormat);
    // 配置验证码服务前缀
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://" + gate_host + ":" + gate_port;
    qDebug() << gate_url_prefix;

    // 视频


    MainWindow w;
    w.show();
    return a.exec();
}
