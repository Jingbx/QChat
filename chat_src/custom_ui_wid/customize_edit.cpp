#include "customize_edit.h"


CustomizeEdit::CustomizeEdit(QWidget *parent) : QLineEdit(parent)  {
    _max_len = 0;
    // 连接失去焦点事件
    connect(this, &QLineEdit::textChanged, this, &CustomizeEdit::limitTextLenght);
}

CustomizeEdit::~CustomizeEdit() {

}

void CustomizeEdit::setMaxLenght(int maxLen) {
    _max_len = maxLen;
}

void CustomizeEdit::focusOutEvent(QFocusEvent *fe) {
    // 确保父类事件执行
    QLineEdit::focusOutEvent(fe);
    emit signal_focus_out();
}

void CustomizeEdit::limitTextLenght(QString text) {
    if (_max_len <= 0) {
        return;     // 不限制
    }
    QByteArray byteArray = text.toUtf8();
    if (byteArray.size() >= _max_len) {
        byteArray = byteArray.left(_max_len);   // 截断
        this->setText(QString::fromUtf8(byteArray));
    }
}

