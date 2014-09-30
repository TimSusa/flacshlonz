#include "ThreadRemoveFiles.hpp"
#include <QMutex>

ThreadRemoveFiles::ThreadRemoveFiles(const QStringList &paths, QObject *parent):
    QThread(parent),
    m_PathList(paths),
    m_Stopped(false),
    m_RemoveProgressCount(0)
{}

void ThreadRemoveFiles::run()
{
    qDebug() << "ThreadRemoveFiles::run";
    bool wasSuccessful=true;
    if (!m_Stopped)
    {
        foreach(const QString dirName, m_PathList)
        {
            QMutex mutex;
            mutex.lock();
            if (!m_Stopped)
            {
                QFileInfo fi(dirName);
                if ( fi.isFile() )
                {
                    wasSuccessful = QFile::remove(dirName);
                    if (wasSuccessful)
                    {
                        emit sigRemoveProgressCount(++m_RemoveProgressCount, dirName);
                    }
                    else
                    {
                        emit sigRemoveProgressCount(-1, dirName);
                    }

                }
                else if ( fi.isDir() )
                {
                    if (!rmDir(dirName))
                    {
                        wasSuccessful=false;
                    }
                }
                else
                {
                    qDebug() << "SomeThing went wrong here!";
                }
            }
            else
            {
                wasSuccessful=false;
            }
            mutex.unlock();
            msleep(10);
        }
        emit sigRemoveRunFinished(wasSuccessful);
    }
    else
    {
        qDebug() << "...could not be ran, because it is stopped!";
    }
}


void ThreadRemoveFiles::slotStopRemoving()
{
    qDebug() << "ThreadRemoveFiles::slotStop()";
    m_Stopped = true;
}

void ThreadRemoveFiles::slotResetProgressCount()
{
    qDebug() << "ThreadRemoveFiles::slotResetProgressCount";
    m_RemoveProgressCount=0;
}


bool ThreadRemoveFiles::rmDir(const QString &dirName)
{
    qDebug() << "ThreadRemoveFiles::rmDir: " << dirName;
    QDir dir(dirName);
    bool result = dir.exists(dirName);
    if (result)
    {
        // Remove folder content.
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if (info.isDir())
            {
                result = rmDir(info.absoluteFilePath());
                if (result)
                {
                    //qDebug() << "rmDir-Counter: " << ++m_RemoveProgressCount;
                    emit sigRemoveProgressCount(++m_RemoveProgressCount, info.absoluteFilePath());
                }
                else
                {
                    qDebug() << "Could not Remove: " << info.absoluteFilePath();
                    emit sigRemoveProgressCount(-1, info.absoluteFilePath());
                }
            }
            else
            {
                result = QFile::remove(info.absoluteFilePath());
                if (result)
                {
                    //qDebug() << "rmDir-Counter: " << ++m_RemoveProgressCount;
                    emit sigRemoveProgressCount(++m_RemoveProgressCount,info.absoluteFilePath());
                }
                else
                {
                    emit sigRemoveProgressCount(-1, info.absoluteFilePath());
                    qDebug() << "Could not Remove: " << info.absoluteFilePath();
                }
            }
        }
        // Remove folder.
        result = dir.rmdir(dirName);
        if (!result)
        {
            qDebug() << "Error! Could not remove directoy: " << dirName;
            emit sigRemoveProgressCount(-1, dirName);
        }
        else
        {
            emit sigRemoveProgressCount(++m_RemoveProgressCount, dirName);
        }
    }
    else
    {
        qDebug() << "Directoy: " << dirName << ". Does not exist!";
        emit sigRemoveProgressCount(-1, dirName);
    }

    return result;
}
