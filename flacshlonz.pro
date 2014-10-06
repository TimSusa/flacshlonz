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
# WIN32: Please configure absolute paths on your own!

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
    player/VideoWidget.hpp \
    StateButton.hpp \
    States.hpp

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
    player/VideoWidget.cpp \
    StateButton.cpp

# RESOURCES
RESOURCES += flacshlonz.qrc

# LIBRARIES
unix: LIBS += -lFLAC++

win32
{
    INCLUDEPATH += "C:\\_SUSATA\\_CODING\\CPP\\flac-1.3.0\\include" \
                      "_SUSATA/_CODING/CPP/qtFlacschlonz/"

    LIBS += -llibFLAC++_dynamic -llibFLAC_dynamic
    # LIBS += -llibFLAC++ -llibFLAC

    FLAC_LIB_DIR = "C:\\_SUSATA\\_CODING\\CPP\\flac-1.3.0\\objs\\release\\lib"
    LIBS += -L$${FLAC_LIB_DIR}
    DESTDIR += "C:\\tmpFlac"

    # Copy all flac- libs to destination dir.
    QMAKE_POST_LINK = copy $${FLAC_LIB_DIR}\*.dll $${DESTDIR}
}

# Misc Files.
#OTHER_FILES += \
#    libFLAC++_dynamic.dll \
#    libFLAC_dynamic.dll
#Hypno.mp4

# Copy OTHER_FILES to DESTDIR.
#win32 {
#    DESTDIR_WIN = $${DESTDIR}
#    DESTDIR_WIN ~= s,/,\\,g
#    PWD_WIN = $${FLAC_LIB_DIR}#$${PWD} # PWD is where the source files lie.
#    PWD_WIN ~= s,/,\\,g
#    for(FILE, OTHER_FILES){
#        QMAKE_POST_LINK += $$quote(cmd /c copy /y $${PWD_WIN}\\$${FILE} $${DESTDIR_WIN}$$escape_expand(\\n\\t))
#    }
#}
#unix {
#    for(FILE, OTHER_FILES){
#        QMAKE_POST_LINK += $$quote(cp $${PWD}/$${FILE} $${DESTDIR}$$escape_expand(\\n\\t))
#}
