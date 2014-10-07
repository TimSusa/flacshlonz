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
#include "fvupdater.h"

int main( int argc, char *argv[] )
{
    Application app( argc, argv );

    // Set this to your own appcast URL, of course
    FvUpdater::sharedUpdater()->SetFeedURL("https://raw.github.com/pypt/fervor/master/sample/Appcast.xml");

    // Check for updates silently -- this will not block the initialization of
    // your application, just start a HTTP request and return immediately.
    FvUpdater::sharedUpdater()->CheckForUpdatesSilent();

    app.setWindowIcon( QIcon( QString::fromUtf8( ":/icon2.png" ) ) );
    QObject::connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    MainWindow mainWin;
    app.setMainWindow(&mainWin);
    mainWin.setObjectName("MainWindow");
    mainWin.showMaximized();

    return app.exec();
}
