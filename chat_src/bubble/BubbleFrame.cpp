#include "BubbleFrame.h"
#include <QPainter>
#include <QDebug>
const int WIDTH_SANJIAO  = 8;  //三角宽
BubbleFrame::BubbleFrame(ChatRole role, QWidget *parent)
    :QFrame(parent)
    ,m_role(role)
    ,m_margin(3)
{
    m_pHLayout = new QHBoxLayout();
    if(m_role == ChatRole::Self)
        m_pHLayout->setContentsMargins(m_margin, m_margin, WIDTH_SANJIAO + m_margin, m_margin);
    else
        m_pHLayout->setContentsMargins(WIDTH_SANJIAO + m_margin, m_margin, m_margin, m_margin);

    this->setLayout(m_pHLayout);
}

void BubbleFrame::setMargin(int margin)
{
    Q_UNUSED(margin);
    //m_margin = margin;
}

void BubbleFrame::setWidget(QWidget *w)
{
    if(m_pHLayout->count() > 0)
        return ;
    else{
        m_pHLayout->addWidget(w);
    }
}

void BubbleFrame::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    // 区分聊天角色
    if (m_role == ChatRole::Other)
    {
        // 对方消息：白色气泡 + 黑色文字
        QColor bk_color(Qt::black); // 气泡颜色
        painter.setBrush(QBrush(bk_color));
        QRect bk_rect = QRect(WIDTH_SANJIAO, 0, this->width() - WIDTH_SANJIAO, this->height());
        painter.drawRoundedRect(bk_rect, 5, 5);

        // 画三角形
        QPointF points[3] = {
            QPointF(bk_rect.x(), 12),
            QPointF(bk_rect.x(), 10 + WIDTH_SANJIAO + 2),
            QPointF(bk_rect.x() - WIDTH_SANJIAO, 10 + WIDTH_SANJIAO - WIDTH_SANJIAO / 2),
        };
        painter.drawPolygon(points, 3);
    }
    else
    {
        // 自己消息：蓝色气泡 + 黑色文字
        QColor bk_color(85, 140, 255); // 气泡颜色
        painter.setBrush(QBrush(bk_color));

        QRect bk_rect = QRect(0, 0, this->width() - WIDTH_SANJIAO, this->height());
        painter.drawRoundedRect(bk_rect, 5, 5);

        // 画三角形
        QPointF points[3] = {
            QPointF(bk_rect.x() + bk_rect.width(), 12),
            QPointF(bk_rect.x() + bk_rect.width(), 12 + WIDTH_SANJIAO + 2),
            QPointF(bk_rect.x() + bk_rect.width() + WIDTH_SANJIAO, 10 + WIDTH_SANJIAO - WIDTH_SANJIAO / 2),
        };
        painter.drawPolygon(points, 3);
    }

    // 设置文字颜色（这里假设通过 QTextLayout 或类似机制绘制文字）
    QPainter textPainter(this);
    textPainter.setPen(QColor(0, 0, 0)); // 黑色文字
    // 此处需确保文字的绘制逻辑匹配气泡大小及内容

    return QFrame::paintEvent(e); // 调用父类的 paintEvent 保持事件链
}
