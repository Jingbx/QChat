#include "state_widget.h"
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>
#include <QLabel>
#include <QVBoxLayout>

StateWidget::StateWidget(QWidget *parent) : QWidget(parent),_curstate(ClickLabelState::NORMAL)
{
    setCursor(Qt::PointingHandCursor);
    //添加红点
    AddRedPoint();
}

void StateWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    return;

}

// 处理鼠标点击事件
void StateWidget::mousePressEvent(QMouseEvent* event)  {
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLabelState::SELECTED){
            // qDebug()<<"PressEvent , already to selected press: "<< _selected_press;
            // 再点一次变化
            // _curstate = ClickLabelState::NORMAL;
            // setProperty("state",_normal);
            // repolish(this);
            // update();
            // 应该是点击别的列表他才变化
            //emit clicked();
            // 调用基类的mousePressEvent以保证正常的事件处理
            QWidget::mousePressEvent(event);
            return;
        }

        if(_curstate == ClickLabelState::NORMAL){
            // qDebug()<<"PressEvent , change to selected press: "<< _selected_press;
            _curstate = ClickLabelState::SELECTED;
            setProperty("state",_selected_press);
            repolish(this);
            update();
        }

        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QWidget::mousePressEvent(event);
}

void StateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLabelState::NORMAL){
            //qDebug()<<"ReleaseEvent , change to normal hover: "<< _normal_hover;
            setProperty("state",_normal_hover);
            repolish(this);
            update();

        }else{
            //qDebug()<<"ReleaseEvent , change to select hover: "<< _selected_hover;
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }
        emit clicked();
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QWidget::mousePressEvent(event);
}

// 处理鼠标悬停进入事件
void StateWidget::enterEvent(QEnterEvent * event) {
    // 在这里处理鼠标悬停进入的逻辑
    if(_curstate == ClickLabelState::NORMAL){
         //qDebug()<<"enter , change to normal hover: "<< _normal_hover;
        setProperty("state",_normal_hover);
        repolish(this);
        update();

    }else{
         //qDebug()<<"enter , change to selected hover: "<< _selected_hover;
        setProperty("state",_selected_hover);
        repolish(this);
        update();
    }

    QWidget::enterEvent(event);
}

// 处理鼠标悬停离开事件
void StateWidget::leaveEvent(QEvent* event){
    // 在这里处理鼠标悬停离开的逻辑
    if(_curstate == ClickLabelState::NORMAL){
        // qDebug()<<"leave , change to normal : "<< _normal;
        setProperty("state",_normal);
        repolish(this);
        update();

    }else{
        // qDebug()<<"leave , change to select normal : "<< _selected;
        setProperty("state",_selected);
        repolish(this);
        update();
    }
    QWidget::leaveEvent(event);
}

void StateWidget::SetState(QString normal, QString hover, QString press,
                            QString select, QString select_hover, QString select_press)
{
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;

    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;

    setProperty("state",normal);
    repolish(this);
}

ClickLabelState StateWidget::GetCurState(){
    return _curstate;
}

void StateWidget::ClearState()
{
    _curstate = ClickLabelState::NORMAL;
    setProperty("state",_normal);
    repolish(this);
    update();
}

void StateWidget::SetSelected(bool bselected)
{
    if(bselected){
        _curstate = ClickLabelState::SELECTED;
        setProperty("state",_selected);
        repolish(this);
        update();
        return;
    }

    _curstate = ClickLabelState::NORMAL;
    setProperty("state",_normal);
    repolish(this);
    update();
    return;

}

void StateWidget::AddRedPoint()
{
    //添加红点示意图
    _red_point = new QLabel();
    _red_point->setObjectName("red_point");
    QVBoxLayout* layout2 = new QVBoxLayout;
    _red_point->setAlignment(Qt::AlignCenter);
    layout2->addWidget(_red_point);
    layout2->setContentsMargins(0,0,0,0);
    this->setLayout(layout2);
    _red_point->setVisible(false);
}

void StateWidget::ShowRedPoint(bool show)
{
    _red_point->setVisible(true);
}

void StateWidget::setText(QString s)
{
    return;
}





