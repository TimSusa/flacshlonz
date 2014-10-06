//////////////////////////////////////////////////////////////////////
//!\file StatusBar.hpp
//!\brief Subclassed QStatusBar.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include <QWidget>
#include <QStatusBar>
#include <QLabel>
#include <QString>

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit StatusBar(QStatusBar *parent = 0);
    void setStatusBarTxt(const QString& txt);
signals:
public slots:
private:
    QString m_LastEntry;
};

#endif // STATUSBAR_HPP
