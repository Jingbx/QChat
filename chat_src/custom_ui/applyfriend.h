/*
 * @File        applyfriend.h
 * @brief       findSuccess ui 的添加好友按钮跳转界面
 *
 * @Author      Jingbx
 * @Data        2024/12/28
 * @History
 */
#ifndef APPLYFRIEND_H
/*
 * @File        applyfriend.h
 * @brief       添加好友界面由findSuccess跳转 与验证界面类似
 *
 * @Author      Jingbx
 * @Data        2025/01/08
 * @History
 */
#define APPLYFRIEND_H

#include <QDialog>
#include "../custom_ui_wid/clicked_label.h"
#include "friendlabel.h"
#include "../user_manager/user_data.h"
namespace Ui {
class ApplyFriend;
}

class ApplyFriend : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriend(QWidget *parent = nullptr);
    ~ApplyFriend();
    // 初始化默认的标签列表
    void InitTipLbs();
    void AddTipLbs(ClickedLabel*, QPoint cur_point, QPoint &next_point, int text_width, int text_height);
    bool eventFilter(QObject *obj, QEvent *event);
    // 初始化添加者的介绍 和 被添加人的姓名备注
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);

private:
    void resetLabels();
    void addLabel(QString name);

private:
    Ui::ApplyFriend *ui;
    //已经创建好的标签
    QMap<QString, ClickedLabel*>    _add_labels;
    std::vector<QString>            _add_label_keys;
    QPoint                          _label_point;
    std::vector<QString>            _tip_data;
    QPoint                          _tip_cur_point;
    std::shared_ptr<SearchInfo>     _si;
    //用来在输入框显示添加新好友的标签
    QMap<QString, FriendLabel*>     _friend_labels;
    std::vector<QString>            _friend_label_keys;

public slots:
    //显示更多label标签
    void ShowMoreLabel();
    //输入label按下回车触发将标签加入展示栏
    void SlotLabelEnter();
    //点击关闭，移除展示栏好友便签
    void SlotRemoveFriendLabel(QString);
    //通过点击tip实现增加和减少好友便签
    void SlotChangeFriendLabelByTip(QString, ClickLabelState);
    //输入框文本变化显示不同提示
    void SlotLabelTextChange(const QString& text);
    //输入框输入完成
    void SlotLabelEditFinished();
    //输入标签显示提示框，点击提示框内容后添加好友便签
    void SlotAddFirendLabelByClickTip(QString text);
    //处理确认回调
    void SlotApplySure();
    //处理取消回调
    void SlotApplyCancel();
};

#endif // APPLYFRIEND_H
