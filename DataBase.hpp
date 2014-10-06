#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <QWidget>
#include <QtSql>
#include <QString>
#include <QSqlDatabase>
#include <QObject>
#include <QVector>
#include "Utils.hpp"

class DataBase: public QObject, public Utils
{
    Q_OBJECT
public:
    DataBase(QObject *parent = 0 );
    ~DataBase();

    static const char* s_DatabaseName;
    static const char* s_CollectionTableName;
    static const char* s_FileIncomingTableName;
    static const char* s_DatabaseDriver;

    bool isOpen();
    void createFileIncomingTable();
    void createCollectionTable();
    void deleteCollectionTable();
    bool deleteFileIncomingTable();
    void selectTableDiffAudioMd5();
    void selectTableDiffMetaMd5();
    void selectTableDiffFileName();
    void selectTableDiffAlbumName();
    QStringList getDiffList()const;
    void eraseDbPhysically();
    void closeDb();
    void closeConnection();

public slots:
    void slotInsertValueToCollectionTable(const QString& id, const QString &albumPath, const QString &fileName, const QString& audioMd5, const QString& metaMd5);
    void slotInsertValueToFileIncomingTable(const QString& id, const QString &albumPath, const QString &fileName, const QString& audioMd5, const QString& metaMd5);

private:
    QString m_Path;
    QString m_TableNameCollection;
    QString m_TableNameFileIncoming;
    QSqlDatabase m_Db;
    QStringList m_QueryResultsToShow;

    void connectToDefaultDb();

    void insertValueToCollectionTable(const QString& id, const QString &albumPath, const QString &fileName, const QString& audioMd5, const QString& metaMd5);
    void insertValueToFileIncomingTable(const QString& id, const QString &albumPath, const QString &fileName, const QString& audioMd5, const QString& metaMd5);
    void queryAndPush(const QString& queryString);
    void selectValue();
    void selectInOrder();
    void selectInOrderCount();
    void updateValueById();
    void updateValue();
    void deleteValueById();
    void deleteValue();
};

#endif // DATABASE_HPP
