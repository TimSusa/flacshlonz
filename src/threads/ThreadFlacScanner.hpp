//////////////////////////////////////////////////////////////////////
//!\file ThreadFlacScanner.hpp
//!\brief Searches for flac files in a thread.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef THREADFLACSCANNER_HPP
#define THREADFLACSCANNER_HPP

#include <QStringList>
#include <QThread>
#include <QObject>
#include <QString>
#include <QDirIterator>
#include <QDebug>
//#include "settings/Persistence.hpp"



class ThreadFlacScanner : public QThread
{
    Q_OBJECT

public:
    ThreadFlacScanner(const QDir &dir, QObject *parent = 0);

    void run();
    bool scanIsRunning()const;

public slots:
    void slotStop();
    void slotDeepScanEnabled(bool isEnabled);

signals:
   //void sigAlbumFound(QString);
    void sigAlbumEntryFound(QString, QString, QString, QString, QString);
    void sigFlacSearchFinished();

private:
    QDirIterator m_Iterator;
   // Persistence m_Persistence;
    bool m_Stopped;    
    bool m_DeepScanEnabled;
    bool m_ScanIsRunning;
};


#endif // THREADFLACSCANNER_HPP
