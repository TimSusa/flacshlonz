//////////////////////////////////////////////////////////////////////
//!\file MainWindow.hpp
//!\brief Main Application.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QtGui>
#include "FileSystemModel.hpp"
#include <QProgressDialog>
#include <QProgressBar>
//#include "StatusBar.hpp"
//#include "FlacTrack.hpp"
#include "FlacMetaDataManager.hpp"
#include "FileSystemManager.hpp"
//#include "Persistence.hpp"
#include "ConfigDialog.hpp"

QT_BEGIN_NAMESPACE
class QWidget;
class QTreeView;
class QFileInfo;
class QDebug;
class QTableWidget;
class StatusBar;
class FlacTrack;
class Persistence;
class QComboBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0 );
    ~MainWindow();

    enum eMainState
    {
        NONE = 0,
        IDLE,
        DB_IMPORT,
        FILE_SCAN
    };

    QString saveState() const; // X11
    QString getSessionFileName() const; // X11
    void restoreState(); // X11

public slots:
    void onSetMusicCollectionPath();
    void onSetFileIncomingPath();
    void onShowSettingsMenu();
    void onShowDiffList(const QStringList& list);
    void onClickedData( const QModelIndex & index ); // When clicked on left Tree View
    void onDblClickedData( const QModelIndex & index );
    void onCollectionTreeCtxMenu( QPoint );
    void onUpdateTreeClickedIdx(QModelIndex idx); // right
    void onUpdateTreeDblClicked(QModelIndex);
    void onTreeUpdateCtxMenu(QPoint);
    void onCtxTopMenuCopyTo();
    void slotAlbumEntryCollectionFound(QString id, QString albumName, QString trackName, QString audioMd5, QString metaMd5);
    void slotAlbumEntryFileIncomingFound(QString id, QString albumName, QString trackName, QString audioMd5, QString metaMd5);
    void slotSearchInMusicCollectionFinished();
    void slotSearchInFileIncomingFinished();
    void slotLoadForeignDbFinished(); // not connected not in use.
    void slotCopyToMusicCollection();
    void slotCopyProgress(int cnt, QString path);
    void slotCopyProgressFinished(bool wasSuccessful);
    void slotRemoveProgress(int cnt, QString path);
    void slotRemoveProgressFinished(bool wasSuccessful);
    void slotRemoveFromCollection();
    void slotRescanAll();
    void slotSetItemToCollectionRootPath();
    void slotGoToLastCollectionRootPath();
    void slotCreateDirInCollection();
    void slotRenameDirInCollection();
    // void slotDbQueryRequested();
    void slotRenamedDirPressedEnter();
    void slotEnableWizard(bool enable);
    void slotEnableDbImportTo(bool enable);
    void slotEnableDeepScan(bool enable);
    void slotRunSelectCollectionPathMsgBox();
    void slotRescanButtonClicked();
    void slotSelectFromComboBox(int selection);
    void slotLoadForeignDb();
    void slotSaveDbAs();

signals:   
    void sigSearchInMusicCollection(QString path);
    void sigSearchInFileIncoming(QString path);
    void sigCopy(bool isFile, QString source, QString dest);
    void sigCopyAllFolderTo(QStringList srcPaths, QStringList destPath);
    void sigResetCopyProgressCounter();
    void sigResetRemoveProgressCounter();
    void sigRescanAll();
    void sigRemoveFromFileSystem(QStringList pathList);
    void sigEnableDeepScan(bool enable);
    void sigRunSelectCollectionPathMsgBox();
    void sigStopAllSearch();
    void sigLoadForeignDb(QString path);

private:
    // Private methods
    void createMainGui();
    void createFileSystemManagerThread();
    void createComboBox();
    bool runMsgBoxRemoveSelection();
    bool runMsgBoxFinishedFlacSearchCollection();
    bool runMsgBoxSelectCollectionPath();
    bool runMsgBoxFolderExistsOrCopy();
    bool runMsgBoxIncomingPathContainedCollectionPath();
    void setTableLabels(int count);
    void setTreeViewRoot(const QString& path);
    void fillMetavalueTable();
    void putFlacMetaDataToTable(const QVector< QString >& metadataValues, bool isFile);
    void traverseFileInfoList();
    QFileInfoList getSelectionListFromCollectionTreeView()const;
    void removeCurFileInfoContent();
    void removeSelectionFromCollection();
    void removeContentFromFileInfoList(const QFileInfoList& fileInfoList);
    void copySelectionFromUpdateCollectionTop( const QString& dest );
    const QFileInfo& getFileInfoByUpdateTreeTopModelIdx(const QModelIndex& idx);
    void removeSelectedUpdateItems();
    void clearUpdateViews();
    void readPersistedFilePath();
    void handleStartBehaviour();
    void quitFileSystemManagerThread();
    void updateFreeDiskSpace(const QString& path);
    void centerProgressDialog();
    void setUpdateTreeLabelTxt(const QString& txt);

    // Membervariables
    Persistence m_Persistence;
    FileSystemManager m_FileSystemManager;
    FileSystemModel m_FSModel;
    QTableWidget *m_pTable;
    QFileInfo m_CurFileInfo;
    FlacMetadataManager m_FlacPathToMeta;
    QFileInfoList m_CurFileInfoList;
    QStringList m_CollectionPathHistory;
    QTreeView *m_pTreeUpdateView;
    QComboBox m_ComboBox;
    QStandardItemModel *m_pUpdateModel;
    QModelIndex m_UpdateTreeViewModelIdx;
    QModelIndexList m_UpdateTreeViewModelIdxList;
    StatusBar *m_pStatusBar;
    ConfigDialog m_ConfigDialog;
    QProgressDialog m_ProgressDialog;

    QTreeView* m_pTree;
    QMenu *m_pCollectionCtxMenu;
    QMenu *m_pUpdateTreeCtxMenu;
    QLineEdit *m_LineEditCollection;
    QProgressBar m_FreeDiskSpaceCollectionBar;

    QString m_MusicCollectionPath;
    QString m_FileIncomingPath;
    QString m_ForeignDbPath;

    QStringList m_CopyRequestSrcList;
    QStringList m_CopyRequestDestList;
    bool        m_IsCopyRequested;
    int         m_LastCopyRequestedFileCount;

    bool m_IsFile;
    int m_TableRow;
    int m_TableColoumn;
    bool m_WizzardEnabled;
    bool m_DeepScanEnabled;
    bool m_DbImportToEnabled;
    eMainState m_MainState;
};

#endif // MAINWINDOW_HPP
