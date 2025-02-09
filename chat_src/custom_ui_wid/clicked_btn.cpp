#include "clicked_btn.h"
#include <QVariant>
#include "../base/global.h"

ClickedBtn::ClickedBtn(QWidget *parent):QPushButton (parent)
{
    setCursor(Qt::PointingHandCursor); // 设置光标为小手
    setFocusPolicy(Qt::NoFocus);        // 设置控件不接收焦点
}

ClickedBtn::~ClickedBtn(){

}

void ClickedBtn::setState(QString normal, QString hover, QString press)
{
    _hover = hover;
    _normal = normal;
    _press = press;
    setProperty("state",normal);
    repolish(this);
    update();
}

void ClickedBtn::enterEvent(QEnterEvent *event)
{
    setProperty("state",_hover);
    repolish(this);
    update();
    QPushButton::enterEvent(event);
}

void ClickedBtn::leaveEvent(QEvent *event)
{
    setProperty("state",_normal);
    repolish(this);
    update();
    QPushButton::leaveEvent(event);
}

void ClickedBtn::mousePressEvent(QMouseEvent *event)
{
    setProperty("state",_press);
    repolish(this);
    update();
    QPushButton::mousePressEvent(event);
}

void ClickedBtn::mouseReleaseEvent(QMouseEvent *event)
{
    setProperty("state",_hover);
    repolish(this);
    update();
    QPushButton::mouseReleaseEvent(event);
}
