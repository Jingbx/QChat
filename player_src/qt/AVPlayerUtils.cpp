#include "AVPlayerUtils.hpp"

static ConfigUtils config;

QPlayer::QPlayer(QObject *parent) : QObject(parent)
{

}

QPlayer::~QPlayer(){

}

QObject *QPlayer::getMSource() const
{

    return m_mSource;
}

void QPlayer::setMSource(QObject *mSource)
{
    m_mSource = mSource;
}

QString QPlayer::getStrBackgroundPath() const
{
    qDebug() << "QPlayer getStrBackgroundPath " << config.getStrBackgroundPath();
    return config.getStrBackgroundPath().c_str();
}

void QPlayer::setStrBackgroundPath(const QString &fileUrl)
{
    config.setStrBackgroundPath(fileUrl.toStdString());
}


int QPlayer::setPlayerVolume(int volume){
    return config.setPlayerVolume(volume);
}

int QPlayer::getPlayerVolume(){
    return config.getPlayerVolume();
}

