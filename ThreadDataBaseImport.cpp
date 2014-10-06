#include "ThreadDataBaseImport.hpp"
#include <QMutex>
//#include "Utils.hpp"



const char* ThreadDataBaseImport::s_ConnectionNameImportDb = "ImportConnection";
const char* ThreadDataBaseImport::s_ConnectionNameDefaultDbImport = "DefaultImport";

ThreadDataBaseImport::ThreadDataBaseImport(const QString& path, bool importToCollection,  QObject *parent):
    QThread(parent),
    m_PathToImportDb(path),
    m_DbImport(QSqlDatabase::addDatabase( DataBase::s_DatabaseDriver, s_ConnectionNameImportDb )),
    m_DbDefault(QSqlDatabase::addDatabase( DataBase::s_DatabaseDriver, s_ConnectionNameDefaultDbImport )),
    m_Stopped(false),
    m_ProgressCount(0),
    m_TableNameCollection(DataBase::s_CollectionTableName),
    m_TableNameFileIncoming(DataBase::s_FileIncomingTableName),
    m_Persistence(),
    m_EnableDbImportToCollection(importToCollection)
{
    connectToImportDBb();
    connectToDefaultDb();

    if ( m_EnableDbImportToCollection )
        createCollectionTable();
    else
        createFileIncomingTable();       
}
void ThreadDataBaseImport::closeDb()
{
    qDebug() << "ThreadDataBaseImport::closeDb()";
    if ( m_DbDefault.isOpen()  )
        m_DbDefault.close();

    if ( m_DbImport.isOpen() )
        m_DbImport.close();
}

void ThreadDataBaseImport::closeConnection()
{
    qDebug() << "ThreadDataBaseImport::closeConnection";
    closeDb();

    m_DbDefault.removeDatabase(s_ConnectionNameDefaultDbImport);
    m_DbImport.removeDatabase(s_ConnectionNameImportDb);
}
void ThreadDataBaseImport::run()
{
    QString tmpStr = m_EnableDbImportToCollection ? "to Collection" : "to incoming";
    qDebug() << "ThreadDataBaseImport::run: " << tmpStr;
    bool wasSuccessful=true;
    if (!m_Stopped)
    {
        QString queryStr ="SELECT * FROM CollectionTracks";// FROM " + path + ".SourceTable";
        QSqlQuery qry(m_DbImport);

        // Select all.
        qry.prepare(queryStr);
        if( !qry.exec() )
            qDebug() << qry.lastError();
        else
        {
            QSqlRecord rec = qry.record();
            int id=0;
            int albumCol = rec.indexOf("albumPath");
            int trackCol = rec.indexOf("fileName");
            int audioMd5Col = rec.indexOf("audioMd5");
            int metaMd5Col = rec.indexOf("metaMd5");
            QString albumN="";
            QString trackN="";
            QString audioMd5N="";
            QString metaMd5N="";


            while(qry.next()&&!m_Stopped)
            {
                QMutex mutex;
                mutex.lock();

                albumN = qry.value(albumCol).toString();
                trackN = qry.value(trackCol).toString();
                audioMd5N = qry.value(audioMd5Col).toString();
                metaMd5N = qry.value(metaMd5Col).toString();
                //qDebug() << "found values" << trackN;
                // insert values to own database
                if (m_EnableDbImportToCollection)
                {
                    insertValueToMusicCollectionTable(QString::number(id++), albumN, trackN, audioMd5N, metaMd5N);
                }
                else
                {
                    insertValueToFileIncomingTable(QString::number(id++), albumN, trackN, audioMd5N, metaMd5N);
                }

                emit sigImportDbEntryFound(QString::number(id++), albumN, trackN, audioMd5N, metaMd5N);
                mutex.unlock();
                //msleep(10);
            }

        }
        m_DbImport.close();
        m_DbDefault.close();
        //m_Db.close();
        //m_ForeignDB.removeDatabase("ForeignConnection");
        //m_Db.removeDatabase("");
    }
    else
    {
        wasSuccessful=false;
    }
    emit sigThreadFinished(wasSuccessful);
}


void ThreadDataBaseImport::slotStop()
{
    qDebug() << "ThreadDataBaseImport::slotStop()";
    m_Stopped = true;
}

void ThreadDataBaseImport::slotResetProgressCount()
{
    qDebug() << "ThreadDataBaseImport::slotResetProgressCount";
    m_ProgressCount=0;
}
/*
void ThreadDataBaseImport::slotEnableDbImportTo(bool toCollection)
{
    qDebug() << "ThreadDataBaseImport::slotEnableDbImportTo " << toCollection;
    m_EnableDbImportTo=toCollection;
}
*/
void ThreadDataBaseImport::slotCloseDb()
{
    closeDb();
}

void ThreadDataBaseImport::slotCloseConnection()
{
    closeConnection();
}

void ThreadDataBaseImport::connectToImportDBb()
{
    qDebug() << "ThreadDataBaseImport::connectToImportDBb";
    if (!m_DbImport.isOpen())
    {
        //m_ForeignDB.addDatabase( "QSQLITE", "ForeignConnection" );
        m_DbImport.setDatabaseName( m_PathToImportDb );
        if( !m_DbImport.open() )
        {
            qDebug() << m_DbImport.lastError();
            qFatal( "Failed to connect to import database." );
        }
        else
        {
            qDebug( "Successfully connected to import database" );
        }
    }
    else
    {
        qDebug() << "Still connected to import database!";
    }
}

void ThreadDataBaseImport::connectToDefaultDb()
{
    qDebug() << "ThreadDataBaseImport::connectToDefaultDb";

    if (!m_DbDefault.isOpen())
    {
        //m_Db.addDatabase( "QSQLITE" );
        m_DbDefault.setDatabaseName( DataBase::s_DatabaseName );
        if( !m_DbDefault.open() )
        {
            qDebug() << "ThreadDataBaseImport::connectToDb " << m_DbDefault.lastError();
            qFatal( "Failed to connect to database." );
        }
        else
        {
            qDebug( "ThreadDataBaseImport: Successfully Connected to database" );
        }
    }
    else
    {
        qDebug() << "ThreadDataBaseImport: Still connected to default database!";
    }
}

void ThreadDataBaseImport::insertValueToFileIncomingTable(const QString &id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5)
{
    //qDebug() << "ThreadDataBaseImport::insertValueToFileIncomingTable id = " << id << " albumPath = " << albumPath << " fileName = " << fileName << " audioMd5 = " << audioMd5 << " metaMd5 = " << metaMd5;
    // create albumname from albumpath
    Utils utl;
    QString albumName = utl.getBaseFromPath(albumPath);

    if (!m_DbDefault.isOpen())
    {
        qDebug() << "try to reopen...";
        if ( !m_DbDefault.open() )
        {
            qDebug() << "ThreadDataBaseImport::insertValueToFileIncomingTable(): Database not open!";
            return;
        }
    }

    QSqlQuery qry(m_DbDefault);

    // Insert Into Table.
    //qry.prepare( "INSERT INTO " + m_TableName + " (id, albumPath, fileName, audioMd5, metaMd5) VALUES ("+id+", '"+albumPath+"', '"+fileName+"', '"+audioMd5+"', '"+metaMd5+"')" );
    qry.prepare( "INSERT INTO " + m_TableNameFileIncoming + " (id, albumPath, albumName, fileName, audioMd5, metaMd5) VALUES (:id, :albumPath, :albumName, :fileName, :audioMd5, :metaMd5)");
    qry.bindValue(":id", id);
    qry.bindValue(":albumPath", albumPath);
    qry.bindValue(":albumName", albumName);
    qry.bindValue(":fileName", fileName);
    qry.bindValue(":audioMd5", audioMd5);
    qry.bindValue(":metaMd5", metaMd5);
    if( !qry.exec() )
        qDebug() << "ThreadDataBaseImport: " << qry.lastError();
    // else
    //    qDebug( "Inserted to FileIncoming-Table!" );
}

void ThreadDataBaseImport::insertValueToMusicCollectionTable(const QString &id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5)
{
    Utils utl;
    QString albumName = utl.getBaseFromPath(albumPath);


    if (!m_DbDefault.isOpen())
    {
        qDebug() << "try to reopen...";
        if ( !m_DbDefault.open() )
        {
            qDebug() << "ThreadDataBaseImport::insertValueToMusicCollectionTable(): Database not open!";
            return;
        }
    }

    QSqlQuery qry(m_DbDefault);

    // Insert Into Table.
    //qry.prepare( "INSERT INTO " + m_TableName + " (id, albumPath, fileName, audioMd5, metaMd5) VALUES ("+id+", '"+albumPath+"', '"+fileName+"', '"+audioMd5+"', '"+metaMd5+"')" );
    qry.prepare( "INSERT INTO " + m_TableNameCollection + " (id, albumPath, albumName, fileName, audioMd5, metaMd5) VALUES (:id, :albumPath, :albumName, :fileName, :audioMd5, :metaMd5)");
    qry.bindValue(":id", id);
    qry.bindValue(":albumPath", albumPath);
    qry.bindValue(":albumName", albumName);
    qry.bindValue(":fileName", fileName);
    qry.bindValue(":audioMd5", audioMd5);
    qry.bindValue(":metaMd5", metaMd5);
    if( !qry.exec() )
        qDebug()  << "ThreadDataBaseImport: insert val to collection " << qry.lastError();
}

void ThreadDataBaseImport::createFileIncomingTable()
{
    qDebug() << "ThreadDataBaseImport::createFileIncomingTable...";

    QSqlQuery qry(m_DbDefault);

    if( !m_DbDefault.open() )
    {
        qDebug() << m_DbDefault.lastError();
        qFatal( "Failed to connect." );
    }
    else
        qry.prepare( "CREATE TABLE IF NOT EXISTS " + m_TableNameFileIncoming +
                     " (id INTEGER, albumPath VARCHAR(200), albumName VARCHAR(100), fileName VARCHAR(100), audioMd5 VARCHAR(33), metaMd5 VARCHAR(33))" );

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
        qDebug()  << "ThreadDataBaseImport: FileIncoming Table OK!";
}

void ThreadDataBaseImport::createCollectionTable()
{
    qDebug() << "ThreadDataBaseImport::createTableCollection";
    //qDebug() << QSqlDatabase::drivers();

    QSqlQuery qry(m_DbDefault);

    if( !m_DbDefault.open() )
    {
        qDebug()  << "ThreadDataBaseImport: " << m_DbDefault.lastError();
        qFatal( "Failed to connect. Database is not open!" );
    }
    else
        qry.prepare( "CREATE TABLE IF NOT EXISTS " + m_TableNameCollection +
                     " (id INTEGER, albumPath VARCHAR(200), albumName VARCHAR(100), fileName VARCHAR(100), audioMd5 VARCHAR(33), metaMd5 VARCHAR(33))" );

    if( !qry.exec() )
        qDebug()  << "ThreadDataBaseImport: " << qry.lastError();
    else
        qDebug() << "Successfully created Collection Table.";
}
