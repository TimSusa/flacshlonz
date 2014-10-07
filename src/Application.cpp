#include "Application.hpp"
#include <QMessageBox>
#include <QDebug>
//#include "fvupdater.h"

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv)
{

}

// Called, when the user logs out.
void Application::commitData(QSessionManager& manager)
{
    qDebug() << "Application::commitData";

    if (manager.allowsInteraction()&&(NULL!=m_MainWindow)) {
        QMessageBox box;
        box.setWindowTitle("flacshlonz");
        box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        box.setIcon(QMessageBox::Warning);

        int ret = box.exec();
        /*QMessageBox::warning(
                     this, //mainWindow,
                     tr("My Application"),
                     tr("Save changes to document?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);*/

        switch (ret) {
        case QMessageBox::Save:
            manager.release();
            // Save and persist here!
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
        default:
            manager.cancel();
        }
    } else {
        // we did not get permission to interact, then
        // do something reasonable instead
    }
}

void Application::setMainWindow(MainWindow *mainWindow)
{
    qDebug() << " Application::setMainWindow";
    m_MainWindow = mainWindow;
}

void Application::saveState(QSessionManager &sessionManager)
{
    qDebug() << "Application::saveState";
    QString fileName = m_MainWindow->saveState();
    qDebug() << "Application::saveState -> Session-File-Name: " << fileName;
    QStringList discardCommand;
    discardCommand << "rm" << fileName;
    sessionManager.setDiscardCommand(discardCommand);
}
