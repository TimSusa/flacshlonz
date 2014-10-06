#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "VideoWidget.hpp"
#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>

QT_BEGIN_NAMESPACE
class QMediaPlayer;
class QVideoWidget;
QT_END_NAMESPACE

class Player : public QWidget
{
    Q_OBJECT

public:
    Player(QWidget *parent = 0);
    ~Player();

signals:
    void fullScreenChanged(bool fullScreen);

public slots:
    void open();
    void previousClicked();
    void seek(int seconds);
    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(int progress);
    void videoAvailableChanged(bool available);
    void displayErrorMessage();
    void addToPlaylist(const QStringList &fileNames);
    void slotPlay();
    void slotStop();
    void slotSetStatusInfo(const QString &info);

private:
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);

    QMediaPlayer *m_MediaPlayer;
    QMediaPlaylist *m_MediaPlayList;
    VideoWidget *m_VideoWidget;
    QString m_TrackInfo;
    QString m_StatusInfo;
    qint64 m_Duration;
};

#endif // PLAYER_HPP
