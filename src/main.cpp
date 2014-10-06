//////////////////////////////////////////////////////////////////////
//!\file main.cpp
//!\brief Application main process.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////
/// Deploymentinfo:
// http://lemirep.wordpress.com/2013/06/01/deploying-qt-applications-on-linux-and-windows-3/

#include "MainWindow.hpp"

#include <QtGui>
#include "Application.hpp"


int main( int argc, char *argv[] )
{
    Application app( argc, argv );

    app.setWindowIcon( QIcon( QString::fromUtf8( ":/icon2.png" ) ) );
    QObject::connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    MainWindow mainWin;
    app.setMainWindow(&mainWin);
    mainWin.setObjectName("MainWindow");
    mainWin.showMaximized();

    return app.exec();
}
