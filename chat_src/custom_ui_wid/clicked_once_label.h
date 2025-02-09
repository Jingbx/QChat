/*
 * @File        clicked_once_label.h
 * @brief       重写鼠标释放，释放后发出clicked()传递输入框QLabel中的文字
 *
 * @Author      Jingbx
 * @Data        2024/12/28
 * @History
 */

#ifndef CLICKEDONCELABEL_H
#define CLICKEDONCELABEL_H
#include <QLabel>
#include <QMouseEvent>

class ClickedOnceLabel:public QLabel
{
    Q_OBJECT
public:
    ClickedOnceLabel(QWidget *parent=nullptr);
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;

signals:
    void clicked(QString );
};

#endif // CLICKEDONCELABEL_H
