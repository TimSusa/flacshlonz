//////////////////////////////////////////////////////////////////////
//!\file ThreadCopyFiles.hpp
//!\brief Handles multi file and folder copy in a thread.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef THREADCOPYFILES_HPP
#define THREADCOPYFILES_HPP

#include <QStringList>
#include <QThread>
#include <QObject>
#include <QString>
#include <QDirIterator>
#include <QDebug>


class ThreadCopyFiles : public QThread
{
    Q_OBJECT

public:
    ThreadCopyFiles(const QStringList &src, const QStringList& dest, QObject *parent = 0);
    void run();
    bool copy(bool isFile, QString source, QString dest);
    bool isCopyStopped();

public slots:
    void slotStopCopy();
    void slotResetCopyProgressCount();

signals:
    void sigCopyProgressCount(int cnt, QString path);
    void sigCopyFinished(bool wasSuccessful);

private:
    QStringList m_SrcPathList;
    QStringList m_DestPathList;
    bool m_Stopped;
    int m_CopyProgressCount;
};

#endif // THREADCOPYFILES_HPP
