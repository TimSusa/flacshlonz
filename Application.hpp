#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QApplication>
#include <QSessionManager>
#include "MainWindow.hpp"

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);
    void commitData(QSessionManager& manager); // linux X11
    void saveState(QSessionManager &sessionManager);// linux X11
    void setMainWindow(MainWindow *mainWindow);
signals:

public slots:
private:
    MainWindow *m_MainWindow;
};

#endif // APPLICATION_HPP
