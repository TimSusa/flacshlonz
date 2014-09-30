#//////////////////////////////////////////////////////////////////////
#//!\file flacschlonz.pro
#//!\brief Projects file.
#//!\author timmsusa@googlemail.com
#//!
#//! $Author: tsusa $
#//! $Date: 2013-05-28 15:48:51 +0200 (Tue, 28 May 2013) $
#//! $Revision: 239220 $
#//////////////////////////////////////////////////////////////////////

QT       += core gui sql widgets
QT += network \
      xml \
      multimedia \
      multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += mobility
MOBILITY += systeminfo

TEMPLATE = app
TARGET = qtFlacSchlonz
win32: DEFINES += UTF8
win32: DEFINES -= UNICODE
#DEFINES += FLAC__NO_DLL
#DEFINES += QT_NO_CAST_FROM_ASCII \
#           QT_NO_CAST_TO_ASCII

#DEPENDPATH += .


# Input
HEADERS += \
    MainWindow.hpp \
    FileSystemManager.hpp \
    FlacMetaDataManager.hpp\
    FlacTrack.hpp \
    StatusBar.hpp \
    Utils.hpp \
    FileSystemModel.hpp \
    Persistence.hpp \
    ConfigDialog.hpp \
    ConfigPageAlgorithm.hpp \
    ConfigPageGeneral.hpp \
    ThreadFlacScanner.hpp \
    ThreadRemoveFiles.hpp \
    ThreadCopyFiles.hpp \
    DataBase.hpp \
    FreeDiskStorage.hpp \
    ThreadDataBaseImport.hpp \
    Application.hpp \
    StringEncodingWin.hpp \
    MetaDataKeys.hpp \
    player/Player.hpp \
    player/VideoWidget.hpp

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    FileSystemManager.cpp \
    FlacMetaDataManager.cpp \
    StatusBar.cpp \
    Utils.cpp \
    FileSystemModel.cpp \
    Persistence.cpp \
    ConfigDialog.cpp \
    ConfigPageAlgorithm.cpp \
    ConfigPageGeneral.cpp \
    ThreadFlacScanner.cpp \
    ThreadRemoveFiles.cpp \
    ThreadCopyFiles.cpp \
    DataBase.cpp \
    FreeDiskStorage.cpp \
    ThreadDataBaseImport.cpp \
    Application.cpp \
    MetaDataKeys.cpp \
    StringEncodingWin.cpp \
    player/Player.cpp \
    player/VideoWidget.cpp


RESOURCES += flacshlonz.qrc

#win32: INCLUDEPATH += "C:\\_SUSATA\\_CODING\\CPP\\flac-1.3.0\\include\\share"
#win32: INCLUDEPATH += "C:\\_SUSATA\\_CODING\\CPP\\FLAC\\include"
win32: INCLUDEPATH += "C:\\_SUSATA\\_CODING\\CPP\\flac-1.3.0\\include" \
 "_SUSATA/_CODING/CPP/qtFlacschlonz/"
win32: LIBS += -llibFLAC++_dynamic -llibFLAC_dynamic
#win32: LIBS += -llibFLAC++ -llibFLAC
unix: LIBS += -lFLAC++
#win32: LIBS += -L"C:\\_SUSATA\\_CODING\\CPP\\FLAC\\lib\\libFLAC++"
win32: LIBS += -L"C:\\_SUSATA\\_CODING\\CPP\\flac-1.3.0\\objs\\release\\lib"


#win32: DESTDIR += "C:\\_SUSATA\\_CODING\\CPP\\build-flacshlonz-Desktop_Qt_5_3_MSVC2010_32bit-Release"
win32: DESTDIR += "C:\_SUSATA\_CODING\CPP\flac-1.3.0\objs\release\lib"
#win32: DESTDIR += "C:\\_SUSATA\\_CODING\\CPP\\build-flacshlonz-Desktop_Qt_5_2_1_MSVC2010_32bit-release\\release"
#choose visual studio compiler build path
