#ifndef THREADREMOVEFILES_HPP
#define THREADREMOVEFILES_HPP

#include <QStringList>
#include <QThread>
#include <QObject>
#include <QString>
#include <QDir>
#include <QDebug>


class ThreadRemoveFiles : public QThread
{
    Q_OBJECT

public:
    ThreadRemoveFiles(const QStringList &paths, QObject *parent = 0);
    void run();
    bool rmDir(const QString &dirName);

public slots:
    void slotStopRemoving();
    void slotResetProgressCount();
signals:
    void sigRemoveProgressCount(int cnt, QString path);
    void sigRemoveRunFinished(bool wasSuccessful);

private:
    QStringList m_PathList;
    bool m_Stopped;
    int m_RemoveProgressCount;
};

#endif // THREADREMOVEFILES_HPP
