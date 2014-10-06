#include "Player.hpp"
#include <QMediaService>
#include <QMediaPlaylist>
//#include <QVideoProbe>
#include <QMediaMetaData>
#include <QtWidgets>

Player::Player(QWidget *parent)
    : QWidget(parent)
    , m_VideoWidget(0)
{

    m_MediaPlayer = new QMediaPlayer(this);
    // owned by PlaylistModel
    m_MediaPlayList = new QMediaPlaylist();
    m_MediaPlayer->setPlaylist(m_MediaPlayList);

    connect(m_MediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
    connect(m_MediaPlayer, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));
    connect(m_MediaPlayer, SIGNAL(videoAvailableChanged(bool)), this, SLOT(videoAvailableChanged(bool)));
    connect(m_MediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));

    m_VideoWidget = new VideoWidget(this);
    m_MediaPlayer->setVideoOutput(m_VideoWidget);
    //connect(m_MediaPlayer, SIGNAL(destroyed()), this, SLOT(slotStop()));

    m_VideoWidget->setMinimumHeight(480);
    m_VideoWidget->setMinimumWidth(640);

    //connect(m_VideoWidget, SIGNAL(destroyed(()), this, SLOT(slotStop()));
    if (!m_MediaPlayer->isAvailable()) {
        QMessageBox::warning(this, tr("Service not available"),
                             tr("The QMediaPlayer object does not have a valid service.\n"\
                                "Please check the media service plugins are installed."));
    }

    QStringList arguments = qApp->arguments();
    arguments.removeAt(0);
    addToPlaylist(arguments);
}

Player::~Player()
{
    emit this->slotStop();
}

void Player::open()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Files"));
    addToPlaylist(fileNames);
}

void Player::addToPlaylist(const QStringList& fileNames)
{
    foreach (QString const &argument, fileNames)
    {
        QFileInfo fileInfo(argument);
        if (fileInfo.exists())
        {
            QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
            if (fileInfo.suffix().toLower() == QLatin1String("m3u"))
            {
                m_MediaPlayList->load(url);
            }
            else
                m_MediaPlayList->addMedia(url);
        }
        else
        {
            QUrl url(argument);
            if (url.isValid())
            {
                m_MediaPlayList->addMedia(url);
            }
        }
    }
}

void Player::slotPlay()
{
    emit m_MediaPlayer->play();
}

void Player::slotStop()
{
    qDebug() << "Player::slotStop";
    emit m_MediaPlayer->stop();
}

void Player::slotSetStatusInfo(const QString &info)
{
    setStatusInfo(info);
}

void Player::previousClicked()
{
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if(m_MediaPlayer->position() <= 5000)
        m_MediaPlayList->previous();
    else
        m_MediaPlayer->setPosition(0);
}
/*
void Player::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        m_MediaPlayList->setCurrentIndex(index.row());
        m_MediaPlayer->play();
    }
}
*/
void Player::seek(int seconds)
{
    m_MediaPlayer->setPosition(seconds * 1000);
}

void Player::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);
/*
    // handle status message
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Media Stalled"));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
    */
}

void Player::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia ||
            status == QMediaPlayer::BufferingMedia ||
            status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void Player::bufferingProgress(int progress)
{
    setStatusInfo(tr("Buffering %4%").arg(progress));
}

void Player::videoAvailableChanged(bool available)
{
    if (!available)
    {
        m_VideoWidget->setFullScreen(false);
    } else {

    }
}

void Player::setTrackInfo(const QString &info)
{
    m_TrackInfo = info;
    if (!m_StatusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(m_TrackInfo).arg(m_StatusInfo));
    else
        setWindowTitle(m_TrackInfo);
}

void Player::setStatusInfo(const QString &info)
{
    m_StatusInfo = info;
    if (!m_StatusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(m_TrackInfo).arg(m_StatusInfo));
    else
        setWindowTitle(m_TrackInfo);
}

void Player::displayErrorMessage()
{
    setStatusInfo(m_MediaPlayer->errorString());
}
