/*
 * @File        findsuccessdlg.h
 * @brief       成功查找到该用户，弹出dialog, 可通过TCP发送好友请求
 *              ⚠️是QDialog的子类和Faild 都是为了多态可以赋值给父类对象
 *
 * @Author      Jingbx
 * @Data        2024/12/26
 * @History
 */
#ifndef FINDSUCCESSDLG_H
#define FINDSUCCESSDLG_H

#include <QDialog>
#include <memory>
#include "../user_manager/user_data.h"

namespace Ui {
class FindSuccessDlg;
}

class FindSuccessDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindSuccessDlg(QWidget *parent = nullptr);
    ~FindSuccessDlg();
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);

private slots:
    void on_add_friend_btn_clicked();

    void on_close_add_friend_btn_clicked();

private:
    Ui::FindSuccessDlg *ui;
    QWidget * _parent;              // 用于透传给父类信息
    std::shared_ptr<SearchInfo> _si;
};

#endif // FINDSUCCESSDLG_H
