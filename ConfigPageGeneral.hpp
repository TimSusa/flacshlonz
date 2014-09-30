//////////////////////////////////////////////////////////////////////
//!\file  ConfigPageGeneral.hpp
//!\brief Definition of class ConfigPageGeneral.
//////////////////////////////////////////////////////////////////////


#ifndef CONFIGPAGEGENERAL_HPP
#define CONFIGPAGEGENERAL_HPP

#include <QWidget>
#include "Persistence.hpp"

class ConfigPageGeneral : public QWidget
{
    Q_OBJECT

public:
    ConfigPageGeneral(QWidget *parent = 0);

signals:
    void sigEnableWizard(bool enabled);
    void sigEnableLastOpenedFile(bool enabled);
    void sigEnableDbImportTo(bool enabled);
    void sigEnableHypnoad(bool enabled);

public slots:
    void slotEnableLastOpenedFile(bool enabled);
    void slotEnableWizard(bool enabled);
    void slotEnableDbImportTo(bool enabled);
    void slotEnableHypnoAd(bool enabled);

private:
    Persistence m_Persistence;
};

#endif
