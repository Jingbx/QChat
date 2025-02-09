/*
 * @File        PictureBubble.h
 * @brief       图片气泡框
 *
 * @Author      Jingbx
 * @Data        2024/12/24
 * @History
 */
#ifndef PICTUREBUBBLE_H
#define PICTUREBUBBLE_H

#include "BubbleFrame.h"
#include <QHBoxLayout>
#include <QPixmap>
class PictureBubble : public BubbleFrame
{
    Q_OBJECT
public:
    PictureBubble(const QPixmap &picture, ChatRole role, QWidget *parent = nullptr);
};

#endif // PICTUREBUBBLE_H
