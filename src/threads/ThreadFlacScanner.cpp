#include "ThreadFlacScanner.hpp"
#include "FlacTrack.hpp"
#include <QMutex>
#include "FlacMetaDataManager.hpp"


ThreadFlacScanner::ThreadFlacScanner(const QDir &dir, QObject *parent):
    QThread(parent),
    m_Iterator(dir, QDirIterator::Subdirectories),
    m_Persistence(),
    m_Stopped(false),
    m_DeepScanEnabled(m_Persistence.getEnableDeepScan()),
    m_ScanIsRunning(false)
{
}

void ThreadFlacScanner::run()
{
    m_ScanIsRunning = true;
    qDebug() << "ThreadFlacScan::run";
    int dbcnt=0;
    int fileCnt=0;
    while(!m_Stopped && m_Iterator.hasNext())
    {
        QMutex mutex;
        mutex.lock();
        m_Iterator.next(); // This has to be right here at the beginning of the loop!
        if (m_Iterator.fileInfo().isFile()) // File found.
        {
            QString dirPath = m_Iterator.fileInfo().absolutePath();
            if (m_Iterator.fileInfo().absoluteFilePath().endsWith(".flac")) // File is FLAC.
            {
                QString fp = m_Iterator.filePath();
                QString fn = m_Iterator.fileInfo().completeBaseName()+".flac";
                if (!m_DeepScanEnabled)
                {
                    emit sigAlbumEntryFound(QString::number(++dbcnt), dirPath, fn, "", "");
                    //qDebug() << "QuickScan! " << m_Iterator.fileName();
                }
                else
                {
                    FlacMetadataManager fmm(fp);

                    QString audioMd5 = "";
                    QString trackMd5 = "";
                    if (fmm.isFlacValid()) //(Utils::getUtf8Thing(fp)))
                    {
                        audioMd5 = fmm.getAudioMd5();//
                        trackMd5 = fmm.getTrackMd5By();//
                        emit sigAlbumEntryFound(QString::number(++dbcnt), dirPath, fn, audioMd5, trackMd5);
                    }
                    else
                    {
                        // try to copy and rename, to read out flac.
                        qDebug() << " try to copy, rename, to read out flac and destroy, afterwards.";
                        QString dst = QDir::currentPath() + "/" + QString::number(++fileCnt)+ ".flac";

                        if (QFile::copy(fp, dst))
                        {
                            fmm(dst);
                            if ( fmm.isFlacValid())
                            {
                                audioMd5 = fmm.getAudioMd5();//
                                trackMd5 = fmm.getTrackMd5By();//
                                qDebug() << "OK: " << fn << " audioMd5: " << audioMd5 << " trackMd5: " << trackMd5;
                                emit sigAlbumEntryFound(QString::number(++dbcnt), dirPath, fn, audioMd5, trackMd5);
                                QFile::remove(dst);
                            }
                        }
                        else
                        {
                            qDebug() << "Copy failed!";
                        }

                    }
                    //qDebug() << "DeepScan! " << QString(QFile::encodeName(fp));

                }
            }
        }
        mutex.unlock();
        msleep(10);
    }
    m_ScanIsRunning=false;
    emit sigFlacSearchFinished();
}

bool ThreadFlacScanner::scanIsRunning() const
{
    return m_ScanIsRunning;
}

void ThreadFlacScanner::slotStop()
{
    m_Stopped = true;
}

void ThreadFlacScanner::slotDeepScanEnabled(bool isEnabled)
{
    qDebug() << "ThreadFlacScanner::slotDeepScanEnabled: " << isEnabled;
    QString msg = isEnabled ? "Deep-Scan enabled" : "Quick-Scan enabled";
    //emit sigAlbumFound(msg);
    m_DeepScanEnabled=isEnabled;
}

