/*
 * @File        customize_edit.h
 * @brief       搜索提示框提升
 *
 * @Author      Jingbx
 * @Data        2024/12/23
 * @History
 */
#ifndef CUSTOMIZE_EDIT_H
#define CUSTOMIZE_EDIT_H

#include <QLineEdit>

class CustomizeEdit : public QLineEdit {
    Q_OBJECT
public:
    CustomizeEdit(QWidget *parent = nullptr);
    ~CustomizeEdit();
    void setMaxLenght(int maxLen);
protected:
    void focusOutEvent(QFocusEvent *) override;
private:
    void limitTextLenght(QString text);
private:
    int _max_len;
signals:
    void signal_focus_out();
};

#endif // CUSTOMIZE_EDIT_H
