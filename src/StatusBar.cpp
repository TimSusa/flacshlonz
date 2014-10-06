//////////////////////////////////////////////////////////////////////
//!\file StatusBar.cpp
//!\brief Subclassed QStatusBar.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////


#include "StatusBar.hpp"

StatusBar::StatusBar(QStatusBar *parent) :
    QStatusBar(parent),
    m_LastEntry()
{
}


void StatusBar::setStatusBarTxt(const QString& txt)
{
    // Block pushing the GUI with same values
    if ( m_LastEntry != txt )
    {
        m_LastEntry = txt;
        showMessage(m_LastEntry);
    }

}
