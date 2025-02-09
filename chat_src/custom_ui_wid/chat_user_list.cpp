#include "chat_user_list.h"
#include "../custom_ui/chat_user_wid.h"
#include "../user_manager/userinfo_group.h"


ChatUserList::ChatUserList(QWidget *parent) : QListWidget(parent) {
    Q_UNUSED(parent);
    // 关闭默认滚轮 交给eventFilter处理
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->viewport()->installEventFilter(this);     // 多态 安装事件过滤 viewport返回QWidget父类调用子类方法
}

ChatUserList::~ChatUserList() {

}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport()) {
        if (event->type() == QEvent::Enter) {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y();
        int numSteps = numDegrees / 15; // 计算滚动步数

        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        //int pageSize = 10; // 每页加载的联系人数量

        if (maxScrollValue - currentValue <= 0) {
            auto b_loaded = UserinfoGroup::GetInstance()->isLoadChatFin();
            if(b_loaded){
                return true;
            }

            if(_load_pending){
                return true;
            }
            // 滚动到底部，加载新的联系人
            qDebug()<<"load more chat user";
            _load_pending = true;

            QTimer::singleShot(100, [this](){
                _load_pending = false;
                // QCoreApplication::quit(); // 完成后退出应用程序
            });
            //发送信号通知聊天界面加载更多聊天内容
            emit signal_loading_chat_user();
        }

        return true; // 停止事件传递
    }

    // 检查事件是否是鼠标点击事件
    // if (watched == this->viewport() && event->type() == QEvent::MouseButtonPress) {
    //     QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    //     if (mouseEvent->button() == Qt::LeftButton) {
    //         // 获取鼠标点击的位置
    //         QPoint clickPos = mouseEvent->pos();

    //         // 找到被点击的项（假设你使用的是 QListWidget）
    //         QModelIndex index = this->indexAt(clickPos);
    //         if (index.isValid()) {
    //             // 如果列表项是自定义控件（例如 ChatUserWid）
    //             QListWidgetItem* item = this->itemFromIndex(index);
    //             if (item) {
    //                 // 获取 item 中关联的 ChatUserWid 控件
    //                 ChatUserWid *chatUserWidget = qobject_cast<ChatUserWid*>(this->itemWidget(item));
    //                 if (chatUserWidget) {
    //                     // 从ChatuserWid中获取user_name_lb
    //                     auto username = chatUserWidget->getUserNameLb();
    //                     qDebug() << "Chat with: " << username;

    //                     // 传回点击的聊天对象
    //                     // emit signal_switch_chat_page(username); 改
    //                 }
    //             }
    //         }
    //     }
    //     return true; // 停止事件传递
    // }

    return QListWidget::eventFilter(watched, event);
}
