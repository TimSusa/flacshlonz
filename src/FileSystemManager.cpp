//////////////////////////////////////////////////////////////////////
//!\file FileSystemManager.cpp
//!\brief Does filesearch and all other file processings in a separate thread.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#include "FileSystemManager.hpp"
#include <QMessageBox>
#include <QDebug>
#include <QDirIterator>
//#include "Utils.hpp"


FileSystemManager::FileSystemManager(QObject *parent) :
    QObject(parent),
    m_Persistence(),
   // m_DeepScanEnabled(m_Persistence.getEnableDeepScan()),
    m_CopyProgressCount(0),
    m_RemoveProgressCount(0),
    m_pScannerMusicCollection(),
    m_pScannerUpdateCollection(),
    m_pThreadRemoveFiles(),
    m_pThreadCopyFiles(),
    m_pThreadDataBaseImport(),
    m_Db(new DataBase(this)),
    m_MainWindowState(states::MAINSTATE_NONE),
    m_SelfState(states::FM_NONE)
{

}

FileSystemManager::~FileSystemManager()
{
    destroyThreadCollectionFlacScan();
    destroyThreadUpdateFlacScan();
    destroyThreadCopyFiles();
    destroyThreadRemoveFiles();
    destroyThreadDBImport();
}

QStringList FileSystemManager::getDiffListAudioMd5()
{
    qDebug() << "FileSystemManager::getDiffListAudioMd5";
    m_Db->selectTableDiffAudioMd5();
    return m_Db->getDiffList();
}

QStringList FileSystemManager::getDiffListMetaMd5()
{
    qDebug() << "FileSystemManager::getDiffListMetaMd5";
    m_Db->selectTableDiffMetaMd5();
    return m_Db->getDiffList();
}

QStringList FileSystemManager::getDiffListFileNames()
{
    qDebug() << "FileSystemManager::getDiffListFileNames";
    m_Db->selectTableDiffFileName();
    return m_Db->getDiffList();
}

QStringList FileSystemManager::getDiffListAlbumName()
{
    qDebug() << "FileSystemManager::getDiffListAlbumName";
    m_Db->selectTableDiffAlbumName();
    return m_Db->getDiffList();
}

states::eFileSystemManagerState FileSystemManager::getSelfState() const
{
    return m_SelfState;
}

void FileSystemManager::slotSearchInMusicCollection(QString path)
{
    qDebug() << "FileSystemManager::slotSearchInMusicCollection " << path;

    if ((NULL!=m_Db))
    {
        if (m_Db->isOpen())
            m_Db->deleteCollectionTable();
    }


    scanDirsToMusicCollection(QDir(path));
}

void FileSystemManager::slotSearchInFileIncoming(QString path)
{
    qDebug() << "FileSystemManager::slotSearchInFileIncoming " << path;

    if (NULL!=m_Db)
    {
        if (m_Db->isOpen())
            m_Db->deleteFileIncomingTable();
    }

    scanDirsToUpdateCollection(QDir(path));
}

void FileSystemManager::slotRemoveFromFileSystem(QStringList pathList)
{
    qDebug() << "FileSystemManager::slotRemoveFromFileSystem ";

    // Recreate thread!
    destroyThreadRemoveFiles();
    createAndConnectThreadRemoveFiles(pathList);
    m_pThreadRemoveFiles->start();
    qDebug() << "m_pThreadRemoveFiles started....";
}

void FileSystemManager::scanDirsToMusicCollection(const QDir &dir)
{
    m_SelfState = states::FM_SCANNING_MUSIC_COLLECTION;
    destroyThreadCollectionFlacScan();
    createAndConnectThreadCollectionFlacScan(dir);
    m_pScannerMusicCollection->start(); // makes high prio sense here?
    qDebug() << "m_pScannerMusicCollection started....";
}

void FileSystemManager::scanDirsToUpdateCollection(const QDir &dir)
{

m_SelfState = states::FM_SCANNING_FILE_INCOMING;
    destroyThreadUpdateFlacScan();
    createAndConnectThreadUpdateFlacScan(dir);
    m_pScannerUpdateCollection->start();
    qDebug() << "m_pScannerUpdateCollection started...";
}

void FileSystemManager::createAndConnectThreadCollectionFlacScan(const QDir &dir)
{
    m_pScannerMusicCollection= new ThreadFlacScanner(dir);
    connect(this, SIGNAL(sigStopScanner()), m_pScannerMusicCollection, SLOT(slotStop()));
    connect(m_pScannerMusicCollection, SIGNAL(sigFlacSearchFinished()), this, SIGNAL(sigSearchInMusicCollectionFinished()));
    // for inserting to db
    connect(m_pScannerMusicCollection, SIGNAL(sigAlbumEntryFound(QString,QString,QString,QString,QString)), m_Db, SLOT(slotInsertValueToCollectionTable(QString,QString,QString,QString,QString)));
    // for triggering GUI
    connect(m_pScannerMusicCollection, SIGNAL(sigAlbumEntryFound(QString,QString,QString,QString,QString)), this, SIGNAL(sigAlbumEntryCollectionFound(QString,QString,QString,QString,QString)));
    connect(this, SIGNAL(sigEnableDeepScan(bool)), m_pScannerMusicCollection, SLOT(slotDeepScanEnabled(bool)));
}

void FileSystemManager::createAndConnectThreadUpdateFlacScan(const QDir &dir)
{
    m_pScannerUpdateCollection= new ThreadFlacScanner(dir);
    connect(this, SIGNAL(sigStopScanner()), m_pScannerUpdateCollection, SLOT(slotStop()));
    connect(m_pScannerUpdateCollection, SIGNAL(sigFlacSearchFinished()), this, SIGNAL(sigSearchInFileIncomingFinished()));
    // for inserting to db
    connect(m_pScannerUpdateCollection, SIGNAL(sigAlbumEntryFound(QString,QString,QString,QString,QString)), m_Db, SLOT(slotInsertValueToFileIncomingTable(QString,QString,QString,QString,QString)));
    // for triggering GUI
    connect(m_pScannerUpdateCollection, SIGNAL(sigAlbumEntryFound(QString,QString,QString,QString,QString)), this, SIGNAL(sigAlbumEntryFileIncomingFound(QString,QString,QString,QString,QString)));
    connect(this, SIGNAL(sigEnableDeepScan(bool)), m_pScannerUpdateCollection, SLOT(slotDeepScanEnabled(bool)));

}

void FileSystemManager::createAndConnectThreadCopyFiles(const QStringList &src, const QStringList &dest)
{
    m_pThreadCopyFiles= new ThreadCopyFiles(src, dest);
    connect(m_pThreadCopyFiles, SIGNAL(sigCopyProgressCount(int,QString)), this, SIGNAL(sigCopyProgress(int, QString)));
    connect(m_pThreadCopyFiles, SIGNAL(sigCopyFinished(bool)), this, SIGNAL(sigCopyProgressFinished(bool)));
    connect(this, SIGNAL(sigStopCopy()), m_pThreadCopyFiles, SLOT(slotStopCopy()));
}

void FileSystemManager::createAndConnectThreadRemoveFiles(const QStringList &path)
{
    m_pThreadRemoveFiles= new ThreadRemoveFiles(path);
    connect(m_pThreadRemoveFiles, SIGNAL(sigRemoveProgressCount(int, QString)), this, SIGNAL(sigRemoveProgress(int, QString)));
    connect(m_pThreadRemoveFiles, SIGNAL(sigRemoveRunFinished(bool)), this, SIGNAL(sigRemoveRunFinished(bool)));
}

void FileSystemManager::createAndConnectThreadDBImport(const QString &path, bool importToCollection)
{
    qDebug() << "FileSystemManager::createAndConnectThreadDBImport";
    m_pThreadDataBaseImport = new ThreadDataBaseImport(path, importToCollection);
    connect(this, SIGNAL(sigStopScanner()), m_pThreadDataBaseImport, SLOT(slotStop()));
    connect(m_pThreadDataBaseImport, SIGNAL(sigThreadFinished(bool)), this, SIGNAL(sigDataBaseImportFinished()));
    connect(m_pThreadDataBaseImport, SIGNAL(sigImportDbEntryFound(QString,QString,QString,QString,QString)), this, SIGNAL(sigImportDbEntryFound(QString,QString,QString,QString,QString)));
}

void FileSystemManager::destroyThreadCollectionFlacScan()
{
    if ( NULL != m_pScannerMusicCollection )
    {
        disconnect(this, SIGNAL(sigStopScanner()), m_pScannerMusicCollection, SLOT(slotStop()));
        disconnect(m_pScannerMusicCollection, SIGNAL(sigFlacSearchFinished()), this, SIGNAL(sigSearchInMusicCollectionFinished()));
        disconnect(m_pScannerMusicCollection, SIGNAL(sigAlbumEntryFound(QString,QString,QString,QString,QString)), m_Db, SLOT(slotInsertValueToCollectionTable(QString,QString,QString,QString,QString)));
        disconnect(m_pScannerMusicCollection, SIGNAL(sigAlbumEntryFound(QString,QString,QString,QString,QString)), this, SIGNAL(sigAlbumEntryCollectionFound(QString,QString,QString,QString,QString)));
        disconnect(this, SIGNAL(sigEnableDeepScan(bool)), m_pScannerMusicCollection, SLOT(slotDeepScanEnabled(bool)));

        if (m_pScannerMusicCollection->isRunning())
        {
            qDebug() << "m_pScannerMusicCollection is running, so we attempt to stopp and quit, before delete!";
            //emit sigStopScanUpdateCollection();
            m_pScannerMusicCollection->wait();
            m_pScannerMusicCollection->quit();
        }
        qDebug() << "Deleting old m_pScannerMusicCollection!";
        delete m_pScannerMusicCollection;
    }
}

void FileSystemManager::destroyThreadUpdateFlacScan()
{
    if ( NULL != m_pScannerUpdateCollection )
    {
        disconnect(this, SIGNAL(sigStopScanner()), m_pScannerUpdateCollection, SLOT(slotStop()));
        disconnect(m_pScannerUpdateCollection, SIGNAL(sigFlacSearchFinished()), this, SIGNAL(sigSearchInFileIncomingFinished()));
        // for inserting to db
        disconnect(m_pScannerUpdateCollection, SIGNAL(sigAlbumEntryFound(QString,QString,QString,QString,QString)), m_Db, SLOT(slotInsertValueToFileIncomingTable(QString,QString,QString,QString,QString)));
        // for triggering GUI
        disconnect(m_pScannerUpdateCollection, SIGNAL(sigAlbumEntryFound(QString,QString,QString,QString,QString)), this, SIGNAL(sigAlbumEntryFileIncomingFound(QString,QString,QString,QString,QString)));
        disconnect(this, SIGNAL(sigEnableDeepScan(bool)), m_pScannerUpdateCollection, SLOT(slotDeepScanEnabled(bool)));

        if (m_pScannerUpdateCollection->isRunning())
        {
            qDebug() << "m_pScannerUpdateCollection is running, so we attempt to stopp and quit, before delete!";
            // emit sigStopScanUpdateCollection();
            m_pScannerUpdateCollection->wait();
            m_pScannerUpdateCollection->quit();
        }
        qDebug() << "Deleting old m_pScannerUpdateCollection";
        delete m_pScannerUpdateCollection;
    }
}

void FileSystemManager::destroyThreadCopyFiles()
{
    if ( NULL != m_pThreadCopyFiles )
    {
        disconnect(m_pThreadCopyFiles, SIGNAL(sigCopyProgressCount(int,QString)), this, SIGNAL(sigCopyProgress(int, QString)));
        disconnect(m_pThreadCopyFiles, SIGNAL(sigCopyFinished(bool)), this, SIGNAL(sigCopyProgressFinished(bool)));
        disconnect(this, SIGNAL(sigStopCopy()), m_pThreadCopyFiles, SLOT(slotStopCopy()));

        if (m_pThreadCopyFiles->isRunning())
        {
            qDebug() << "m_pThreadCopyFiles is running, so we attempt to stopp and quit, before delete!";
            m_pThreadCopyFiles->wait();
            m_pThreadCopyFiles->quit();
        }
        qDebug() << "Deleting old m_pThreadCopyFiles!";
        delete m_pThreadCopyFiles;
    }
}

void FileSystemManager::destroyThreadRemoveFiles()
{
    if ( NULL != m_pThreadRemoveFiles )
    {
        disconnect(m_pThreadRemoveFiles, SIGNAL(sigRemoveProgressCount(int, QString)), this, SIGNAL(sigRemoveProgress(int, QString)));
        disconnect(m_pThreadRemoveFiles, SIGNAL(sigRemoveRunFinished(bool)), this, SIGNAL(sigRemoveRunFinished(bool)));
        if (m_pThreadRemoveFiles->isRunning())
        {
            qDebug() << "m_pThreadRemoveFiles is running, so we attempt to stopp and quit, before delete!";
            m_pThreadRemoveFiles->wait();
            m_pThreadRemoveFiles->quit();
        }
        qDebug() << "Deleting old m_pThreadRemoveFiles!";
        delete m_pThreadRemoveFiles;
    }
}

void FileSystemManager::destroyThreadDBImport()
{
    if ( NULL != m_pThreadDataBaseImport )
    {
        emit sigStopScanner();
        emit m_pThreadDataBaseImport->slotCloseConnection();
        disconnect(this, SIGNAL(sigStopScanner()), m_pThreadDataBaseImport, SLOT(slotStop()));
        disconnect(m_pThreadDataBaseImport, SIGNAL(sigThreadFinished(bool)), this, SIGNAL(sigSearchInFileIncomingFinished()));
        disconnect(m_pThreadDataBaseImport, SIGNAL(sigImportDbEntryFound(QString,QString,QString,QString,QString)), this, SIGNAL(sigImportDbEntryFound(QString,QString,QString,QString,QString)));
        //disconnect(m_pThreadDataBaseImport, SIGNAL(sigForeignDbEntryFound(QString,QString,QString,QString,QString)), m_Db, SLOT(slotInsertValueToFileIncomingTable(QString,QString,QString,QString,QString)));
        /*if (m_pThreadDataBaseImport->isRunning())
        {
            qDebug() << "m_pThreadDataBaseImport is running, so we attempt to stopp and quit, before delete!";
            m_pThreadDataBaseImport->wait();
            m_pThreadDataBaseImport->quit();
        }
        */
        qDebug() << "Deleting old m_pThreadDataBaseImport!";
        m_pThreadDataBaseImport->wait();
        m_pThreadDataBaseImport->quit();
        delete m_pThreadDataBaseImport;
    }
}

void FileSystemManager::slotSetMainWindowState(states::eMainState state)
{
    m_MainWindowState=state;
    QString tmpStr="";
    switch(state)
    {
    case states::MAINSTATE_NONE:
        tmpStr="NONE";
        break;
    case states::MAINSTATE_IDLE:
        tmpStr="IDLE";
        break;
    case states::MAINSTATE_DB_IMPORT:
        tmpStr="DB_IMPORT";
        break;
    case states::MAINSTATE_FILE_SCAN:
        tmpStr="FILE_SCAN";
        break;
    default:
        break;
    }
    qDebug() << "FileSystemManager::slotMainWindowState: " << tmpStr;
}

void FileSystemManager::slotCopyAllFolderTo( QStringList srcPaths, QStringList destPaths)
{
    qDebug() << "FileSystemManager::slotCopyAllFolderTo()";

    // Recreate thread!
    destroyThreadCopyFiles();
    createAndConnectThreadCopyFiles(srcPaths, destPaths);
    m_SelfState=states::FM_COPY;
    m_pThreadCopyFiles->start();
    qDebug() << "New Remove-Thread created and started....";

    // Reset File Counter.
    m_CopyProgressCount = 0;
}

void FileSystemManager::slotResetCopyProgressCounter()
{
    m_CopyProgressCount = 0;
}

void FileSystemManager::slotResetRemoveProgressCounter()
{
    m_RemoveProgressCount = 0;
}

void FileSystemManager::slotEnableDeepScan(bool enable)
{
    qDebug() << "FileSystemManager::slotEnableDeepScan: " << enable;
    m_DeepScanEnabled = enable;
    emit sigEnableDeepScan(enable);
}

void FileSystemManager::slotInitOfMusicCollectionModelFinished()
{
    qDebug() << "FileSystemManager::slotInitOfAlbumModelFinished()";
    emit sigSearchInMusicCollectionFinished();
}

void FileSystemManager::slotInitOfUpdateCollectionModelFinished()
{
    qDebug() << "FileSystemManager::slotInitOfUpdateCollectionModelFinished()";
    emit sigSearchInFileIncomingFinished();
}

void FileSystemManager::slotImportDbToCollection(QString path)
{
    qDebug() << " FileSystemManager::slotImportDbToCollection " << path;
    m_SelfState = states::FM_DB_IMPORT_COLLECTION;
    destroyThreadDBImport();
    //    destroyThreadCollectionFlacScan();
    //    destroyThreadUpdateFlacScan();

    if (NULL!= m_Db)
    {
        m_Db->deleteCollectionTable();
        m_Db->createCollectionTable();

        delete m_Db;
        m_Db = new DataBase(this);


    }

    createAndConnectThreadDBImport(path, true);
    //emit m_pThreadDataBaseImport->slotEnableDbImportTo(true);
    m_pThreadDataBaseImport->start();
    //emit sigSearchInFileIncomingFinished();
}

void FileSystemManager::slotImportDbToIncoming(QString path)
{
    qDebug() << "FileSystemManager::slotImportDbToIncoming: " << path;
m_SelfState = states::FM_DB_IMPORT_INCOMING;
    destroyThreadDBImport();
  //  destroyThreadCollectionFlacScan();
  //  destroyThreadUpdateFlacScan();

    if (NULL!= m_Db)
    {
        m_Db->deleteFileIncomingTable();
        m_Db->createFileIncomingTable();

        delete m_Db;
        m_Db = new DataBase(this);


    }

    createAndConnectThreadDBImport(path, false);
    //emit m_pThreadDataBaseImport->slotEnableDbImportTo(false);
    m_pThreadDataBaseImport->start();

}
