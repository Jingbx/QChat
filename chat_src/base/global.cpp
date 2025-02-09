#include "global.h"

QString gate_url_prefix = "";
int TCP_HEAD_LEN = static_cast<int>(sizeof(quint16)) * 2;

std::function<void(QWidget *)> repolish = [] (QWidget *w) {
    w->style()->unpolish(w);    // 清理之前由polish函数设置的外观属性
    w->style()->polish(w);
};

std::function<QString(QString)> xorString = [](QString input){
    QString result = input; // 复制原始字符串，以便进行修改
    int length = input.length(); // 获取字符串的长度
    ushort xor_code = length % 255;
    for (int i = 0; i < length; ++i) {
        // 对每个字符进行异或操作
        // 注意：这里假设字符都是ASCII，因此直接转换为QChar
        result[i] = QChar(static_cast<ushort>(input[i].unicode() ^ xor_code));
    }
    return result;
};

std::function<void(QLabel *, QString)> setMovLabel = [](QLabel *label, QString path) {
    QMovie *movie = new QMovie(path); // 加载 GIF 动画
    label->setMovie(movie);  // 将 GIF 动画设置给 QLabel
    movie->start();  // 启动 GIF 动画
    // ui->logo_label->setScaledContents(true);// 如果你希望设置 QLabel 的大小自适应动画内容，可以使用 setScaledContents
};
