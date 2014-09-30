//////////////////////////////////////////////////////////////////////
//!\file  ConfigPageAlgorithm.cpp
//!\brief definition of class ConfigPageAlgorithm.
//////////////////////////////////////////////////////////////////////


#include "ConfigPageAlgorithm.hpp"
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QDebug>

ConfigPageAlgorithm::ConfigPageAlgorithm(QWidget *parent)
    : QWidget(parent),
      m_Persistence()
{
    QGroupBox *algorithmGroup = new QGroupBox(tr("Algorithm Settings"));
    /*
    QCheckBox *deepScanCheckBox = new QCheckBox(tr("Deep Scan (Scan & Compare Metadata)"));
    deepScanCheckBox->setChecked(m_Persistence.getEnableDeepScan());
    QObject::connect(deepScanCheckBox, SIGNAL(clicked(bool)), this, SLOT(slotEnableDeepScan(bool)));

    QCheckBox *quickScanCheckBox = new QCheckBox(tr("Quick Scan (Scan Quick and Dirty)"));
    quickScanCheckBox->setChecked(false);
    quickScanCheckBox->setEnabled(false);
    QObject::connect(quickScanCheckBox, SIGNAL(clicked(bool)), this, SLOT(slotEnableQuickScan(bool)));
    */

    // Radio Buttons.
    QButtonGroup *radioGroup = new QButtonGroup;
    QRadioButton *radioButtonDeepScan = new QRadioButton(this);
    radioButtonDeepScan->setText("Deep Scan (Compare Metadata)");
    radioButtonDeepScan->setChecked(m_Persistence.getEnableDeepScan());
    QObject::connect(radioButtonDeepScan, SIGNAL(clicked(bool)), this, SLOT(slotEnableDeepScan(bool)));
    radioGroup->addButton(radioButtonDeepScan);

    QRadioButton *radioButtonQuickScan = new QRadioButton(this);
    radioButtonQuickScan->setText("Quick Scan (Compare Foldernames only)");
    radioButtonQuickScan->setChecked(!m_Persistence.getEnableDeepScan());
    QObject::connect(radioButtonQuickScan, SIGNAL(clicked(bool)), this, SLOT(slotEnableQuickScan(bool)));
    radioGroup->addButton(radioButtonQuickScan);
   // radioButtonQuickScan->hide();

    QVBoxLayout *algorithmLayout = new QVBoxLayout;
    //algorithmLayout->addWidget(deepScanCheckBox);
    algorithmLayout->addWidget(radioButtonDeepScan);
    algorithmLayout->addWidget(radioButtonQuickScan);
    algorithmGroup->setLayout(algorithmLayout);
    algorithmGroup->hide();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(algorithmGroup);
    mainLayout->addSpacing(12);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    // As long as this is not implemented, we disable this GUI part.
    //this->setEnabled(false);
}


// Be carefull here, not to trigger an endless loop.
void ConfigPageAlgorithm::slotEnableDeepScan(bool isChecked)
{
    qDebug() << "ConfigPageAlgorithm::slotEnableDeepScan " << isChecked;
    m_Persistence.persistEnableDeepScan(isChecked);

    if (isChecked)
    {
        emit slotEnableQuickScan(false);
    }

    emit sigEnableDeepScan(isChecked);
}

void ConfigPageAlgorithm::slotEnableQuickScan(bool isChecked)
{
    qDebug() << "ConfigPageAlgorithm::slotEnableQuickScan " << isChecked;

    emit sigEnableQuickScan(isChecked);

    if (isChecked)
    {
        emit slotEnableDeepScan(false);
    }
}
