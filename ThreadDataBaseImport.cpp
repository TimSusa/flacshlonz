#include "ThreadDataBaseImport.hpp"
#include <QMutex>
//#include "Utils.hpp"



const char* ThreadDataBaseImport::s_ForeignDbConnectionName = "ForeignConnection";
const char* ThreadDataBaseImport::s_DefaultDbImportConnectionName = "DefaultImport";

ThreadDataBaseImport::ThreadDataBaseImport(const QString& path,  QObject *parent):
    QThread(parent),
    m_Path(path),
    m_ForeignDB(QSqlDatabase::addDatabase( DataBase::s_DatabaseDriver, s_ForeignDbConnectionName )),
    m_Db(QSqlDatabase::addDatabase( DataBase::s_DatabaseDriver, s_DefaultDbImportConnectionName )),
    m_Stopped(false),
    m_ProgressCount(0),
    m_TableNameCollection(DataBase::s_CollectionTableName),
    m_TableNameFileIncoming(DataBase::s_FileIncomingTableName),
    m_Persistence(),
    m_EnableDbImportTo(m_Persistence.getEnableDbImportTo())
{
    connectToForeignDB();
    connectToDb();

    if ( m_EnableDbImportTo )
        createCollectionTable();
    else
        createFileIncomingTable();
}
void ThreadDataBaseImport::closeDb()
{
    qDebug() << "ThreadDataBaseImport::closeDb()";
    if ( m_Db.isOpen()  )
        m_Db.close();
}

void ThreadDataBaseImport::closeConnection()
{
    qDebug() << "ThreadDataBaseImport::closeConnection";
    closeDb();

    m_Db.removeDatabase(s_DefaultDbImportConnectionName);
    m_ForeignDB.removeDatabase(s_ForeignDbConnectionName);
}
void ThreadDataBaseImport::run()
{
    qDebug() << "ThreadDataBaseImport::run";
    bool wasSuccessful=true;
    if (!m_Stopped)
    {
        QString queryStr ="SELECT * FROM CollectionTracks";// FROM " + path + ".SourceTable";
        QSqlQuery qry(m_ForeignDB);

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
                if (m_EnableDbImportTo)
                {
                    insertValueToMusicCollectionTable(QString::number(id++), albumN, trackN, audioMd5N, metaMd5N);
                }
                else
                {
                    insertValueToFileIncomingTable(QString::number(id++), albumN, trackN, audioMd5N, metaMd5N);
                }

                emit sigForeignDbEntryFound(QString::number(id++), albumN, trackN, audioMd5N, metaMd5N);
                mutex.unlock();
                //msleep(10);
            }

        }
        m_ForeignDB.close();
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

void ThreadDataBaseImport::slotEnableDbImportTo(bool toCollection)
{
    qDebug() << "ThreadDataBaseImport::slotEnableDbImportTo " << toCollection;
    m_EnableDbImportTo=toCollection;
}

void ThreadDataBaseImport::connectToForeignDB()
{
    qDebug() << "ThreadDataBaseImport::connectToForeignDB";
    if (!m_ForeignDB.isOpen())
    {
        //m_ForeignDB.addDatabase( "QSQLITE", "ForeignConnection" );
        m_ForeignDB.setDatabaseName( m_Path );
        if( !m_ForeignDB.open() )
        {
            qDebug() << m_ForeignDB.lastError();
            qFatal( "Failed to connect to foreign database." );
        }
        else
        {
            qDebug( "Successfully connected to foreign database" );
        }
    }
    else
    {
        qDebug() << "Still connected to foreign database!";
    }
}

void ThreadDataBaseImport::connectToDb()
{
    qDebug() << "ThreadDataBaseImport::connectToDb";

    if (!m_Db.isOpen())
    {
        //m_Db.addDatabase( "QSQLITE" );
        m_Db.setDatabaseName( DataBase::s_DatabaseName );
        if( !m_Db.open() )
        {
            qDebug() << "ThreadDataBaseImport::connectToDb " << m_Db.lastError();
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

    if (!m_Db.isOpen())
    {
        qDebug() << "try to reopen...";
        if ( !m_Db.open() )
        {
            qDebug() << "ThreadDataBaseImport::insertValueToFileIncomingTable(): Database not open!";
            return;
        }
    }

    QSqlQuery qry(m_Db);

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


    if (!m_Db.isOpen())
    {
        qDebug() << "try to reopen...";
        if ( !m_Db.open() )
        {
            qDebug() << "ThreadDataBaseImport::insertValueToMusicCollectionTable(): Database not open!";
            return;
        }
    }

    QSqlQuery qry(m_Db);

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

    QSqlQuery qry(m_Db);

    if( !m_Db.open() )
    {
        qDebug() << m_Db.lastError();
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

    QSqlQuery qry(m_Db);

    if( !m_Db.open() )
    {
        qDebug()  << "ThreadDataBaseImport: " << m_Db.lastError();
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
