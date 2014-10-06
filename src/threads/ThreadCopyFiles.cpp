#include "ThreadCopyFiles.hpp"
#include <QMutex>

ThreadCopyFiles::ThreadCopyFiles(const QStringList &src, const QStringList& dest, QObject *parent):
    QThread(parent),
    m_SrcPathList(src),
    m_DestPathList(dest),
    m_Stopped(false),
    m_CopyProgressCount(0)
{}

void ThreadCopyFiles::run()
{

    qDebug() << "ThreadCopyFiles::run";
    bool wasSuccessful=true;
    for ( int i =0;( (i< m_SrcPathList.size() ) && (!m_Stopped)); ++i )
    {
        QMutex mutex;
        mutex.lock();
        QFileInfo fi(m_SrcPathList[i]);
        if (fi.exists())
        {
            if ( fi.isFile() )
            {
                wasSuccessful=copy( true, m_SrcPathList[i], m_DestPathList[i]);
            }
            else if ( fi.isDir() )
            {
                wasSuccessful=copy( false, m_SrcPathList[i], m_DestPathList[i]);
            }
            else
            {
                qDebug() << "Nothing was copied, because fileinfo was invalid";
            }
        }
        else
        {
            qDebug() << "Source: " << fi.absoluteFilePath()  <<  " does not exist!";
            wasSuccessful = false;
        }
        mutex.unlock();
        msleep(10);
    }
    emit sigCopyFinished(wasSuccessful);
}


void ThreadCopyFiles::slotStopCopy()
{
    qDebug() << "ThreadCopyFiles::slotStop()";
    m_Stopped = true;
}

void ThreadCopyFiles::slotResetCopyProgressCount()
{
    qDebug() << "ThreadCopyFiles::slotResetProgressCount";
    m_CopyProgressCount=0;
}

bool ThreadCopyFiles::copy(bool isFile, QString source, QString dest)
{
    qDebug() << "ThreadCopyFiles::copy";
    bool result = false;
    if (m_Stopped)
    {
        qDebug() << "Copy stopped!";
        result = false;
        emit sigCopyProgressCount(-1, dest);
    }
    else if ( source.isEmpty() || dest.isEmpty() )
    {
        qDebug() << "Empty file or folder name!";
        emit sigCopyProgressCount(-1, "empty file or folder name");
        result = false;
    }
    else
    {
        // Copy file or folder.
        if (isFile)
        {
            if (QFile(dest).exists())
            {
                if (!QFile::remove(dest))
                {
                    qDebug() << "remove went wrong";
                    // remove went wrong
                    emit sigCopyProgressCount(-1, dest);
                    result = false;
                }
            }
            result = QFile::copy(source, dest);
            if ( result )
            {
                emit sigCopyProgressCount(++m_CopyProgressCount, dest);
            }
            else
            {
                qDebug() << "Copy went wrong!";
                emit sigCopyProgressCount(-1, dest);
            }
        }
        else
        {
            // Folder copy.
            qDebug() << "Folder copy";
            QDir destDir(dest);
            bool destExists=destDir.exists();

            // TODO: Handle if folder exists.
            if (destExists)
            {
                qDebug() << "Destination folder already exists.. please handle this issue!";
                result = false;
            }
            else
            {
                result = destDir.mkdir(dest);
                if (!result)
                {
                    qDebug() << "Destination Folder could not be created!";
                    emit sigCopyProgressCount(-1, dest);
                }
                else
                {
                    // Copy stuff
                    QDir sourceDir(source);
                    QStringList filesList = sourceDir.entryList(QDir::Files);
                    QString fileName;
                    foreach(fileName, filesList)
                    {
                        if ( !m_Stopped )
                        {
                            result = QFile::copy(source + "//" + fileName, dest + "//" + fileName);
                            if ( result )
                            {
                                //qDebug() << "copyoverwrite progress: " << m_FilesToCopyCount << " size= " << filesList.size();
                                emit sigCopyProgressCount(++m_CopyProgressCount, fileName);
                            }
                            else
                            {
                                qDebug() << "Copy of " << fileName << " failed!";
                                emit sigCopyProgressCount(-1, fileName);
                            }
                        }
                        else
                        {
                            result = false;
                            emit sigCopyProgressCount(-1, fileName);
                            qDebug() << "Copy stopped!";
                        }
                    }
                }
            }

        }
    }
    return result;
}

bool ThreadCopyFiles::isCopyStopped()
{
    return m_Stopped;
}
