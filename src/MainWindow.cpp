//////////////////////////////////////////////////////////////////////
//!\file MainWindow.cpp
//!\brief Main Application.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////


#include "MainWindow.hpp"

#include <QApplication>
#include <QSplashScreen>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QTreeView>
#include <QFileInfo>
#include <QDebug>
#include <QTableWidget>
#include "StatusBar.hpp"
#include "States.hpp"
#include <QComboBox>
#include "DataBase.hpp"
#include "FreeDiskStorage.hpp"
#include "Utils.hpp"
#include "player/Player.hpp"

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    m_Persistence(),
    m_FileSystemManager(),
    m_FSModel(),
    m_pTable(),
    m_CurFileInfo(),
    m_FlacPathToMeta(""),
    m_CurFileInfoList(),
    m_pTreeUpdateView(),
    m_ButtonUpdateView(new StateButton()),
    m_ButtonCollectionView(new StateButton()),
    m_pUpdateModel(),
    m_ConfigDialog(),
    m_ProgressDialog("Please wait...", "Cancel", 0, 0, this),
    m_pTree(),
    m_pUpdateTreeCtxMenu(),
    m_LineEditCollection(),
    m_Player(new Player),
    m_MusicCollectionPath(QDir::homePath()),
    m_FileIncomingPath(QDir::homePath()),
    m_ImportDbPath(""),
    m_CopyRequestSrcList(),
    m_CopyRequestDestList(),
    m_IsCopyRequested(false),
    m_LastCopyRequestedFileCount(0),
    m_IsFile(false),
    m_TableRow(0),
    m_TableColoumn(0),
    m_WizzardEnabled(m_Persistence.getEnableWizard()),
    m_DeepScanEnabled(m_Persistence.getEnableDeepScan()),
    //m_DbImportToEnabled(m_Persistence.getEnableDbImportTo()),
    m_HypnotoadEnabled(m_Persistence.getEnableHypnotoad()),
    m_MainState(states::MAINSTATE_IDLE)
{

    if (qApp->isSessionRestored())
        restoreState();

    // Read persisted Filepath
    readPersistedFilePath();

    // Create and configure GUI
    createMainGui();

    // In order to decouple GUI from Filesystemmanager activities
    // we have to create a seaparate thread.
    createFileSystemManagerThread();

    emit sigMainWindowState(m_MainState);

    // Either we have remembered a valid, last set collection path and do a search,
    // or we prompt a msgbox for the user to set the path.
    handleStartBehaviour();
}


MainWindow::~MainWindow()
{
    // DTOR
    emit sigStopScans();
    quitFileSystemManagerThread();
}

// linux: is called before user logs out!
QString MainWindow::saveState() const
{
    qDebug() << "MainWindow::saveState() ";
    QFile file(MainWindow::getSessionFileName());

    /*
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        for (int row = 0; row < 3; ++row) {
            for (int column = 0; column < 3; ++column)
                out << board[row][column];
        }
    }
    */
    return file.fileName();
}

QString MainWindow::getSessionFileName() const
{
    qDebug() << "MainWindow::getSessionFileName() ";
    return QDir::homePath() + "/.flacShlonz" + qApp->sessionId() + "_"
            + qApp->sessionKey();
}

// linux: Is called on application startup.
void MainWindow::restoreState()
{
    qDebug() << "MainWindow::restoreState()";
    QFile file(getSessionFileName());
    /*
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        for (int row = 0; row < 3; ++row) {
            for (int column = 0; column < 3; ++column) {
                in >> board[row][column];
                if (board[row][column] != Empty)
                    ++turnNumber;
            }
        }
    }
    */
    update();
}

void MainWindow::createMainGui()
{
    qDebug() << "MainWindow::createMainGui";

    // Create Main Window
    setWindowTitle( QString::fromUtf8( "Flacshlonz" ) );
    QWidget *mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);
    QVBoxLayout *mainLayoutVertical = new QVBoxLayout;
    QSplitter *mainSplitterHorizontal = new QSplitter;
    mainSplitterHorizontal->setOrientation(Qt::Horizontal);
    mainLayoutVertical->addWidget(mainSplitterHorizontal);
    mainWidget->setLayout(mainLayoutVertical);

    // Create Menu Bar
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar( menuBar );

    // Sub Menu
    QMenu *shlonzMenu = new QMenu( tr("Shlonz") );
    QMenu *musicCollectionMenu = new QMenu( tr("Music Collection") );
    QMenu *newFilesMenu = new QMenu( tr("New Files") );
    menuBar->addMenu( shlonzMenu );
    menuBar->addMenu( musicCollectionMenu );
    menuBar->addMenu( newFilesMenu );

    // Create action
    QAction *setCollectionPathAction = new QAction( tr( "Set path to music collection" ), shlonzMenu );
    QAction *setFileIncomingPathAction = new QAction( tr( "Set path to new files" ), shlonzMenu );
    QAction *actionImportDbToCollection = new QAction(tr("Import a database file to music collection"), musicCollectionMenu);
    QAction *actionImportDbToIncoming = new QAction(tr("Import a database file to file-incoming"), newFilesMenu);
    QAction *exportDbAs = new QAction(tr("Export music collection to database file"), shlonzMenu);
    QAction *stopScan = new QAction( tr( "&Stop Scan"), shlonzMenu );
    QAction *quitAction = new QAction( QString::fromUtf8( "&Quit" ), shlonzMenu );
    QAction *aboutAction = new QAction( QString::fromUtf8( "&About" ), musicCollectionMenu );

    QObject::connect( setCollectionPathAction, SIGNAL( triggered()), this, SLOT( onSetMusicCollectionPath()));
    QObject::connect( setFileIncomingPathAction, SIGNAL( triggered()), this, SLOT( onSetFileIncomingPath()));
    QObject::connect( actionImportDbToCollection, SIGNAL(triggered()), this, SLOT(slotImportDbToCollection()));
    QObject::connect( actionImportDbToIncoming, SIGNAL(triggered()), this, SLOT(slotImportDbToIncoming()));
    QObject::connect( exportDbAs, SIGNAL(triggered()), this, SLOT(slotExportDbAs()));
    QObject::connect( stopScan, SIGNAL(triggered()), this, SIGNAL(sigStopScans()));
    QObject::connect( quitAction, SIGNAL( triggered()), qApp, SLOT( quit()));

    // Add action
    shlonzMenu->addAction( stopScan );
    shlonzMenu->addAction( exportDbAs );
    musicCollectionMenu->addAction( setCollectionPathAction );
    newFilesMenu->addAction( setFileIncomingPathAction );

    newFilesMenu->addAction( actionImportDbToIncoming );
    musicCollectionMenu->addAction(actionImportDbToCollection);


    // Settings Menu
    QAction *settingsAction = new QAction( tr("Settings"), newFilesMenu );
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(onShowSettingsMenu()));
    shlonzMenu->addAction( settingsAction );
    shlonzMenu->addAction( aboutAction );
    shlonzMenu->addAction( quitAction );

    // About dialog.
    QPixmap *pixmap = new QPixmap( QString::fromUtf8(":/about.png"));
    QSplashScreen *splash = new QSplashScreen(*pixmap);
    Utils utl;
    QString msg = "Built: " + utl.getBuildDate();
    splash->showMessage(QObject::tr(msg.toStdString().c_str()), Qt::AlignRight | Qt::AlignBottom, Qt::white);

    // Connect about-dialogue to action
    QObject::connect( aboutAction, SIGNAL( triggered() ), splash, SLOT( show() ) );

    // Left pane: FileTree of Music Collection
    QSplitter *uvSplitLeft=new QSplitter(this);
    uvSplitLeft->setOrientation(Qt::Vertical);
    m_FSModel.init(QDir::homePath());
    m_pTree = new QTreeView(this);
    m_pTree->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pTree->setModel( &m_FSModel );
    m_pTree->expandToDepth(1);
    m_pTree->hideColumn( 1 );
    m_pTree->hideColumn( 2 );
    m_pTree->hideColumn( 3 );

    // If tree is clicked, setData to table.
    QObject::connect( m_pTree, SIGNAL( clicked( const QModelIndex & )), this, SLOT( onClickedData( const QModelIndex & )));
    QObject::connect( m_pTree, SIGNAL( doubleClicked( QModelIndex )), this, SLOT( onDblClickedData( QModelIndex )));
    // If tree is clicked, show Context Menu.
    QObject::connect( m_pTree, SIGNAL( customContextMenuRequested(QPoint) ), this, SLOT( onCollectionTreeCtxMenu( QPoint ) ) );
    uvSplitLeft->addWidget(m_pTree);

    // Show free disk space progressbar
    if (m_Persistence.getLastOpenCollectionPathEnabled())
        updateFreeDiskSpace(m_Persistence.getLastOpenCollectionPath());
    else
        updateFreeDiskSpace("");

    QLabel* usedSpaceLabel = new QLabel(tr("Used Disk Space: "));
    usedSpaceLabel->setMaximumHeight(15);
    uvSplitLeft->addWidget(usedSpaceLabel);
    m_FreeDiskSpaceCollectionBar.setMaximumHeight(15);
    uvSplitLeft->addWidget(&m_FreeDiskSpaceCollectionBar);

    //Create Multi-State Button for MusicCollectionView
    m_ButtonCollectionView->setMaximumHeight(25);
    connect(m_ButtonCollectionView, SIGNAL(sigButtonClicked(states::eButtonState)), this, SLOT(slotButtonCollectionViewClicked(states::eButtonState)));
    if (m_Persistence.getLastOpenCollectionPathEnabled())
    {
        emit m_ButtonCollectionView->slotSetButtonState(states::BUTTON_UPDATE);
    }
    else
        emit m_ButtonCollectionView->slotSetButtonState(states::BUTTON_SET_PATH);

    uvSplitLeft->addWidget(m_ButtonCollectionView);


    // Add Left Pane
    mainSplitterHorizontal->addWidget(uvSplitLeft);

    // Mid Pane: Table
    m_pTable = new QTableWidget(this);
    m_pTable->setAcceptDrops( false );
    m_pTable->setDragEnabled( false );
    m_pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pTable->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
    m_pTable->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
    mainSplitterHorizontal->addWidget(m_pTable);
    m_pTable->hide();

    // Right Pane.
    QSplitter *uvSplit=new QSplitter(this);
    uvSplit->setOrientation(Qt::Vertical);

    //m_ComboBox = new QComboBox(this);
    m_ComboBox.addItem(tr("Nothing to choose!"));
    m_ComboBox.setMaximumHeight(25);

    uvSplit->addWidget(&m_ComboBox);

    // Tree Update View Top
    m_pUpdateModel = new QStandardItemModel(0,0, this);
    m_pUpdateModel->setHorizontalHeaderLabels(QStringList(tr("File Incomings...")));
    m_pTreeUpdateView = new QTreeView(this);
    m_pTreeUpdateView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pTreeUpdateView->setSelectionMode(QAbstractItemView::ContiguousSelection);
    QObject::connect(m_pTreeUpdateView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTreeUpdateCtxMenu(QPoint)));
    QObject::connect(m_pTreeUpdateView, SIGNAL(clicked(QModelIndex)), this, SLOT(onUpdateTreeClickedIdx(QModelIndex)));
    QObject::connect(m_pTreeUpdateView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onUpdateTreeDblClicked(QModelIndex)));
    uvSplit->addWidget(m_pTreeUpdateView);

    // Create Multi-State Button for Update View
    m_ButtonUpdateView->setMaximumHeight(25);
    QObject::connect(m_ButtonUpdateView, SIGNAL(sigButtonClicked(states::eButtonState)), this, SLOT(slotButtonUpdateViewClicked(states::eButtonState)));
    emit m_ButtonUpdateView->slotSetButtonState(states::BUTTON_SET_PATH);
    uvSplit->addWidget(m_ButtonUpdateView);
    mainSplitterHorizontal->addWidget(uvSplit);

    // Status Bar
    m_pStatusBar = new StatusBar;
    m_pStatusBar->setMinimumHeight(height() / 25);
    m_pStatusBar->setMaximumHeight(height() / 25);
    mainLayoutVertical->addWidget(m_pStatusBar);

    // Context Menu for MusicCollection View
    m_pCollectionCtxMenu = new QMenu(this);
    m_pCollectionCtxMenu->addAction(tr("Go into"), this, SLOT(slotSetItemToCollectionRootPath()));
    m_pCollectionCtxMenu->addAction(tr("Go back"), this, SLOT(slotGoToLastCollectionRootPath()));
    m_pCollectionCtxMenu->addAction(tr("Create Directory"), this, SLOT(slotCreateDirInCollection()));
    m_pCollectionCtxMenu->addAction(tr("Rename Directory"), this, SLOT(slotRenameDirInCollection()));
    m_pCollectionCtxMenu->addAction(tr("Remove"), this, SLOT(slotRemoveFromCollection()));
    m_pCollectionCtxMenu->hide();

    createUpdateCtxMenu();

    // Progress Dialog
    m_ProgressDialog.setWindowModality(Qt::WindowModal);
    m_ProgressDialog.hide();
    connect(&m_ProgressDialog, SIGNAL(canceled()), &m_FileSystemManager, SIGNAL(sigStopCopy()));

    connect(this, SIGNAL(sigRescanAll()), this, SLOT(slotRescanAll()));
    connect(&m_ConfigDialog, SIGNAL(sigEnableWizard(bool)), this, SLOT(slotEnableWizard(bool)));
    //connect(&m_ConfigDialog, SIGNAL(sigEnableDbImportTo(bool)), this, SLOT(slotEnableDbImportTo(bool)));
    connect(&m_ConfigDialog, SIGNAL(sigEnableHypnotoad(bool)), this, SLOT(slotEnableHypnotoad(bool)));
    //connect(&m_ConfigDialog, SIGNAL(sigEnableDeepScan(bool)), this, SLOT(slotEnableDeepScan(bool)));
    connect(this, SIGNAL(sigRunSelectCollectionPathMsgBox()), this, SLOT(slotRunSelectCollectionPathMsgBox()));

    setUpdateTreeLabelTxt(tr("Please select an incoming path (right click)!"));
}

// Decouples GUI from FS Manager activities.
void MainWindow::createFileSystemManagerThread()
{
    // Create a thread for tts-wrapper.
    // QThread* pFileSystemManagerThread = new QThread(this);
    // pFileSystemManagerThread->setObjectName("pFlacSearchThread");

    // Put it into, which is only possible, because of deriving from QObject.
    // m_FileSystemManager.moveToThread(pFileSystemManagerThread);

    // Connect signal to slot.
    connect(this, SIGNAL(sigMainWindowState(states::eMainState)), &m_FileSystemManager, SLOT(slotMainWindowState(states::eMainState)));
    connect(this, SIGNAL(sigSearchInMusicCollection(QString)), &m_FileSystemManager, SLOT(slotSearchInMusicCollection(QString)) );
    connect(this, SIGNAL(sigSearchInFileIncoming(QString)), &m_FileSystemManager, SLOT(slotSearchInFileIncoming(QString)));
    connect(this, SIGNAL(sigCopyAllFolderTo(QStringList,QStringList)), &m_FileSystemManager, SLOT(slotCopyAllFolderTo(QStringList,QStringList)));
    connect(this, SIGNAL(sigResetCopyProgressCounter()), &m_FileSystemManager, SLOT(slotResetCopyProgressCounter()));
    connect(this, SIGNAL(sigResetRemoveProgressCounter()), &m_FileSystemManager, SLOT(slotResetRemoveProgressCounter()));
    connect(this, SIGNAL(sigRemoveFromFileSystem(QStringList)), &m_FileSystemManager, SLOT(slotRemoveFromFileSystem(QStringList)));
    connect(this, SIGNAL(sigEnableDeepScan(bool)), &m_FileSystemManager, SLOT(slotEnableDeepScan(bool)));
    connect(this, SIGNAL(sigStopScans()), &m_FileSystemManager, SIGNAL(sigStopScanner()));
    connect(this, SIGNAL(sigStopScans()), &m_FileSystemManager, SIGNAL(sigStopCopy()));
    connect(this, SIGNAL(sigImportDbToCollection(QString)), &m_FileSystemManager, SLOT(slotImportDbToCollection(QString)));
    connect(this, SIGNAL(sigImportDbToIncoming(QString)), &m_FileSystemManager, SLOT(slotImportDbToIncoming(QString)));


    // In order to use slots and signals with parameters, we have to use qRegisterMetaType.
    //typedef QVector<Album> tAlbumVec;
    //qRegisterMetaType< Album > ( "Album" );
    //qRegisterMetaType< QVector< Album > > ( "QVector<Album>" );


    //connect(&m_FileSystemManager, SIGNAL(sigFlacFound(QString)), this, SLOT(slotFlacFound(QString)));
    //connect(&m_FileSystemManager, SIGNAL(sigFolderFound(Album)), this, SLOT(slotFlacFolderFound(Album)));
    connect(&m_FileSystemManager, SIGNAL(sigAlbumEntryCollectionFound(QString,QString,QString,QString,QString)), this, SLOT(slotAlbumEntryCollectionFound(QString,QString,QString,QString,QString)));
    connect(&m_FileSystemManager, SIGNAL(sigAlbumEntryFileIncomingFound(QString,QString,QString,QString,QString)), this, SLOT(slotAlbumEntryFileIncomingFound(QString,QString,QString,QString,QString)));
    connect(&m_FileSystemManager, SIGNAL(sigImportDbEntryFound(QString,QString,QString,QString,QString)), this, SLOT(slotAlbumEntryFileIncomingFound(QString,QString,QString,QString,QString)));
    connect(&m_FileSystemManager, SIGNAL(sigSearchInMusicCollectionFinished()), this, SLOT(slotSearchInMusicCollectionFinished()));
    connect(&m_FileSystemManager, SIGNAL(sigSearchInFileIncomingFinished()), this, SLOT(slotSearchInFileIncomingFinished()));
    connect(&m_FileSystemManager, SIGNAL(sigDataBaseImportFinished()), this, SLOT(slotDbImportFinished()));
    //connect(&m_FileSystemManager, SIGNAL(sigFlacListComparisonFinished()), this, SLOT(slotFlacListComparisonFinished()));
    connect(&m_FileSystemManager, SIGNAL(sigCopyProgress(int,QString)), this, SLOT(slotCopyProgress(int,QString)));
    connect(&m_FileSystemManager, SIGNAL(sigCopyProgressFinished(bool)), this, SLOT(slotCopyProgressFinished(bool)));
    connect(&m_FileSystemManager, SIGNAL(sigRemoveProgress(int, QString)), this, SLOT(slotRemoveProgress(int, QString)));
    connect(&m_FileSystemManager, SIGNAL(sigRemoveRunFinished(bool)), this, SLOT(slotRemoveProgressFinished(bool)));

    // Do we need this line?
    // connect(m_FlacSearcher.thread(), SIGNAL(finished()), m_FlacSearcher.thread(), SLOT(deleteLater()));

    // Start thread.
    //pFileSystemManagerThread->start();
}

void MainWindow::createComboBox()
{
    m_ComboBox.clear();

    if ( m_DeepScanEnabled )
    {
        m_ComboBox.addItem("Tracks you don´t have");
        m_ComboBox.addItem("Tracks you own, but with different metadata");
    }
    m_ComboBox.addItem("Tracks you maybe have, with different filename");
    m_ComboBox.addItem("Tracks you maybe have, but in a different named folder");
    connect(&m_ComboBox, SIGNAL(activated(int)), this, SLOT(slotSelectFromComboBox(int)));
}

void MainWindow::createUpdateCtxMenu()
{
    qDebug() << "MainWindow::createUpdateCtxMenu";

    // Context Menu for Update Tree view top
    m_pUpdateTreeCtxMenu = new QMenu(this);
    m_pUpdateTreeCtxMenu->addAction(tr("Import to music collection."), this, SLOT(slotCopyToMusicCollection()));
    m_pUpdateTreeCtxMenu->addAction(tr("Copy to..."), this, SLOT(onCtxTopMenuCopyTo()));
    m_pUpdateTreeCtxMenu->hide();

}

bool MainWindow::runMsgBoxRemoveSelection()
{
    bool retVal = false;

    // Message Box
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Question);
    msgBox->setText("Do you really want to remove your selection?");
    //msgBox->setInformativeText((m_fileInfo.absoluteFilePath() + "...");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::No);

    int ret = msgBox->exec();
    switch (ret)
    {
    case QMessageBox::Yes:
        qDebug() << "Yes was clicked!";
        retVal = true;
        break;
    case QMessageBox::No:
        retVal = false;
        break;
    default:
        qDebug() << ret << " was clicked... this should never been happen!";
        // should never be reached
        break;
    }

    return retVal;
}

bool MainWindow::runMsgBoxFinishedFlacSearchCollection()
{
    bool retVal = false;

    m_pStatusBar->setStatusBarTxt("SUCCESS! Your FLAC Music-Collection was tracked. Now, please scan your Incoming-File Folder for new FLAC-Files.");
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Question);
    msgBox->setText("SUCCESS! Your FLAC Music-Collection was tracked. Now, please scan your Incoming-File Folder for new FLAC-Files.");
    msgBox->setInformativeText("Would you like to set your Incoming-File-Path, now?");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::Yes);

    int ret = msgBox->exec();
    switch (ret)
    {
    case QMessageBox::Yes:
        qDebug() << "Yes was clicked!";
        retVal = true;
        emit onSetFileIncomingPath();
        break;
    case QMessageBox::No:
        qDebug() << "No was clicked!";
        retVal = false;
        break;
    default:
        qDebug() << ret << " was clicked... this should never been happen!";
        // should never be reached
        break;
    }
    return retVal;
}

bool MainWindow::runMsgBoxSelectCollectionPath()
{
    bool retVal = false;

    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Question);
    msgBox->setText("Salve!"); //("Would you like to select the path to your Music-Collection?");
    msgBox->setInformativeText("Would you like to select the path to your Music-Collection, now?");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::Yes);

    int ret = msgBox->exec();
    switch (ret)
    {
    case QMessageBox::Yes:
        qDebug() << "Yes was clicked!";
        retVal = true;
        emit onSetMusicCollectionPath();
        break;
    case QMessageBox::No:
        qDebug() << "No was clicked!";
        retVal = false;
        break;
    default:
        qDebug() << ret << " was clicked... this should never been happen!";
        // should never be reached
        break;
    }
    return retVal;
}

bool MainWindow::runMsgBoxFolderExistsOrCopy()
{
    bool retVal = false;

    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Question);
    msgBox->setText("Folder(s) with same names already exist:\n");
    msgBox->setInformativeText("Do you really want to replace these files?");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::Yes);

    int ret = msgBox->exec();
    switch (ret)
    {
    case QMessageBox::Yes:
        qDebug() << "Yes was clicked!";
        retVal = true;

        break;
    case QMessageBox::No:
        qDebug() << "No was clicked!";
        retVal = false;
        break;
    default:
        qDebug() << ret << " was clicked... this should never been happen!";
        // should never be reached
        break;
    }
    return retVal;
}

bool MainWindow::runMsgBoxIncomingPathContainedCollectionPath()
{
    qDebug() << "MainWindow::runMsgBoxIncomingPathContainedCollectionPath";
    bool retVal = false;

    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setText("The File incoming path you have chosen is already part of your collection.\n");
    msgBox->setInformativeText("Do you really want to continue?");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::Yes);

    int ret = msgBox->exec();
    switch (ret)
    {
    case QMessageBox::Yes:
        qDebug() << "Yes was clicked!";
        retVal = true;

        break;
    case QMessageBox::No:
        qDebug() << "No was clicked!";
        retVal = false;
        break;
    default:
        qDebug() << ret << " was clicked... this should never been happen!";
        // should never be reached
        break;
    }
    return retVal;

}

void MainWindow::setTableLabels(int count)
{
    qDebug() << "MainWindow::setTableLabels " << count;
    m_pTable->show();

    m_TableRow=0;
    m_TableColoumn=0;

    m_CurFileInfoList.clear();

    // Set table labels.
    if(  m_CurFileInfo.isFile() )
    {
        m_CurFileInfoList.append( m_CurFileInfo );
        m_pTable->setColumnCount( 1 );
        m_pTable->setRowCount( count );
        QStringList labels;
        labels << tr("Information of selected file");
        m_pTable->setHorizontalHeaderLabels( labels );
        m_pTable->setVerticalHeaderLabels( m_FlacPathToMeta.m_metadataKeys.toList() );
    }
    else
    {
        QDir dir(  m_CurFileInfo.filePath());
        m_CurFileInfoList = dir.entryInfoList
                (
                    FileSystemModel::s_FileFilter,
                    QDir::Files,
                    QDir::Name
                    );

        m_pTable->setColumnCount( count );
        m_pTable->setRowCount( m_CurFileInfoList.size() );
        m_pTable->setHorizontalHeaderLabels( m_FlacPathToMeta.m_metadataKeys.toList() );
    }
}

void MainWindow::setTreeViewRoot(const QString& path)
{
    qDebug() << "MainWindow::setTreeViewRoot " << path;
    m_FSModel.setHeaderTxt("Your music collection\n" + path);
    m_pTree->setRootIndex(m_FSModel.index(path));
}

void MainWindow::fillMetavalueTable()
{
    qDebug() << "MainWindow::fillMetavalueTable";

    m_IsFile =  m_CurFileInfo.isFile();

    // Flush table on screen.
    m_pTable->clear();

    // Create Metadata Container
    //QVector< QString > metadataValues( m_FlacPathToMeta.m_metadataKeys.size(), "" );

    // Create dimensions.
    setTableLabels(m_FlacPathToMeta.m_metadataKeys.size());

    // Fill with table values and paint to table.
    traverseFileInfoList();

    // Resize
    m_pTable->resizeColumnsToContents();
    m_pTable->resizeRowsToContents();

    QHeaderView *headerView = m_pTable->horizontalHeader();
    headerView->setStretchLastSection( true );

}

// Be shure to check if table or couloumn row must be reset beforehand!
void MainWindow::putFlacMetaDataToTable(const QVector<QString> & metadataValues, bool isFile)
{
    //qDebug() << "MainWindow::putFlacMetaDataToTable";

    // Paint values to table.
    for( int i = 0; i < metadataValues.size(); i++ )
    {
        QTableWidgetItem *newItem = new QTableWidgetItem( metadataValues.at( i ) );
        m_pTable->setItem( m_TableRow, m_TableColoumn, newItem );

        if( isFile )
        {
            m_TableRow++;
        }
        else
        {
            m_TableColoumn++;
        }
    }
}

void MainWindow::onSetMusicCollectionPath()
{
    m_MusicCollectionPath = QFileDialog::getExistingDirectory(this, tr("Choose music collection"), m_MusicCollectionPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    //qDebug() << "Your Music Collection Folder is: " << m_MusicCollectionPath << "\n";

    if (m_MusicCollectionPath.isEmpty())
    {
        qDebug() << "Path is empty";
    }
    else
    {
        m_MainState = states::MAINSTATE_FILE_SCAN;
        emit sigMainWindowState(m_MainState);
        emit m_ButtonCollectionView->slotSetButtonState(states::BUTTON_STOP);

        m_pStatusBar->setStatusBarTxt("Your Music Collection Folder is: " + m_MusicCollectionPath );
        m_Persistence.persistLastOpenCollectionPath(m_MusicCollectionPath);
        updateFreeDiskSpace(m_MusicCollectionPath);
        // Reset Tree-View
        setTreeViewRoot(m_MusicCollectionPath);
        emit sigSearchInMusicCollection(m_MusicCollectionPath);
        startPlayer();
    }
}

void MainWindow::onSetFileIncomingPath()
{
    // Clear old results.
    clearUpdateViews();
    m_ButtonUpdateView->setEnabled(true);

    m_FileIncomingPath = QFileDialog::getExistingDirectory(this, tr("Choose incoming files"), m_FileIncomingPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    // Check if file-incoming is part of collection
    // and give user a hint.
    if ( m_FileIncomingPath.contains(m_MusicCollectionPath) )
    {
        if (!runMsgBoxIncomingPathContainedCollectionPath())
            return;
    }

    qDebug() << "Your Incoming Files Folder is: " << m_FileIncomingPath << "\n";

    if (m_FileIncomingPath.isEmpty())
    {
        qDebug() << "Path is empty";
        m_pStatusBar->setStatusBarTxt("Invalid Path!");
        setUpdateTreeLabelTxt(tr( "Please try to set a valid file incoming path (right-click)!" ));
        emit m_ButtonUpdateView->slotSetButtonState(states::BUTTON_SET_PATH);
    }
    else
    {
        emit sigSearchInFileIncoming(m_FileIncomingPath); // Will send finished signal!

        // Set new MainState
        m_MainState = states::MAINSTATE_FILE_SCAN;
        emit m_ButtonUpdateView->slotSetButtonState(states::BUTTON_STOP);
        emit sigMainWindowState(m_MainState);

        setUpdateTreeLabelTxt(tr("File Scan Mode!"));
        m_pStatusBar->setStatusBarTxt("Your incoming files folder is: " + m_FileIncomingPath );
        startPlayer();
    }
}

void MainWindow::onShowSettingsMenu()
{
    qDebug() << "MainWindow::onShowSettingsMenu";
    m_ConfigDialog.show();
}

void MainWindow::onShowDiffList(const QStringList &list)
{
    qDebug() << "MainWindow::onShowDiffList";
    clearUpdateViews();

    // Set header label text.

    if (states::MAINSTATE_DB_IMPORT == m_MainState)
    {
    //    item = new QStandardItem(foundPath);
        m_pUpdateModel->setHorizontalHeaderLabels(QStringList("Database Import: " + Utils::getBaseFromPath(m_ImportDbPath)));
    }
    else if ( states::MAINSTATE_FILE_SCAN == m_MainState )
    {
        m_pUpdateModel->setHorizontalHeaderLabels(QStringList(m_FileIncomingPath));
    }

    // Print if no new files were found!
    if (list.empty())
    {
        m_pStatusBar->setStatusBarTxt("No FLAC files for update found!");
        return;
    }
    else
    {
        m_pStatusBar->setStatusBarTxt("FLAC files found.");

        // Take the first value and remember.
        QStandardItem *parentItem = m_pUpdateModel->invisibleRootItem();
        QStandardItem *item;
        for( QStringList::const_iterator uit = list.begin(); uit != list.end(); ++uit )
        {
            QString foundPath = (*uit);
            if ( !(*uit).endsWith(".flac"))
            {
                parentItem = m_pUpdateModel->invisibleRootItem();

                // critical?
                if (states::MAINSTATE_DB_IMPORT == m_MainState)
                {
                    item = new QStandardItem(foundPath);
                    //m_pUpdateModel->setHorizontalHeaderLabels(QStringList("Database Import: " + Utils::getBaseFromPath(m_ImportDbPath)));
                }
                else if ( states::MAINSTATE_FILE_SCAN == m_MainState )
                {
                    // Crop off FileincomingPath, because its part of the header.
                    if (foundPath.contains(m_FileIncomingPath))
                    {
                        item = new QStandardItem(foundPath.remove(0,m_FileIncomingPath.length()));
                    }
                }
                parentItem->appendRow(item);
                parentItem=item;
            }
            else
            {
                item = new QStandardItem(*uit);
                parentItem->appendRow(item);
            }
        }
        //m_UpdateModel->setData(m_UpdateModel->index(0, 0), Qt::blue, Qt::BackgroundRole);
        m_pTreeUpdateView->setModel(m_pUpdateModel);
        //m_pTreeUpdateViewBottom->setModel(m_pUpdateModelBottom);
    }
    m_pTreeUpdateView->setModel(m_pUpdateModel);
}


// Scans and fills m_table with flac metadata information.
void MainWindow::onClickedData( const QModelIndex & index )
{
    qDebug() << "MainWindow::onClickedData";
    m_CurFileInfo = QFileInfo(m_FSModel.fileInfo(index));
    fillMetavalueTable();
}

void MainWindow::onDblClickedData( const QModelIndex & index )
{
    qDebug() << "MainWindow::onDblClickedData";

    m_CurFileInfo = QFileInfo(m_FSModel.fileInfo(index));
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_CurFileInfo.absoluteFilePath()));
    fillMetavalueTable();
}

void MainWindow::onCollectionTreeCtxMenu( QPoint )
{
    qDebug() << "MainWindow::onCollectionTreeCtxMenu";
    m_pCollectionCtxMenu->show();
    m_pCollectionCtxMenu->exec(QCursor::pos());
}

void MainWindow::onUpdateTreeClickedIdx(QModelIndex idx)
{
    qDebug() << "MainWindow::onUpdateTreeClickedIdx" << idx.column() << idx.row();
    //m_UpdateModel->setData(m_UpdateModel->index(0, 0), Qt::blue, Qt::BackgroundRole);
    m_CurFileInfo = getFileInfoByUpdateTreeTopModelIdx(idx);

    fillMetavalueTable();
}

// Will open folders or files by OS set application.
void MainWindow::onUpdateTreeDblClicked(QModelIndex idx)
{

    m_CurFileInfo = getFileInfoByUpdateTreeTopModelIdx(idx);

    // EXP
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_CurFileInfo.absoluteFilePath()));
}

void MainWindow::onTreeUpdateCtxMenu(QPoint)
{
    qDebug() << "MainWindow::onTreeUpdateCtxMenu(QPoint)";
    if (m_MainState == states::MAINSTATE_FILE_SCAN)
    {
        m_pUpdateTreeCtxMenu->show();
        m_pUpdateTreeCtxMenu->exec(QCursor::pos());
    }
    else if (m_MainState != states::MAINSTATE_DB_IMPORT) // do not search in db import mode.
    {
        emit onSetFileIncomingPath();
    }
}

void MainWindow::onCtxTopMenuCopyTo()
{
    qDebug() << "MainWindow::onCtxTopMenuCopyTo";
    // Choose copy location via dialogue.
    QString copyDest = QFileDialog::getExistingDirectory(this, tr("Please, choose the place where your files should be copied to!"), m_FileIncomingPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    qDebug() << "Destination to copy to: " << copyDest;

    if (!copyDest.isEmpty())
        if (QDir(copyDest).exists())
            copySelectionFromUpdateCollectionTop(copyDest);

}

void MainWindow::slotAlbumEntryCollectionFound(QString /*id*/, QString albumName, QString /*trackName*/, QString /*audioMd5*/, QString /*metaMd5*/)
{
    // qDebug() << "slotAlbumEntryFound";
    m_pStatusBar->setStatusBarTxt(albumName);
}

void MainWindow::slotAlbumEntryFileIncomingFound(QString /*id*/, QString albumName, QString /*trackName*/, QString /*audioMd5*/, QString /*metaMd5*/)
{
    //qDebug() << "MainWindow::slotAlbumEntryFileIncomingFound";
    m_pStatusBar->setStatusBarTxt(albumName);
}

void MainWindow::slotSearchInMusicCollectionFinished()
{
    qDebug() << "MainWindow::slotSearchInMusicCollectionFinished";
    m_pStatusBar->setStatusBarTxt("Searching in Music-Collection successfully finished.");
    stopPlayer();
    emit m_ButtonCollectionView->slotSetButtonState(states::BUTTON_UPDATE);

    if (m_WizzardEnabled)
    {
        if ( !runMsgBoxFinishedFlacSearchCollection() )
        {
            return;
        }
    }
}
void MainWindow::slotSearchInFileIncomingFinished()
{
    qDebug() << "MainWindow::slotSearchInFileIncomingFinished";
    m_pStatusBar->setStatusBarTxt("Searching in File-Incoming Folder successfully finished.");
    m_WizzardEnabled = false;
    stopPlayer();
    createComboBox();

    // Fill Combobox, with relevant values
    if (!m_FileSystemManager.getDiffListAudioMd5().isEmpty() || !m_FileSystemManager.getDiffListFileNames().isEmpty() || !m_FileSystemManager.getDiffListAlbumName().isEmpty() || !m_FileSystemManager.getDiffListMetaMd5().isEmpty())
    {
        emit onShowDiffList(m_FileSystemManager.getDiffListAudioMd5());
        emit m_ButtonUpdateView->slotSetButtonState(states::BUTTON_UPDATE);
    }
    else
    {
        m_pStatusBar->setStatusBarTxt(tr("No Update Content found!"));
        setUpdateTreeLabelTxt(m_FileIncomingPath);
        emit m_ButtonUpdateView->slotSetButtonState(states::BUTTON_SET_PATH);
    }
}

void MainWindow::slotDbImportFinished()
{
    qDebug() << "MainWindow::slotDbImportFinished";
    clearUpdateViews();
    stopPlayer();

    // Fill Combobox, with relevant values
    if (!m_FileSystemManager.getDiffListAudioMd5().isEmpty() || !m_FileSystemManager.getDiffListFileNames().isEmpty() || !m_FileSystemManager.getDiffListAlbumName().isEmpty() || !m_FileSystemManager.getDiffListMetaMd5().isEmpty())
    {
        m_pStatusBar->setStatusBarTxt("Import of database successfully finished.");
        createComboBox();
        m_pUpdateModel->setHorizontalHeaderLabels(QStringList("Content of imported database:"));
        emit onShowDiffList(m_FileSystemManager.getDiffListAudioMd5());
        emit m_ButtonUpdateView->slotSetButtonState(states::BUTTON_UPDATE);
        m_ButtonUpdateView->setEnabled(false);
    }
    else
    {
        //setUpdateTreeLabelTxt(tr("No Update Content found!"));
        m_pStatusBar->setStatusBarTxt(tr("No Update Content found!"));

        emit m_ButtonUpdateView->slotSetButtonState(states::BUTTON_SET_PATH);
    }

}

void MainWindow::slotCopyToMusicCollection()
{
    qDebug() << "MainWindow::slotCopyToMusicCollection";

    m_ProgressDialog.show();
    if (m_pStatusBar!=NULL)
        m_pStatusBar->setStatusBarTxt("Starting to copy...");
    copySelectionFromUpdateCollectionTop("");

}

void MainWindow::slotCopyProgress(int cnt, QString path)
{

    if (cnt > -1)
    {
        m_ProgressDialog.setValue(cnt);
        QString tmp = "Copying: " + path;// + " " + QString::number(cnt);
        m_ProgressDialog.setLabelText(tmp);
        if (NULL!=m_pStatusBar)
            m_pStatusBar->setStatusBarTxt( tmp );
    }

    if (cnt == -1)
    {
        m_ProgressDialog.hide();
        m_pStatusBar->setStatusBarTxt("Nothing was copied!");
        // TODO: Introduce states in threadCopyFiles class to handle copy issues
        // by a message box.
        //QMessageBox *box = new QMessageBox;
        //box->setText("Nothing was copied!");
        //box->show();
    }

    centerProgressDialog();
}

void MainWindow::slotCopyProgressFinished(bool wasSuccessful)
{

    m_ProgressDialog.hide();
    if (wasSuccessful)
        m_pStatusBar->setStatusBarTxt("Copy successfully finished!");
    else
        m_pStatusBar->setStatusBarTxt(" Something went wrong copying files!");

    // TODO: make it cool
    //removeSelectedUpdateItems();

    qDebug() << "MainWindow::slotCopyProgress: CopyFinished";
    updateFreeDiskSpace(m_MusicCollectionPath);

    setTreeViewRoot(m_MusicCollectionPath);
    //clearUpdateViews();
    emit sigStopScans();
    emit sigSearchInMusicCollection(m_MusicCollectionPath);
    emit sigSearchInFileIncoming(m_FileIncomingPath);
}

void MainWindow::slotRemoveProgress(int cnt, QString path)
{

    if (cnt > -1)
    {
        m_ProgressDialog.setValue(cnt);
        QString tmp = "Removing: " + path + " " + QString::number(cnt);// + " / " + QString::number(m_pStatusBar->getProgressBarMax());
        m_pStatusBar->setStatusBarTxt( tmp );
    }

    if (cnt == -1)
    {
        m_ProgressDialog.hide();
        m_pStatusBar->setStatusBarTxt("Nothing was removed!");
    }
}

void MainWindow::slotRemoveProgressFinished(bool wasSuccessful)
{
    qDebug() << " MainWindow::slotRemoveProgressFinished: " << wasSuccessful;
    m_ProgressDialog.hide();
    if(wasSuccessful)
        m_pStatusBar->setStatusBarTxt("All files were successfully removed.");
    else
        m_pStatusBar->setStatusBarTxt("Something went wrong, while removing files!");

    // Copy Overwrite was requested
    if (m_IsCopyRequested)
    {
        m_IsCopyRequested=false;
        emit sigResetCopyProgressCounter();
        m_ProgressDialog.setMaximum(m_LastCopyRequestedFileCount);
        m_LastCopyRequestedFileCount=0;
        //m_pStatusBar->setProgressBarMax(fileCount);
        m_ProgressDialog.show();
        emit sigCopyAllFolderTo(m_CopyRequestSrcList, m_CopyRequestDestList);
    }
    else
    {
        // TODO: Make this cool!
        //removeSelectedUpdateItems();
        emit sigStopScans();
        emit sigSearchInMusicCollection(m_MusicCollectionPath);
        emit sigSearchInFileIncoming(m_FileIncomingPath);
    }
    updateFreeDiskSpace(m_MusicCollectionPath);
}

void MainWindow::slotRemoveFromCollection()
{
    qDebug() << "MainWindow::slotRemoveFromCollection" << m_CurFileInfo.absoluteFilePath();

    // Never ever remove a file, while searching
    // in filesystem!
    emit sigStopScans();

    if (runMsgBoxRemoveSelection())
    {
        // Remove selection
        removeSelectionFromCollection();
    }
}

void MainWindow::slotRescanAll()
{
    m_pStatusBar->setStatusBarTxt("Rescanning...");
    emit sigSearchInMusicCollection(m_MusicCollectionPath);
    //Approach fails! emit sigSearchInFileIncoming(m_FileIncomingPath);
    m_pStatusBar->setStatusBarTxt("Rescan finished.");
}

void MainWindow::slotSetItemToCollectionRootPath()
{
    qDebug() << "MainWindow::slotSetItemToCollectionRootPath";

    // Remember!
    m_CollectionPathHistory.push_back(m_MusicCollectionPath);
    // Get path from file info
    m_MusicCollectionPath = QDir(m_CurFileInfo.absoluteFilePath()).absolutePath();
    // Persist.
    m_Persistence.persistLastOpenCollectionPath(m_MusicCollectionPath);

    // Reset Tree-View
    setTreeViewRoot(m_MusicCollectionPath);
    updateFreeDiskSpace(m_MusicCollectionPath);
    emit sigStopScans();
    emit sigSearchInMusicCollection(m_MusicCollectionPath);
    startPlayer();
    emit m_ButtonCollectionView->slotSetButtonState(states::BUTTON_STOP);
}

void MainWindow::slotGoToLastCollectionRootPath()
{
    qDebug() << "MainWindow::slotGoToLastCollectionRootPath";

    if (m_CollectionPathHistory.isEmpty())
        return;

    QString lastPath = m_CollectionPathHistory.last();
    if (lastPath.isEmpty())
        return;
    m_CollectionPathHistory.removeLast();

    if (m_MusicCollectionPath == lastPath)
        return;

    m_MusicCollectionPath = lastPath;
    m_Persistence.persistLastOpenCollectionPath(m_MusicCollectionPath);
    setTreeViewRoot(m_MusicCollectionPath);
    updateFreeDiskSpace(m_MusicCollectionPath);
    emit sigStopScans();
    emit sigSearchInMusicCollection(m_MusicCollectionPath);
    startPlayer();
    emit m_ButtonCollectionView->slotSetButtonState(states::BUTTON_STOP);
}

void MainWindow::slotCreateDirInCollection()
{
    qDebug() << "MainWindow::slotCreateDirInCollection";
    QDir dir;
    dir.mkdir(m_MusicCollectionPath + "//NewFolder");
}

void MainWindow::slotRenameDirInCollection()
{
    qDebug() << "MainWindow::slotRenameDirInCollection";
    m_LineEditCollection=new QLineEdit("Set Directory Name here...");
    connect(m_LineEditCollection,SIGNAL(returnPressed()), this, SLOT(slotRenamedDirPressedEnter()));
    m_LineEditCollection->show();
}

void MainWindow::slotRenamedDirPressedEnter()
{
    // Get first value of selection
    QString oldPath = getSelectionListFromCollectionTreeView().first().absoluteFilePath();
    QString tmp = Utils::getBaseFromPath(oldPath);
    QString newPath = oldPath.left(oldPath.length() - tmp.length()) + m_LineEditCollection->text();
    if (QDir(oldPath).exists() && !m_LineEditCollection->text().isEmpty() && (m_LineEditCollection->text() != "Set Directory Name..."))
    {
        QDir(oldPath).rename(oldPath, newPath);
    }
    qDebug() << "MainWindow::slotRenamedDirPressedEnter " << oldPath << " " <<  newPath;
    delete m_LineEditCollection;
}

void MainWindow::slotEnableWizard(bool enable)
{
    qDebug() << "MainWindow::slotEnableWizard " << enable;
    m_WizzardEnabled = enable;
}
/*
void MainWindow::slotEnableDbImportTo(bool enable)
{
    qDebug() << "MainWindow::slotEnableDbImportTo " << enable;
    m_DbImportToEnabled = enable;
}
*/
void MainWindow::slotEnableHypnotoad(bool enable)
{
    qDebug() << "MainWindow::slotEnableHypnotoad" << enable;
    m_HypnotoadEnabled = enable;
}

void MainWindow::slotEnableDeepScan(bool enable)
{
    qDebug() << "MainWindow::slotEnableDeepScan " << enable;
    m_DeepScanEnabled = enable;
    /*
    if (enable)
    {
        m_pTreeUpdateViewBottom->show();
    }
    else
    {
        m_pTreeUpdateViewBottom->hide();
    }
    */
    emit sigEnableDeepScan(enable);
}

void MainWindow::slotRunSelectCollectionPathMsgBox()
{
    runMsgBoxSelectCollectionPath();
}

void MainWindow::slotButtonUpdateViewClicked(states::eButtonState state)
{

    clearUpdateViews();

    if (state == states::BUTTON_UPDATE)
    {
        qDebug() << "Update Update Button Clicked: states::BUTTON_UPDATE";
        if (!m_FileIncomingPath.isEmpty())
        {
            emit sigSearchInFileIncoming(m_FileIncomingPath);
            emit m_ButtonUpdateView->slotSetButtonState(states::BUTTON_STOP);
            startPlayer();
        }
    }
    else if (state == states::BUTTON_STOP)
    {
        qDebug() << "Update Stop Button Clicked: states::BUTTON_STOP";
        stopPlayer();
        emit sigStopScans();
        emit m_ButtonUpdateView->slotSetButtonState(states::BUTTON_UPDATE);
    }
    else if (state == states::BUTTON_SET_PATH )
    {
        emit onSetFileIncomingPath();
    }
    else
    {
        qDebug() << "Rescan Button Clicked: states::BUTTON_SOMETHING!";
    }
}

void MainWindow::slotButtonCollectionViewClicked(states::eButtonState state)
{
    if (state == states::BUTTON_UPDATE)
    {
        qDebug() << "Collectoin Button Clicked: states::BUTTON_UPDATE";
        if (!m_MusicCollectionPath.isEmpty())
        {
            emit sigSearchInMusicCollection(m_MusicCollectionPath);
            emit m_ButtonCollectionView->slotSetButtonState(states::BUTTON_STOP);
            startPlayer();
        }
    }
    else if (state == states::BUTTON_STOP)
    {
        qDebug() << "Collection Button Clicked: states::BUTTON_STOP";
        stopPlayer();
        emit sigStopScans();
        emit m_ButtonCollectionView->slotSetButtonState(states::BUTTON_UPDATE);
    }
    else if (state == states::BUTTON_SET_PATH )
    {
        emit onSetMusicCollectionPath();
    }
    else
    {
        qDebug() << "Collection Button Clicked: states::BUTTON_SOMETHING!";
    }
}

void MainWindow::slotSelectFromComboBox(int selection)
{
    if(m_DeepScanEnabled)
    {
        switch(selection)
        {
        case 0:
            emit onShowDiffList(m_FileSystemManager.getDiffListAudioMd5());
            break;
        case 1:
            emit onShowDiffList(m_FileSystemManager.getDiffListMetaMd5());
            break;
        case 2:
            emit onShowDiffList(m_FileSystemManager.getDiffListFileNames());
            break;
        case 3:
            emit onShowDiffList(m_FileSystemManager.getDiffListAlbumName());
            break;
        default:
            break;
        }
    }
    else
    {
        switch(selection)
        {
        case 0:
            emit onShowDiffList(m_FileSystemManager.getDiffListFileNames());
            break;
        case 1:
            emit onShowDiffList(m_FileSystemManager.getDiffListAlbumName());
            break;
        default:
            break;
        }
    }
}

void MainWindow::slotImportDbToCollection()
{
    qDebug() << "MainWindow::slotImportDbToCollection";
    if (states::FM_DB_IMPORT_INCOMING == m_FileSystemManager.getSelfState())
    {
        qDebug() << "CRASH IGNORED!";
        m_pStatusBar->setStatusBarTxt("Action ignored. Try to scan for new files at first!");
        return;
    }

    QString dbPath = QFileDialog::getOpenFileName(this, tr("Choose database file to import to your music collection!"), QDir::rootPath());

    // Avoid crash, when loading the same database twice
    //  if (dbPath != m_ForeignDbPath)
    //  {
    m_ImportDbPath = dbPath;
    if (QFile(m_ImportDbPath).exists())
    {
        // Avoid loading the database, in which we want to write to.
        // PS: There are some idiots outta there!
        if (m_ImportDbPath.contains(DataBase::s_DatabaseName))
        {
            m_pStatusBar->setStatusBarTxt(tr("Never ever load the same database, that we want to write into!"));
            return;
        }

        // Set new MainState
        m_MainState = states::MAINSTATE_DB_IMPORT;
        emit sigMainWindowState(m_MainState);
        emit m_ButtonCollectionView->slotSetButtonState(states::BUTTON_STOP);

        qDebug() << "Choosen database path: " << m_ImportDbPath;
        emit sigImportDbToCollection(m_ImportDbPath);
        QString tmp = tr("Database Import to Music Collection\n ") + Utils::getBaseFromPath(m_ImportDbPath);
        m_FSModel.setHeaderTxt(tmp);

        m_pTree->reset();
        startPlayer();
        //setUpdateTreeLabelTxt( tmp);
    }
    // }
    // else
    //  {
    //     m_pStatusBar->setStatusBarTxt(tr("Ignored action! Database has already been loaded!"));
    // }
}

void MainWindow::slotImportDbToIncoming()
{
    qDebug() << "MainWindow::slotImportDbToIncoming";
    if (states::FM_DB_IMPORT_COLLECTION == m_FileSystemManager.getSelfState())
    {
        qDebug() << "CRASH IGNORED!";
        m_pStatusBar->setStatusBarTxt("Action ignored. Try to scan for new files at first!");
        return;
    }

    QString dbPath = QFileDialog::getOpenFileName(this, tr("Choose database file to import to file incoming!"), QDir::rootPath());

    // Avoid crash, when loading the same database twice
    //  if (dbPath != m_ForeignDbPath)
    //  {
    m_ImportDbPath = dbPath;
    if (QFile(m_ImportDbPath).exists())
    {
        // Avoid loading the database, which we want to write into.
        // PS: There are some idiots outta there!
        if (m_ImportDbPath.contains(DataBase::s_DatabaseName))
        {
            m_pStatusBar->setStatusBarTxt(tr("Never ever load the same database, that we want to write into!"));
            return;
        }

        // Set new MainState
        m_MainState = states::MAINSTATE_DB_IMPORT;
        emit sigMainWindowState(m_MainState);
        emit m_ButtonUpdateView->slotSetButtonState(states::BUTTON_STOP);

        qDebug() << "Choosen database path: " << m_ImportDbPath;

        QString tmp = tr("Database Import to File Incoming:\n") + Utils::getBaseFromPath(m_ImportDbPath);
        setUpdateTreeLabelTxt(tmp);
        emit sigImportDbToIncoming(m_ImportDbPath);
        startPlayer();
    }
    // }
    // else
    //  {
    //     m_pStatusBar->setStatusBarTxt(tr("Ignored action! Database has already been loaded!"));
    // }
}

void MainWindow::slotExportDbAs()
{
    qDebug() << "MainWindow::slotSaveDbAs";
    QString dbPath = QFileDialog::getSaveFileName(this, tr("Choose a filename for the database"), QDir::rootPath());
    qDebug() << "Choosen database path: " << dbPath;

    if(QFile::copy(DataBase::s_DatabaseName, dbPath))
        m_pStatusBar->setStatusBarTxt(tr("Successfully copied database to ") + dbPath);
    else
        m_pStatusBar->setStatusBarTxt(tr("Failed to copy database to ") + dbPath);
}

void MainWindow::traverseFileInfoList()
{
    qDebug() << "MainWindow::traverseFileInfoList";

    // Set values.
    int fileCnt=0;
    for( QFileInfoList::const_iterator it = m_CurFileInfoList.constBegin(); it != m_CurFileInfoList.constEnd(); it++ )
    {
        QString fp = it->filePath();
        QString name = Utils::getBaseFromPath(fp);

        m_FlacPathToMeta(fp);
        m_FlacPathToMeta.m_MetaDataValues[ m_FlacPathToMeta.m_LUT[ m_FlacPathToMeta.m_LabelFileName ] ] = name; // hack!
        if (m_FlacPathToMeta.isFlacValid()) //(Utils::getUtf8Thing(fp)))
        {
            //metadataValues = m_FlacPathToMeta.getMetaDataByFilePath();
            putFlacMetaDataToTable(m_FlacPathToMeta.getMetaDataValues(),  m_CurFileInfo.isFile());
        }
        else
        {
            // try to copy and rename, to read out flac.
            qDebug() << " try to copy, rename, to read out flac and destroy, afterwards.";
            QString dst = QDir::currentPath() + "/" + QString::number(++fileCnt)+ ".flac";

            if (QFile::copy(fp, dst))
            {
                m_FlacPathToMeta(dst);
                if ( m_FlacPathToMeta.isFlacValid() )
                {
                    //metadataValues = m_FlacPathToMeta.getMetaDataByFilePath();
                    putFlacMetaDataToTable(m_FlacPathToMeta.getMetaDataValues(),  m_CurFileInfo.isFile());
                    QFile::remove(dst);
                }
            }
            else
            {
                qDebug() << "Copy failed!";
            }

        }
    }
}

// Returns a list of selected items in music collection model
QFileInfoList MainWindow::getSelectionListFromCollectionTreeView()const
{
    QModelIndexList mil = m_pTree->selectionModel()->selectedIndexes();
    QFileInfoList fil;
    QFileInfo lastFi;

    foreach (const QModelIndex& it, mil)
    {
        QFileInfo fi(m_FSModel.fileInfo(it));
        if (lastFi.absoluteFilePath() != fi.absoluteFilePath())
        {
            fil.push_back(fi);
            lastFi=fi;
        }

    }
    return fil;
}


void MainWindow::removeSelectionFromCollection()
{
    qDebug() << "MainWindow::removeSelectionFromCollection()";

    // Remove selection
    QFileInfoList list = getSelectionListFromCollectionTreeView();

    if (!list.empty())
        removeContentFromFileInfoList(list);
    else
        qDebug() << "empty selection";
}

void MainWindow::removeContentFromFileInfoList(const QFileInfoList &fileInfoList)
{
    // Count values.
    int cnt=0;
    QStringList pathList;
    foreach(const QFileInfo cfi, fileInfoList)
    {
        pathList << cfi.absoluteFilePath();
        if (cfi.isDir())
        {
            QDir cdir(cfi.absoluteFilePath());
            cnt=cnt+(cdir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst).size());
        }
        else// (cfi.isFile())
        {
            cnt=cnt+1;
        }
    }
    emit sigResetRemoveProgressCounter();
    m_ProgressDialog.setMaximum(cnt);
    m_ProgressDialog.show();
    m_pStatusBar->show();
    emit sigRemoveFromFileSystem(pathList);
}

// Copy, if data already exists, it would be handled.
void MainWindow::copySelectionFromUpdateCollectionTop(const QString& dest)
{
    qDebug() << "MainWindow::copySelectionFromUpdateCollectionTop";

    // copy selection
    QModelIndexList list = m_pTreeUpdateView->selectionModel()->selectedIndexes();

    m_UpdateTreeViewModelIdxList = list;
    QStringList dataExistsList;
    int fileCount=0;
    m_CopyRequestDestList.clear();
    m_CopyRequestSrcList.clear();

    foreach (const QModelIndex& idx, list)
    {
        m_UpdateTreeViewModelIdx = idx;
        QFileInfo fi(getFileInfoByUpdateTreeTopModelIdx(idx));

        // Count Files for progressbar
        if(fi.isDir())
        {
            fileCount = fileCount + QDir(fi.absoluteFilePath()).entryList(QDir::Files).size();
        }
        else
        {
            fileCount = fileCount + 1;
        }

        // Construct destination paths.
        QString dotPart = Utils::getBaseFromPath(fi.absoluteFilePath());
        QString tmp="";

        // If destination parameter is empty, we copy to music collection path.
        if ( !dest.isEmpty() )
        {
            tmp = dest + "//" + dotPart;
        }
        else
        {
            tmp = m_MusicCollectionPath + "//" + dotPart;
        }
        if ( !dotPart.isEmpty() )
        {
            // Remember if data exists!
            if (QDir(tmp).exists())
            {
                dataExistsList.push_back(tmp);
            }
            m_CopyRequestDestList.push_back(tmp);
            m_CopyRequestSrcList.push_back(fi.absoluteFilePath());
        }
    }

    // Check if stuff exists.
    // Request a copy after remove is finished.
    if (!dataExistsList.isEmpty())
    {
        m_LastCopyRequestedFileCount=fileCount;
        qDebug() << "Data to copy already exists... Start user dialog...";
        if (!runMsgBoxFolderExistsOrCopy())
        {
            qDebug() << "User stopped copy action!";
            m_IsCopyRequested=false;
            m_LastCopyRequestedFileCount=0;
            return;
        }
        else
        {
            m_IsCopyRequested=true;
            int fcnt=0;
            emit sigResetRemoveProgressCounter();
            foreach (QString tmp, dataExistsList)
            {
                // Count Files for progressbar
                if(QFileInfo(tmp).isDir())
                {
                    fcnt = fcnt + QDir(tmp).entryList(QDir::Files).size();
                }
                else
                {
                    fcnt = fcnt + 1;
                }
            }
            m_ProgressDialog.setMaximum(fcnt);
            m_ProgressDialog.show();
            emit sigRemoveFromFileSystem(dataExistsList);
        }
    }
    else
    {
        m_IsCopyRequested=false;
        m_LastCopyRequestedFileCount=0;
        // Data does not exist! Copy...
        emit sigResetCopyProgressCounter();
        m_ProgressDialog.setMaximum(fileCount);
        m_ProgressDialog.show();
        emit sigCopyAllFolderTo(m_CopyRequestSrcList, m_CopyRequestDestList);
    }
}
/*
void MainWindow::copySelectionFromUpdateCollectionBottom( const QString& dest )
{
    // copy selection
    QModelIndexList list = m_pTreeUpdateViewBottom->selectionModel()->selectedIndexes();
    m_UpdateTreeViewBModelIdxList = list;
    QStringList dataExistsList;
    int fileCount=0;
    m_CopyRequestDestList.clear();
    m_CopyRequestSrcList.clear();

    foreach (const QModelIndex& idx, list)
    {
        m_UpdateTreeViewBModelIdx = idx;
        QFileInfo fi(getFileInfoByUpdateTreeBottomModelIdx(idx));

        // Count Files for progressbar
        if(fi.isDir())
        {
            fileCount = fileCount + QDir(fi.absoluteFilePath()).entryList(QDir::Files).size();
        }
        else
        {
            fileCount = fileCount + 1;
        }

        // Construct destination paths.
        QString dotPart = Utils::getBaseFromPath(fi.absoluteFilePath());
        QString tmp = "";
        // If destination parameter is empty we copy to collection.
        if ( dest.isEmpty() )
        {
            tmp = dest + "//" + dotPart;
        }
        else
        {
            tmp = m_MusicCollectionPath + "//" + dotPart;
        }

        if ( !dotPart.isEmpty() )
        {
            // Remember if data exists!
            if (QDir(tmp).exists())
            {
                dataExistsList.push_back(tmp);
            }
            m_CopyRequestDestList.push_back(tmp);
            m_CopyRequestSrcList.push_back(fi.absoluteFilePath());
        }
    }

    // Check if stuff exists.
    // Request a copy after remove is finished.
    if (!dataExistsList.isEmpty())
    {
        m_LastCopyRequestedFileCount=fileCount;
        qDebug() << "Data to copy already exists... Start user dialog...";
        if (!runMsgBoxFolderExistsOrCopy())
        {
            qDebug() << "User stopped copy action!";
            m_IsCopyRequested=false;
            m_LastCopyRequestedFileCount=0;
            return;
        }
        else
        {
            m_IsCopyRequested=true;
            int fcnt=0;
            emit sigResetRemoveProgressCounter();
            foreach (QString tmp, dataExistsList)
            {
                // Count Files for progressbar
                if(QFileInfo(tmp).isDir())
                {
                    fcnt = fcnt + QDir(tmp).entryList(QDir::Files).size();
                }
                else
                {
                    fcnt = fcnt + 1;
                }
            }
            m_ProgressDialog.setMaximum(fcnt);
            m_ProgressDialog.show();
            emit sigRemoveFromFileSystem(dataExistsList);
        }
    }
    else
    {
        // Data does not exist! Copy...
        m_IsCopyRequested=false;
        m_LastCopyRequestedFileCount=0;
        emit sigResetCopyProgressCounter();
        m_ProgressDialog.setMaximum(fileCount);
        m_ProgressDialog.show();
        emit sigCopyAllFolderTo(m_CopyRequestSrcList, m_CopyRequestDestList);
    }
}
*/
const QFileInfo& MainWindow::getFileInfoByUpdateTreeTopModelIdx(const QModelIndex& idx)
{
    QString curFolder;
    QString curFile;

    m_UpdateTreeViewModelIdx = idx;

    if (idx.parent().isValid())
    {
        curFolder = m_FileIncomingPath + idx.parent().data().toString();
        curFile = idx.data().toString();
        m_pStatusBar->setStatusBarTxt("File Clicked! Current Folder: " + curFolder + " and File: " + curFile);
        m_IsFile = true;

    }
    else
    {
        curFolder = m_FileIncomingPath + idx.data().toString();
        m_pStatusBar->setStatusBarTxt("Folder clicked! Current Folder: " + curFolder);
        m_IsFile = false;
    }

    QString sumPath = (curFile.isEmpty()) ? curFolder : (curFolder + "//" + curFile);
    QFileInfo *fi = new QFileInfo(sumPath);
    qDebug() << sumPath;
    return *fi;
}
/*
const QFileInfo &MainWindow::getFileInfoByUpdateTreeBottomModelIdx(const QModelIndex &idx)
{
    QString curFolder;
    QString curFile;

    m_UpdateTreeViewBModelIdx = idx;

    if (idx.parent().isValid())
    {
        curFolder = m_FileIncomingPath + idx.parent().data().toString();
        curFile = idx.data().toString();
        m_pStatusBar->setStatusBarTxt("File Clicked! Current Folder: " + curFolder + " and File: " + curFile);
        m_IsFile = true;

    }
    else
    {
        curFolder = m_FileIncomingPath + idx.data().toString();
        m_pStatusBar->setStatusBarTxt("Folder clicked! Current Folder: " + curFolder);
        m_IsFile = false;
    }

    QString sumPath = (curFile.isEmpty()) ? curFolder : (curFolder + "//" + curFile);
    QFileInfo *fi = new QFileInfo(sumPath);
    qDebug() << sumPath;
    return *fi;
}
*/
void MainWindow::removeSelectedUpdateItems()
{

    // This approach fails for removing selected child items, why?
    foreach(const QModelIndex& idx, m_pTreeUpdateView->selectionModel()->selectedIndexes())
    {
        if (idx.parent().isValid())
        {

            m_pUpdateModel->removeRow(idx.row(), idx.parent());
            //qDebug() << "isValid " << idx.data().toString();
        }
        else
        {
            //
            m_pUpdateModel->removeRow(idx.row());
            m_pTreeUpdateView->update();
            //qDebug() << "notValid";
        }
    }

    /*
    if( m_TreeUpdateView->selectionModel()->hasSelection() ) {  // check if there are selections?
       // there are selections
       // QTreeView also contains api's to work with selected indexes, although convenient, this would cause a problem
       // because selectedIndexes is unsorted - meaning updated as user selects items and can be random
       // foreach( QModelIndex index, treeView->selectedIndexes() ) {
       //    model->takeRow( index.row() );  // QStandardItemModel takeRow( int row )
       // }
       //
       // In the above scenario if you had rows 1,2,3,4,5  and rows 2,4 were selected for deletion,
      //  then rows 2 and 5 would be deleted.  Because once you deleted 2, row 5 now becomes row 4 (in model).
      //  So you need to ensure you work from the bottom up when deleting rows from a QTreeView.
      //  Something Like:

      // Capture all rows to be deleted by it's row number in list
      QList<int> rowList;
      foreach( QModelIndex index, m_TreeUpdateView->selectionModel()->selectedIndexes() )
      {
          rowList << index.row();
      }
      // Use QtAlgorithm qSort to reverse sort row numbers in list
      qSort( rowList.begin(), rowList.end(), qGreater<int>() );  // reverse sort 5,4,3,...

      // Perform deletions - model updates will update tree view.
      foreach( int row, rowList )
      {
          m_UpdateModel->takeRow( row );  // QStandardItemModel takeRow( int row )
      }
    }

*/
}


// Should clear out all content from update view.
void MainWindow::clearUpdateViews()
{
    qDebug() << "MainWindow::clearUpdateViews";
    m_pTreeUpdateView->reset();
    m_pUpdateModel->clear();
    //m_pTreeUpdateViewBottom->reset();
    //m_pUpdateModelBottom->clear();
}

void MainWindow::readPersistedFilePath()
{
    if ( m_Persistence.getLastOpenCollectionPathEnabled())
        m_MusicCollectionPath = m_Persistence.getLastOpenCollectionPath();
    else
        m_MusicCollectionPath.clear();// = QDir::rootPath();
}

// Either we have remembered a valid, last set collection path
// or we prompt a msgbox for the user to set the path.
void MainWindow::handleStartBehaviour()
{
    if(QDir(m_MusicCollectionPath).exists() && !m_MusicCollectionPath.isEmpty())
    {
        //emit sigSearchInMusicCollection(m_MusicCollectionPath);
        m_pStatusBar->setStatusBarTxt("Your Music Collection was succesfully regenerated. Please chose a file incoming path!");
        setTreeViewRoot(m_MusicCollectionPath);
    }
    else
    {

        m_pStatusBar->setStatusBarTxt("Please chose your Music collection path!");
        if (m_Persistence.getEnableWizard())
        {
            emit sigRunSelectCollectionPathMsgBox();
        }
    }
}

void MainWindow::quitFileSystemManagerThread()
{
    m_FileSystemManager.thread()->quit();
}

void MainWindow::updateFreeDiskSpace(const QString &path)
{
    qDebug() << "MainWindow::updateFreeDiskSpace: " << path;

    FreeDiskStorage fds;
    FreeDiskStorageInfo res = fds.getFreeStorageInfo(path);
    qDebug() << "Free diskspace -> total: " << res.totalAmountOfSpace << " free: " << res.freeAmountOfSpace;
    m_FreeDiskSpaceCollectionBar.setMaximum(res.totalAmountOfSpace);
    m_FreeDiskSpaceCollectionBar.setValue(res.totalAmountOfSpace - res.freeAmountOfSpace);
    QString toolTipString = "Total Disk Space = " + QString::number(res.totalAmountOfSpace/1024) + "." + QString::number(res.totalAmountOfSpace%1024)+ " GB"
            + "\nUsed Disk Space = " + (QString::number((res.totalAmountOfSpace-res.freeAmountOfSpace)/1024) + "." + QString::number((res.totalAmountOfSpace-res.freeAmountOfSpace)%1024)+ " GB"
                                        + "\nFree Disk Space = " + QString::number(res.freeAmountOfSpace/1024) + "." + QString::number(res.freeAmountOfSpace%1024)+ " GB");
    m_FreeDiskSpaceCollectionBar.setToolTip(toolTipString);
}

void MainWindow::centerProgressDialog()
{
    // Center ProgressDialog
    int w = this->width();   // returns screen width
    int h = this->height();  // returns screen height
    int mw = m_ProgressDialog.width();
    int mh = m_ProgressDialog.height();
    int cw = (w/2) - (mw/2);
    int ch = (h/2) - (mh/2);
    m_ProgressDialog.move(cw,ch);
}

void MainWindow::setUpdateTreeLabelTxt(const QString &txt)
{
    m_pUpdateModel->setHorizontalHeaderLabels(QStringList(txt));
    m_pTreeUpdateView->setModel(m_pUpdateModel);
}

void MainWindow::startPlayer()
{
    qDebug() << "MainWindow::startPlayer()";

    // Ladies and Gentleman: The Hypnotoad!
    if (m_HypnotoadEnabled && (NULL != m_Player))
    {
        m_Player->setWindowFlags(Qt::WindowStaysOnTopHint);
        QStringList playlist;
        playlist << "Hypno.mp4" << "Hypno.mp4" <<  "Hypno.mp4" <<  "Hypno.mp4" <<  "Hypno.mp4" <<  "Hypno.mp4";
        if ( states::MAINSTATE_DB_IMPORT == m_MainState )
            emit m_Player->slotSetStatusInfo("Database Import...");
        else
            emit m_Player->slotSetStatusInfo("File Scan...");

        emit m_Player->addToPlaylist(playlist);
        emit m_Player->slotPlay();
        m_Player->show();
        connect(m_Player, SIGNAL(fullScreenChanged(bool)), this, SIGNAL(sigStopScans()));
    }
    else
    {
        qDebug() << "ERROR: MainWindow::startPlayer: player is NULL" ;
    }
}

void MainWindow::stopPlayer()
{
    qDebug() << "MainWindow::stopPlayer()";
    if (NULL!=m_Player)
    {
        emit m_Player->slotStop();
        m_Player->hide();
    }
}

