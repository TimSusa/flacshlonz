//////////////////////////////////////////////////////////////////////
//!\file main.cpp
//!\brief Application main process.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#include "MainWindow.hpp"
#include <QtGui>
#include "Application.hpp"

int main( int argc, char *argv[] )
{
    Application app( argc, argv );
    app.setWindowIcon( QIcon( QString::fromUtf8( ":/icon2.png" ) ) );
    QObject::connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
    MainWindow mainWin;
    app.setMainWindow(&mainWin); // TODO: Enable GUI control via command line.
    mainWin.setObjectName("MainWindow");
    mainWin.showMaximized();
    return app.exec();
}
