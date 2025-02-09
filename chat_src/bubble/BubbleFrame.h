/*
 * @File        BubbleFrame.h
 * @brief       气泡内容布局基类，分为Text Pic File等子类进行不同布局
 *
 * @Author      Jingbx
 * @Data        2024/12/24
 * @History
 */
#ifndef BUBBLE_H
#define BUBBLE_H

#include <QFrame>
#include "../base/global.h"
#include <QHBoxLayout>
class BubbleFrame : public QFrame
{
    Q_OBJECT
public:
    BubbleFrame(ChatRole role, QWidget *parent = nullptr);
    void setMargin(int margin);
    //inline int margin(){return margin;}
    void setWidget(QWidget *w);
protected:
    void paintEvent(QPaintEvent *e);
private:
    QHBoxLayout *m_pHLayout;
    ChatRole m_role;
     int      m_margin;
};

#endif // BUBBLE_H
