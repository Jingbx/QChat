#ifndef AVPLAYERUTILS_H
#define AVPLAYERUTILS_H

#include <QMainWindow>
#include <QObject>
#include <QQuickItem>

#include <QTime>
//#include "main.h"

#include "../utils/configutils.h"


class QPlayer : public QObject
{
    Q_OBJECT
public:
    explicit QPlayer(QObject *parent = nullptr);
    ~QPlayer();

private:
    QObject*                                    m_mSource{nullptr};

public:
    QObject *source() const;
    QObject *getMSource() const;
    void setMSource(QObject *mSource);



    void setStrBackgroundPath(const QString &fileUrl);
    int setPlayerVolume(int volume);

public slots:
    QString getStrBackgroundPath() const;
    int getPlayerVolume();
};

#endif // AVPLAYERUTILS_H
