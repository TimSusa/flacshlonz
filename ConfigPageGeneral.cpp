//////////////////////////////////////////////////////////////////////
//!\file  ConfigPageGeneral.cpp
//!\brief definition of class ConfigPages.
//////////////////////////////////////////////////////////////////////


#include <QtGui>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include "ConfigPageGeneral.hpp"

ConfigPageGeneral::ConfigPageGeneral(QWidget *parent)
    : QWidget(parent),
      m_Persistence()
{
    QGroupBox *generalSettingsGroup = new QGroupBox(tr("General settings"));

    QCheckBox *rememberLastOpenFileCheckBox = new QCheckBox(tr("Keep search results until next restart."));
    rememberLastOpenFileCheckBox->setChecked( m_Persistence.getLastOpenCollectionPathEnabled() );
    QObject::connect(rememberLastOpenFileCheckBox, SIGNAL(clicked(bool)), this, SLOT(slotEnableLastOpenedFile(bool)));

    QCheckBox *checkBoxEnableWizard = new QCheckBox(tr("Enable Wizard, which leads you to app."));
    checkBoxEnableWizard->setChecked(m_Persistence.getEnableWizard());
    QObject::connect(checkBoxEnableWizard, SIGNAL(clicked(bool)), this, SLOT(slotEnableWizard(bool)));
/*
    QCheckBox *checkBoxDbTo = new QCheckBox(tr("Database import to collection (If unchecked, import to file-incoming)."));
    checkBoxDbTo->setChecked(m_Persistence.getEnableDbImportTo());
    QObject::connect(checkBoxDbTo, SIGNAL(clicked(bool)), this, SLOT(slotEnableDbImportTo(bool)));
*/
    QCheckBox *checkBoxHypnotoad = new QCheckBox(tr("Show Hypnotoad on progress."));
    checkBoxHypnotoad->setChecked(m_Persistence.getEnableHypnotoad());
    QObject::connect(checkBoxHypnotoad, SIGNAL(clicked(bool)), this, SLOT(slotEnableHypnotoad(bool)));

    QVBoxLayout *mainSettingsLayout = new QVBoxLayout;
    mainSettingsLayout->addWidget(rememberLastOpenFileCheckBox);
    mainSettingsLayout->addWidget(checkBoxEnableWizard);
  //  mainSettingsLayout->addWidget(checkBoxDbTo);
    mainSettingsLayout->addWidget(checkBoxHypnotoad);
    generalSettingsGroup->setLayout(mainSettingsLayout);

    QGroupBox *configConnectionGroup = new QGroupBox(tr("Connection settings"));
    QLabel *serverLabel = new QLabel(tr("Server:"));
    QComboBox *serverCombo = new QComboBox;
    serverCombo->addItem(tr("Server 1"));
    serverCombo->addItem(tr("Server 2"));
    QHBoxLayout *serverLayout = new QHBoxLayout;
    serverLayout->addWidget(serverLabel);
    serverLayout->addWidget(serverCombo);
    QLabel *ipLabel = new QLabel(tr("IP:"));
    QLineEdit *ipEdit = new QLineEdit("192.168.1.2");
    QHBoxLayout *ipLayout = new QHBoxLayout;
    ipLayout->addWidget(ipLabel);
    ipLayout->addWidget(ipEdit);
    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addLayout(serverLayout);
    configLayout->addLayout(ipLayout);
    configConnectionGroup->setLayout(configLayout);

    configConnectionGroup->setEnabled(false);
    configConnectionGroup->hide();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(generalSettingsGroup); // main settings
    mainLayout->addWidget(configConnectionGroup); // connection settings
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void ConfigPageGeneral::slotEnableLastOpenedFile(bool enabled)
{
    m_Persistence.persistLastOpenCollectionPathEnabled(enabled);
    emit sigEnableLastOpenedFile(enabled);
}

void ConfigPageGeneral::slotEnableWizard(bool enabled)
{
    m_Persistence.persistEnableWizard(enabled);
    emit sigEnableWizard(enabled);
}
/*
void ConfigPageGeneral::slotEnableDbImportTo(bool enabled)
{
    qDebug() << "ConfigPageGeneral::slotEnableDbQueryBox";
    m_Persistence.persistEnableDbImportTo(enabled);
    emit sigEnableDbImportTo(enabled);
}
*/
void ConfigPageGeneral::slotEnableHypnotoad(bool enabled)
{
    qDebug() << "ConfigPageGeneral::slotEnableHypnoAd" << enabled;
    m_Persistence.persistEnableHypnotoad(enabled);
    emit sigEnableHypnotoad(enabled);
}


