//////////////////////////////////////////////////////////////////////
//!\file  ConfigPageAlgorithm.hpp
//!\brief definition of class ConfigPageAlgorithm.
//////////////////////////////////////////////////////////////////////

#ifndef CONFIGPAGEALGORITHM_HPP
#define CONFIGPAGEALGORITHM_HPP

#include <QWidget>
#include "Persistence.hpp"

class ConfigPageAlgorithm : public QWidget
{
    Q_OBJECT

public:
    ConfigPageAlgorithm(QWidget *parent = 0);

signals:
   // void sigEnableDeepScan(bool isChecked);
   // void sigEnableQuickScan(bool isChecked);

public slots:
   // void slotEnableDeepScan(bool isChecked);
   // void slotEnableQuickScan(bool isChecked);

private:
    Persistence m_Persistence;
};

#endif // CONFIGPAGEALGORITHM_HPP
