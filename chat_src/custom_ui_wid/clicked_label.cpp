#include "clicked_label.h"


ClickedLabel::ClickedLabel(QWidget *parent) : QLabel(parent),
                                              _curr_state(ClickLabelState::NORMAL) {
    this->setCursor(Qt::PointingHandCursor);
}

ClickedLabel::~ClickedLabel() {
    // qDebug() << "ClickedLabel close";
}

void ClickedLabel::mousePressEvent(QMouseEvent *ev) {
    if (Qt::LeftButton == ev->button()) {
        if (ClickLabelState::NORMAL == _curr_state) {
            // qDebug() << "QLabel has been clicked, state from NORMAL to SELECTED";
            _curr_state = ClickLabelState::SELECTED;
            setProperty("state", _selected_press);      // ⚠️: 通知QSS
            repolish(this);
            update();
        } else {
            // qDebug() << "QLabel has been clicked, state from SELECTED to NORMAL";
            _curr_state = ClickLabelState::NORMAL;
            setProperty("state", _normal_press);      // ⚠️: 通知QSS
            repolish(this);
            update();
        }
        return;
    }
    // Enhanced:

    // 调用基类mousePressEvent确保正常事件的处理
    QLabel::mousePressEvent(ev);
}

void ClickedLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curr_state == ClickLabelState::NORMAL){
            // qDebug()<<"ReleaseEvent , change to normal hover: "<< _normal_hover;
            setProperty("state",_normal_hover);
            repolish(this);
            update();

        }else{
            //  qDebug()<<"ReleaseEvent , change to select hover: "<< _selected_hover;
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }
        emit clicked(this->text(), _curr_state);    // note:
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

void ClickedLabel::enterEvent(QEnterEvent *event) {
    // 只涉及界面的变换
    if (ClickLabelState::NORMAL == _curr_state) {
        // qDebug() << "Enter QLabel, curr state is NORMAL";
        setProperty("state", _normal_hover);
        repolish(this);
        update();
    } else {
        // qDebug() << "Enter QLabel, curr state is SELECTED";
        setProperty("state", _selected_hover);
        repolish(this);
        update();
    }
    QLabel::enterEvent(event);
}

void ClickedLabel::leaveEvent(QEvent *event) {
    // 只涉及界面的变换
    if (ClickLabelState::NORMAL == _curr_state) {
        // qDebug() << "Leave QLabel, curr state is NORMAL";
        setProperty("state", this->_normal);
        repolish(this);
        update();
    } else {
        // qDebug() << "Leave QLabel, curr state is SELECTED";
        setProperty("state", _selected);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}

void ClickedLabel::setState(QString normal, QString normal_hover, QString normal_press,
                            QString selected, QString selected_hover, QString selected_press) {
    _normal = normal;
    _normal_hover = normal_hover;
    _normal_press = normal_press;
    _selected = selected;
    _selected_hover = selected_hover;
    _selected_press = selected_press;
    setProperty("state", _normal);
    repolish(this);
}

ClickLabelState ClickedLabel::getCurrState() {
    return _curr_state;
}

void ClickedLabel::resetNormalState() {
    _curr_state = ClickLabelState::NORMAL;
    setProperty("state", _normal);
    repolish(this);
}

bool ClickedLabel::setCurState(ClickLabelState state) {
    _curr_state = state;
    if (_curr_state == ClickLabelState::NORMAL) {
        setProperty("state", _normal);
        repolish(this);
    }
    else if (_curr_state == ClickLabelState::NORMAL) {
        setProperty("state", _selected);
        repolish(this);
    }

    return true;
}





