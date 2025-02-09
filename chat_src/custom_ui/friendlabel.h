/*
 * @File        friendlabel.h
 * @brief       联系人列表中的label ui
 *
 * @Author      Jingbx
 * @Data        2024/12/31
 * @History
 */
#ifndef FRIENDLABEL_H
#define FRIENDLABEL_H

#include <QFrame>
#include <QString>
#include "../custom_ui_wid/clicked_label.h"

namespace Ui {
class FriendLabel;
}

class FriendLabel : public QFrame
{
    Q_OBJECT

public:
    explicit FriendLabel(QWidget *parent = nullptr);
    ~FriendLabel();
    void SetText(QString text);
    int Width();
    int Height();
    QString Text();
private:
    Ui::FriendLabel *ui;
    QString _text;
    int _width;
    int _height;
public slots:
    void slot_close();
signals:
    void sig_close(QString);
};

#endif // FRIENDLABEL_H
