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
    setlocale(LC_ALL, "");
    Application app( argc, argv );

    app.setWindowIcon( QIcon( QString::fromUtf8( ":/icon2.png" ) ) );
    QObject::connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    MainWindow mainWin;
    app.setMainWindow(&mainWin);
    mainWin.setObjectName("MainWindow");
    mainWin.showMaximized();
    //mainWin.showNormal();

    return app.exec();
}
