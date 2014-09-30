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
#include "DataBase.hpp"
#include "Persistence.hpp"

class ThreadDataBaseImport : public QThread
{
    Q_OBJECT

public:
    ThreadDataBaseImport(const QString& path, QObject *parent = 0);
    void run();

    static const char* s_ForeignDbConnectionName;
    static const char* s_DefaultDbImportConnectionName;

    void closeDb();
    void closeConnection();

public slots:
    void slotStop();
    void slotResetProgressCount();
    void slotEnableDbImportTo(bool toCollection);
signals:
    void sigForeignDbEntryFound( const QString& id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5);
    void sigThreadFinished(bool wasSuccessful);


private:
    QString m_Path;
    QSqlDatabase m_ForeignDB;
    QSqlDatabase m_Db;
    bool m_Stopped;
    int m_ProgressCount;
    QString m_TableNameCollection;
    QString m_TableNameFileIncoming;
    Persistence m_Persistence;
    bool    m_EnableDbImportTo;


    void connectToForeignDB();
    void connectToDb();
    void createFileIncomingTable();
    void createCollectionTable();
    void insertValueToFileIncomingTable(const QString &id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5);
    void insertValueToMusicCollectionTable(const QString &id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5);
};

#endif // THREADDATABASEIMPORT_HPP
