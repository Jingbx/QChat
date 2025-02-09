/*
 * @File        clicked_label.h
 * @brief       为QLabel添加可点击属性
 *
 * @Author      Jingbx
 * @Data        2024/12/18
 * @History
 */
#ifndef CLICKED_Label_H
#define CLICKED_Label_H

#include <QLabel>
#include <QMouseEvent>
#include "../base/global.h"

class ClickedLabel : public QLabel {
    Q_OBJECT
public:
    ClickedLabel(QWidget *parent);
    ~ClickedLabel();
    // press
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    // hover
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    // setState
    void setState(QString normal="", QString normal_hover="", QString normal_press="",
                  QString selected="", QString selected_hover="", QString selected_press="");
    // get curr state
    ClickLabelState getCurrState();
    bool setCurState(ClickLabelState state);
    void resetNormalState();
private:
    // States
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    QString _selected;
    QString _selected_hover;
    QString _selected_press;

    ClickLabelState _curr_state;

signals:
    void clicked(QString, ClickLabelState);
};

#endif // CLICKED_Label_H
