#//////////////////////////////////////////////////////////////////////
#//!\file flacschlonz.pro
#//!\brief Projects file.
#//!\author timmsusa@googlemail.com
#//!
#//! $Author: tsusa $
#//! $Date: 2013-05-28 15:48:51 +0200 (Tue, 28 May 2013) $
#//! $Revision: 239220 $
#//////////////////////////////////////////////////////////////////////
# Requirements:
# - FLAC 1.3
# - QT 5.2.1

QT +=   core \
        gui \
        sql \
        widgets \
        network \
        xml \
        multimedia \
        multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = FlacShlonz

win32: DEFINES += UTF8
win32: DEFINES -= UNICODE

#DEPENDPATH += .


# SOURCE FILES
HEADERS += \
    src/player/Player.hpp \
    src/player/VideoWidget.hpp \
    src/settings/ConfigDialog.hpp \
    src/settings/ConfigPageAlgorithm.hpp \
    src/settings/ConfigPageGeneral.hpp \
    src/settings/Persistence.hpp \
    src/threads/ThreadCopyFiles.hpp \
    src/threads/ThreadDataBaseImport.hpp \
    src/threads/ThreadFlacScanner.hpp \
    src/threads/ThreadRemoveFiles.hpp \
    src/Application.hpp \
    src/DataBase.hpp \
    src/FileSystemManager.hpp \
    src/FileSystemModel.hpp \
    src/FlacMetaDataManager.hpp \
    src/FlacTrack.hpp \
    src/FreeDiskStorage.hpp \
    src/MainWindow.hpp \
    src/MetaDataKeys.hpp \
    src/StateButton.hpp \
    src/States.hpp \
    src/StatusBar.hpp \
    src/StringEncodingWin.hpp \
    src/Utils.hpp

SOURCES += \
    src/player/Player.cpp \
    src/player/VideoWidget.cpp \
    src/settings/ConfigDialog.cpp \
    src/settings/ConfigPageAlgorithm.cpp \
    src/settings/ConfigPageGeneral.cpp \
    src/settings/Persistence.cpp \
    src/threads/ThreadCopyFiles.cpp \
    src/threads/ThreadDataBaseImport.cpp \
    src/threads/ThreadFlacScanner.cpp \
    src/threads/ThreadRemoveFiles.cpp \
    src/Application.cpp \
    src/DataBase.cpp \
    src/FileSystemManager.cpp \
    src/FileSystemModel.cpp \
    src/FlacMetaDataManager.cpp \
    src/FreeDiskStorage.cpp \
    src/main.cpp \
    src/MainWindow.cpp \
    src/MetaDataKeys.cpp \
    src/StateButton.cpp \
    src/StatusBar.cpp \
    src/StringEncodingWin.cpp \
    src/Utils.cpp


# RESOURCES
RESOURCES += flacshlonz.qrc \

# LIBRARIES
unix: LIBS += -lFLAC++

# Define function that copies given files to dest dir.
defineTest(copyToDestdir) {
    files = $${1}

    for(FILE, files) {
        DDIR = $${DESTDIR}

        # On windows replace slashes in paths with backslashes.
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$quote(cmd /c copy /y $${FILE} $${DDIR}$$escape_expand(\\n\\t))
    }
    export(QMAKE_POST_LINK)
}

win32 {    
    DESTDIR += "C:\\tmpFlac"

    INCLUDEPATH += $${PWD}\\3rdParty\\flac\\win32\\inc

    LIBS += -llibFLAC++_dynamic -llibFLAC_dynamic
    # LIBS += -llibFLAC++ -llibFLAC

    FLAC_LIB_DIR = $$quote($${PWD}\\3rdParty\\flac\\win32\\lib)
    LIBS += -L$${FLAC_LIB_DIR}


    # Copy flac libs + hypnovid
    copyToDestdir( $${FLAC_LIB_DIR}\\*.dll $${PWD}\\src\\player\\$$quote(Hypno.mp4) )
}

#unix {
#    for(FILE, OTHER_FILES){
#        QMAKE_POST_LINK += $$quote(cp $${PWD}/$${FILE} $${DESTDIR}$$escape_expand(\\n\\t))
#}
