//////////////////////////////////////////////////////////////////////
//!\file FileSystemManager.hpp
//!\brief Does filesearch and all other file processings in a separate thread.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef FILESYSTEMMANAGER_HPP
#define FILESYSTEMMANAGER_HPP

#include <QObject>
#include <QDir>
#include <QString>
#include "FlacTrack.hpp"
#include <QVector>
#include <QFileInfo>
#include <QSet>
#include "Persistence.hpp"
#include "ThreadFlacScanner.hpp"
#include "ThreadRemoveFiles.hpp"
#include "ThreadCopyFiles.hpp"
#include "DataBase.hpp"
#include "ThreadDataBaseImport.hpp"
#include "States.hpp"

class FileSystemManager : public QObject
{
    Q_OBJECT
public:
    FileSystemManager(QObject *parent = 0);
    ~FileSystemManager();
    QStringList getDiffListAudioMd5();
    QStringList getDiffListMetaMd5();
    QStringList getDiffListFileNames();
    QStringList getDiffListAlbumName();
    states::eFileSystemManagerState getSelfState()const;

private:
    void scanDirsToMusicCollection(const QDir& dir);
    void scanDirsToUpdateCollection(const QDir& dir);
    void createAndConnectThreadCollectionFlacScan(const QDir &dir);
    void createAndConnectThreadUpdateFlacScan(const QDir &dir);
    void createAndConnectThreadCopyFiles(const QStringList &src,const QStringList &dest);
    void createAndConnectThreadRemoveFiles(const QStringList &path);
    void createAndConnectThreadDBImport(const QString& path, bool importToCollection);
    void destroyThreadCollectionFlacScan();
    void destroyThreadUpdateFlacScan();
    void destroyThreadCopyFiles();
    void destroyThreadRemoveFiles();
    void destroyThreadDBImport();

signals:    
    void sigAlbumEntryCollectionFound(QString, QString, QString, QString, QString);
    void sigAlbumEntryFileIncomingFound(QString, QString, QString, QString, QString);
    void sigSearchInMusicCollectionFinished();
    void sigSearchInFileIncomingFinished();
    void sigDataBaseImportFinished();
    void sigLoadForeignDbFinished();
    void sigCopyProgress(int cnt, QString path);
    void sigCopyProgressFinished(bool wasSuccessful);
    void sigRemoveProgress(int cnt, QString path);
    void sigRemoveRunFinished(bool wasSuccessful);
    void sigEnableDeepScan(bool enable);
    void sigStopScanMusicCollection();
    void sigStopScanUpdateCollection();
    void sigInitMusicCollectionByPath(QString path);
    void sigInitUpdateCollectionByPath(QString path);
    void sigStopScanner();
    void sigStopCopy();
    void sigImportDbEntryFound( const QString& id, const QString &albumPath, const QString &fileName, const QString &audioMd5, const QString &metaMd5);
  //  void sigEnableDbImportTo(bool);


public slots:
    void slotSetMainWindowState(states::eMainState state);
    void slotSearchInMusicCollection(QString path);
    void slotSearchInFileIncoming(QString path);
    void slotRemoveFromFileSystem(QStringList pathList);
    void slotCopyAllFolderTo(QStringList srcPaths, QStringList destPaths);
    void slotResetCopyProgressCounter();
    void slotResetRemoveProgressCounter();
    void slotEnableDeepScan(bool enable);
    void slotInitOfMusicCollectionModelFinished();
    void slotInitOfUpdateCollectionModelFinished();
    void slotImportDbToCollection(QString path);
    void slotImportDbToIncoming(QString path);

private:
    Persistence             m_Persistence;
    bool                    m_DeepScanEnabled;
    int                     m_CopyProgressCount;
    int                     m_RemoveProgressCount;
    ThreadFlacScanner       *m_pScannerMusicCollection;
    ThreadFlacScanner       *m_pScannerUpdateCollection;
    ThreadRemoveFiles       *m_pThreadRemoveFiles;
    ThreadCopyFiles         *m_pThreadCopyFiles;
    ThreadDataBaseImport    *m_pThreadDataBaseImport;
    DataBase                *m_Db;
    states::eMainState      m_MainWindowState;
    states::eFileSystemManagerState m_SelfState;
};

#endif // FILESYSTEMMANAGER_HPP
