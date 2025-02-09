/*
 * @File        state_label.h
 * @brief       侧边栏头像框的提升类
 *
 * @Author      Jingbx
 * @Data        2024/12/25
 * @History
 */
#ifndef STATE_LABEL_H
#define STATE_LABEL_H
#include <QLabel>
#include "../base/global.h"
#include <QMouseEvent>

class StateLabel : public QLabel
{
    Q_OBJECT
public:
    StateLabel(QWidget* parent = nullptr);
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;
    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");

    ClickLabelState GetCurState();
    void ClearState();

    void SetSelected(bool bselected);
protected:

private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;

    QString _selected;
    QString _selected_hover;
    QString _selected_press;

    ClickLabelState _curstate;

signals:
    void clicked(void);
};

#endif // STATE_LABEL_H
