#include "DataBase.hpp"
#include "FlacTrack.hpp"

const char* DataBase::s_DatabaseName = "trackDataBase.db3";
const char* DataBase::s_CollectionTableName = "CollectionTracks";
const char* DataBase::s_FileIncomingTableName = "FileIncomingTracks";
const char* DataBase::s_DatabaseDriver = "QSQLITE";

DataBase::DataBase(QObject *parent):
    QObject(parent),
    m_Path(DataBase::s_DatabaseName),
    m_TableNameCollection(DataBase::s_CollectionTableName),
    m_TableNameFileIncoming(DataBase::s_FileIncomingTableName),
    m_Db(QSqlDatabase::addDatabase( DataBase::s_DatabaseDriver ))
{
    //eraseDbPhysically();
    connectToDefaultDb();
    createCollectionTable();
    createFileIncomingTable();
}

DataBase::~DataBase()
{
    qDebug() << "DataBase::~DataBase()";
    closeConnection();
}

bool DataBase::isOpen()
{
    return m_Db.isOpen();
}

void DataBase::connectToDefaultDb()
{
    qDebug() << "DataBase::connectToDefaultDb()";
    // Create database.
    if (!m_Db.isOpen())
    {
        //m_Db = QSqlDatabase::addDatabase( "QSQLITE" );
        m_Db.setDatabaseName( m_Path );

        if( !m_Db.open() )
        {
            qDebug() << m_Db.lastError();
            qFatal( "Failed to connect." );
        }
        else
        {
           qDebug( "Connected!" );
        }
    }
    else
    {
        qDebug( "Still connected!" );
    }
}

void DataBase::createCollectionTable()
{
    qDebug() << "DataBase::createTableCollection";
    //qDebug() << QSqlDatabase::drivers();

    //connectToDefaultDb();
    QSqlQuery qry(m_Db);

    if( !m_Db.isOpen() )
    {
        qDebug() << m_Db.lastError();
        qFatal( "Failed to connect. Database is not open!" );
    }
    else
        qry.prepare( "CREATE TABLE IF NOT EXISTS " + m_TableNameCollection +
                     " (id INTEGER, albumPath VARCHAR(200), albumName VARCHAR(100), fileName VARCHAR(100), audioMd5 VARCHAR(33), metaMd5 VARCHAR(33))" );

    if( !qry.exec() )
        qDebug() << "Database: " << qry.lastError();
    else
        qDebug() << "Database: Successfully created Collection Table.";
}

void DataBase::createFileIncomingTable()
{
    qDebug() << "DataBase::createFileIncomingTable";

    //connectToDefaultDb();
    QSqlQuery qry(m_Db);

    if( !m_Db.isOpen() )
    {        
        qDebug() << "Your Database is not open! Here are the consequences: " << m_Db.lastError();
        return;
        //qFatal( "Failed to connect." );
    }
    else
        qry.prepare( "CREATE TABLE IF NOT EXISTS " + m_TableNameFileIncoming +
                     " (id INTEGER, albumPath VARCHAR(200), albumName VARCHAR(100), fileName VARCHAR(100), audioMd5 VARCHAR(33), metaMd5 VARCHAR(33))" );

    if( !qry.exec() )
        qDebug() << "Database: " << qry.lastError();
    else
        qDebug() << "Successfully created File Incoming Table.";
}

void DataBase::insertValueToCollectionTable(const QString& id, const QString &albumPath, const QString &fileName, const QString& audioMd5, const QString& metaMd5)
{
    //qDebug() << "DataBase::insertValueToCollection: id = " << id << " albumPath = " << albumPath << " fileName = " << fileName << " audioMd5 = " << audioMd5 << " metaMd5 = " << metaMd5;

    if (!m_Db.isOpen())
    {
        qDebug() << "try to reopen...";
        if ( !m_Db.open() )
        {
            qDebug() << "DataBase::insertValueToCollectionTable(): Database not open!";
            return;
        }
    }

    // create albumname from albumpath
    QString albumName = Utils::getBaseFromPath(albumPath);
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
        qDebug() << "Database: " << qry.lastError();
    // else
    //   qDebug( "Inserted to Collection-Table!" );
}

void DataBase::insertValueToFileIncomingTable(const QString &id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5)
{
    //qDebug() << "DataBase::insertValueToFileIncomingTable id = " << id << " albumPath = " << albumPath << " fileName = " << fileName << " audioMd5 = " << audioMd5 << " metaMd5 = " << metaMd5;
    // create albumname from albumpath
    QString albumName = Utils::getBaseFromPath(albumPath);

    if (!m_Db.isOpen())
    {
        qDebug() << "try to reopen...";
        if ( !m_Db.open() )
        {
            qDebug() << "DataBase::insertValueToFileIncomingTable(): Database not open!";
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
        qDebug() << "Database: " << qry.lastError();
    // else
    //    qDebug( "Inserted to FileIncoming-Table!" );
}

void DataBase::deleteCollectionTable()
{
    qDebug() << "DataBase::deleteCollectionTable";

    if ( m_Db.isOpen() )
    {
        QSqlQuery qry(m_Db);
        qry.prepare( "DELETE FROM " + m_TableNameCollection );
        if( !qry.exec() )
            qDebug() << "Database: " << qry.lastError();
        else
            qDebug( "Database: Deleted CollectionTracks table!" );
    }
    else
    {
        qDebug() << "Database: not open";
    }


}

bool DataBase::deleteFileIncomingTable()
{
    qDebug() << "DataBase::deleteFileIncomingTable";
    bool retVal=false;

    if (m_Db.isOpen())
    {
        QSqlQuery qry(m_Db);
        qry.prepare( "DELETE FROM " + m_TableNameFileIncoming );
        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
            retVal=false;
        }
        else
        {
            qDebug( "Deleted FileIncomingTracks Table" );
            retVal = true;
        }
    }
    else
    {
        qDebug() << "Database: not open!";
    }
    return retVal;
}

void DataBase::queryAndPush(const QString &queryString)
{
    qDebug() << "DataBase::queryAndPush";

    if (!m_Db.isOpen())
    {
        qDebug() << "try to reopen...";
        if ( !m_Db.open() )
        {
            qDebug() << "DataBase::queryAndPush(): Database not open!";
            return;
        }
    }

    QSqlQuery qry(m_Db);

    // Select all.
    qry.prepare(queryString);
    if( !qry.exec() )
        qDebug() << "Database: " << qry.lastError();
    else
    {
        m_QueryResultsToShow.clear();
        //qDebug( "Poke out all update values..." );

        QSqlRecord rec = qry.record();

        int albumCol = rec.indexOf("albumPath");
        int trackCol = rec.indexOf("fileName");
        //int audioMd5Col = rec.indexOf("audioMd5");
        //int metaMd5Col = rec.indexOf("metaMd5");

        QString albumN="";
        QString trackN="";
        // QString audioMd5N="";
        // QString metaMd5N="";
        QString lastAlbName="";

        while(qry.next())
        {
            albumN = qry.value(albumCol).toString();
            trackN = qry.value(trackCol).toString();
            //  audioMd5N = qry.value(audioMd5Col).toString();
            //  metaMd5N = qry.value(metaMd5Col).toString();

            if ( lastAlbName != albumN )
            {
                //qDebug() << albumN;
                m_QueryResultsToShow.push_back(albumN);
                lastAlbName = albumN;
            }

            // qDebug() << trackN;
            m_QueryResultsToShow.push_back(trackN);
        }
    }
}

// Shows entries from file-incoming table, that are not found in collection-table.
void DataBase::selectTableDiffAudioMd5()
{
    qDebug() << "DataBase::selectTableDiffAudioMd5";

    // Select all.
    QString queryString = "select * from " + m_TableNameFileIncoming + " where not exists (select * from " + m_TableNameCollection + " where " + m_TableNameFileIncoming + ".audioMd5 = " + m_TableNameCollection + ".audioMd5)";
    queryAndPush(queryString);

    // select * from FileIncomingTracks where not exists (select * from CollectionTracks where FileIncomingTracks.audioMd5 = CollectionTracks.audioMd5);
    // or
    /*
        SELECT id, fileName, audioMd5, metaMd5
        FROM FileIncomingTracks
        EXCEPT
        SELECT id, fileName, audioMd5, metaMd5
        FROM CollectionTracks
        ;
    */
}

void DataBase::selectTableDiffMetaMd5()
{

    QString queryString = "select * from " + m_TableNameFileIncoming + " where exists (select * from " + m_TableNameCollection + " where ((" + m_TableNameFileIncoming + ".metaMd5 <> " + m_TableNameCollection + ".metaMd5) AND (" + m_TableNameFileIncoming + ".audioMd5 = " + m_TableNameCollection + ".audioMd5)))";
    queryAndPush(queryString);
}

void DataBase::selectTableDiffFileName()
{
    QString queryString = "select * from " + m_TableNameFileIncoming + " where exists (select * from " + m_TableNameCollection + " where ((" + m_TableNameFileIncoming + ".fileName <> " + m_TableNameCollection + ".fileName) AND (" + m_TableNameFileIncoming + ".audioMd5 = " + m_TableNameCollection + ".audioMd5)))";
    queryAndPush(queryString);
}

void DataBase::selectTableDiffAlbumName()
{
    //
    // QString queryString = "select * from " + m_TableNameFileIncoming + " where not exists (select * from " + m_TableNameCollection + " where " + m_TableNameFileIncoming + ".albumName = " + m_TableNameCollection + ".albumName)";
    QString queryString = "select * from " + m_TableNameFileIncoming + " where exists (select * from " + m_TableNameCollection + " where ((" + m_TableNameFileIncoming + ".albumName <> " + m_TableNameCollection + ".albumName) AND (" + m_TableNameFileIncoming + ".metaMd5 = " + m_TableNameCollection + ".metaMd5)AND (" + m_TableNameFileIncoming + ".audioMd5 = " + m_TableNameCollection + ".audioMd5)AND (" + m_TableNameFileIncoming + ".fileName = " + m_TableNameCollection + ".fileName)))";
    queryAndPush(queryString);
}

QStringList DataBase::getDiffList() const
{
    return m_QueryResultsToShow;
}

// To avoid fragementation of database, we have to periodically destroy the file.
void DataBase::eraseDbPhysically()
{
    qDebug() << "DataBase::eraseDbPhysically()";

    if ( QFile::exists(m_Path) )
    {
        if (m_Db.isOpen())
            m_Db.close();

        if (QFile::remove(m_Path))
            qDebug() << m_Path << " successfully removed!";
        else
            qDebug() << m_Path << " remove failed!";
    }
    else
    {
        qDebug() << m_Path <<  " could not have been removed! File does not exist.";
    }
}

void DataBase::closeDb()
{
    qDebug() << "DataBase::closeDb()";
    m_Db.close();
}

void DataBase::closeConnection()
{
    qDebug() << "DataBase::closeConnection";
    if ( m_Db.isOpen()  )
        closeDb();

    m_Db.removeDatabase("qt_sql_default_connection");
}

void DataBase::slotInsertValueToCollectionTable(const QString &id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5)
{
    insertValueToCollectionTable(id, albumPath, fileName, audioMd5, metaMd5);
}

void DataBase::slotInsertValueToFileIncomingTable(const QString &id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5)
{
    //qDebug() << "DataBase::slotInsertValueToFileIncomingTable " << fileName;
    insertValueToFileIncomingTable(id, albumPath, fileName, audioMd5, metaMd5);
}

void DataBase::selectValue()
{
    qDebug() << "DataBase::selectValue";

    QSqlQuery qry;

    // Select fileName = Roe
    qry.prepare( "SELECT albumPath, fileName FROM " + m_TableNameCollection + " WHERE fileName = 'Roe'" );
    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "Selected!" );

        QSqlRecord rec = qry.record();

        int cols = rec.count();

        for( int c=0; c<cols; c++ )
            qDebug() << QString( "Column %1: %2" ).arg( c ).arg( rec.fieldName(c) );

        for( int r=0; qry.next(); r++ )
            for( int c=0; c<cols; c++ )
                qDebug() << QString( "Row %1, %2: %3" ).arg( r ).arg( rec.fieldName(c) ).arg( qry.value(c).toString() );
    }
}

void DataBase::selectInOrder()
{
    qDebug() << "DataBase::selectInOrder";
    QSqlQuery qry;
    qry.prepare( "SELECT albumPath, fileName FROM " + m_TableNameCollection + " WHERE fileName = 'Roe' ORDER BY albumPath" );
    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "Selected!" );

        QSqlRecord rec = qry.record();

        int cols = rec.count();

        for( int c=0; c<cols; c++ )
            qDebug() << QString( "Column %1: %2" ).arg( c ).arg( rec.fieldName(c) );

        for( int r=0; qry.next(); r++ )
            for( int c=0; c<cols; c++ )
                qDebug() << QString( "Row %1, %2: %3" ).arg( r ).arg( rec.fieldName(c) ).arg( qry.value(c).toString() );
    }
}

void DataBase::selectInOrderCount()
{
    qDebug() << "DataBase::selectInOrderCount";

    QSqlQuery qry;

    qry.prepare( "SELECT fileName, COUNT(*) as 'members' FROM " + m_TableNameCollection + " GROUP BY fileName ORDER BY fileName" );
    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "Selected!" );

        QSqlRecord rec = qry.record();

        int cols = rec.count();

        for( int c=0; c<cols; c++ )
            qDebug() << QString( "Column %1: %2" ).arg( c ).arg( rec.fieldName(c) );

        for( int r=0; qry.next(); r++ )
            for( int c=0; c<cols; c++ )
                qDebug() << QString( "Row %1, %2: %3" ).arg( r ).arg( rec.fieldName(c) ).arg( qry.value(c).toString() );
    }
}

void DataBase::updateValueById()
{
    qDebug() << "DataBase::updateValueById";

    QSqlQuery qry;

    qry.prepare( "UPDATE " + m_TableNameCollection + " SET albumPath = 'Nisse', fileName = 'Svensson' WHERE id = 7" );
    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
        qDebug( "Updated!" );
}

void DataBase::updateValue()
{
    qDebug() << "DataBase::updateValue";

    QSqlQuery qry;

    qry.prepare( "UPDATE " + m_TableNameCollection + " SET fileName = 'Johnson' WHERE albumPath = 'Jane'" );
    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
        qDebug( "Updated!" );
}

void DataBase::deleteValueById()
{
    qDebug() << "DataBase::deleteValueById";

    QSqlQuery qry;

    qry.prepare( "DELETE FROM " + m_TableNameCollection + " WHERE id = 7" );
    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
        qDebug( "Deleted!" );
}

void DataBase::deleteValue()
{
    qDebug() << "DataBase::deleteValue";

    QSqlQuery qry;

    qry.prepare( "DELETE FROM " + m_TableNameCollection + " WHERE fileName = 'Johnson'" );
    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
        qDebug( "Deleted!" );
}
