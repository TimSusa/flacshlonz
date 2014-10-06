//////////////////////////////////////////////////////////////////////
//!\file ThreadDataBaseImport.hpp
//!\brief Connects and imports between to two databases.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef THREADDATABASEIMPORT_HPP
#define THREADDATABASEIMPORT_HPP

#include <QStringList>
#include <QThread>
#include <QObject>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QSqlDatabase>
#include <QtSql>
//#include "DataBase.hpp"
//#include "settings/Persistence.hpp"

class ThreadDataBaseImport : public QThread
{
    Q_OBJECT

public:
    ThreadDataBaseImport(const QString& path, bool importToCollection, QObject *parent = 0);
    void run();

    static const char* s_ConnectionNameImportDb;
    static const char* s_ConnectionNameDefaultDbImport;


public slots:
    void slotStop();
    void slotResetProgressCount();
    void slotCloseDb();
    void slotCloseConnection();

signals:
    void sigImportDbEntryFound( const QString& id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5);
    void sigThreadFinished(bool wasSuccessful);


private:
    QString m_PathToImportDb;
    QSqlDatabase m_DbImport;
    QSqlDatabase m_DbDefault;
    bool m_Stopped;
    int m_ProgressCount;
    QString m_TableNameCollection;
    QString m_TableNameFileIncoming;
   // Persistence m_Persistence;
    bool    m_EnableDbImportToCollection;


    void closeDb();
    void closeConnection();
    void connectToImportDBb();
    void connectToDefaultDb();
    void createFileIncomingTable();
    void createCollectionTable();
    void insertValueToFileIncomingTable(const QString &id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5);
    void insertValueToMusicCollectionTable(const QString &id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5);
};

#endif // THREADDATABASEIMPORT_HPP
