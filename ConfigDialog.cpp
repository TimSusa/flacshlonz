//////////////////////////////////////////////////////////////////////
//!\file  ConfigDialog.cpp
//!\brief definition of class ConfigDialog.
//////////////////////////////////////////////////////////////////////


#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "ConfigDialog.hpp"

ConfigDialog::ConfigDialog():
    m_pContentsWidget(new QListWidget),
    m_pPagesWidget(new QStackedWidget),
    m_pConfigPageGeneral(new ConfigPageGeneral),
    m_pConfigPageAlgorithm(new ConfigPageAlgorithm)
{
    m_pContentsWidget->setViewMode(QListView::IconMode);
    m_pContentsWidget->setIconSize(QSize(96, 84));
    m_pContentsWidget->setMovement(QListView::Static);
    m_pContentsWidget->setMaximumWidth(128);
    m_pContentsWidget->setSpacing(12);

    m_pPagesWidget->addWidget(m_pConfigPageGeneral);
    m_pPagesWidget->addWidget(m_pConfigPageAlgorithm);

    QPushButton *closeButton = new QPushButton(tr("Close"));

    createLeftPane();
    m_pContentsWidget->setCurrentRow(0);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(slotConfigDiagFinished()));

    // set sigs.
    connect(m_pConfigPageAlgorithm, SIGNAL(sigEnableQuickScan(bool)), this, SIGNAL(sigEnableQuickScan(bool)));
    connect(m_pConfigPageAlgorithm, SIGNAL(sigEnableDeepScan(bool)), this, SIGNAL(sigEnableDeepScan(bool)));
    connect(m_pConfigPageGeneral, SIGNAL(sigEnableWizard(bool)), this, SIGNAL(sigEnableWizard(bool)));
    connect(m_pConfigPageGeneral, SIGNAL(sigEnableDbImportTo(bool)), this, SIGNAL(sigEnableDbImportTo(bool)));
    connect(m_pConfigPageGeneral, SIGNAL(sigEnableLastOpenedFile(bool)), this, SIGNAL(sigEnableLastOpenedFile(bool)));
    connect(m_pConfigPageGeneral, SIGNAL(sigEnableHypnoad(bool)), this, SIGNAL(sigEnableHypnoad(bool)));


    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(m_pContentsWidget);
    horizontalLayout->addWidget(m_pPagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Configuration Dialog"));
}

void ConfigDialog::createLeftPane()
{
    QListWidgetItem *configGeneralButton = new QListWidgetItem(m_pContentsWidget);
    //configButton->setIcon(QIcon(":/images/config.png"));
    configGeneralButton->setText(tr("General"));
    configGeneralButton->setTextAlignment(Qt::AlignHCenter);
    configGeneralButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *algorithmButton = new QListWidgetItem(m_pContentsWidget);
    //appearanceButton->setIcon(QIcon(":/images/appearance.png"));
    //algorithmButton->setText(tr("Algorithm"));
    algorithmButton->setTextAlignment(Qt::AlignHCenter);
    algorithmButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);    


    connect(m_pContentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(slotChangePage(QListWidgetItem*,QListWidgetItem*)));
}

void ConfigDialog::slotChangePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
    {
        current = previous;
    }

    m_pPagesWidget->setCurrentIndex(m_pContentsWidget->row(current));
}

void ConfigDialog::slotConfigDiagFinished()
{
    // TODO: Save settings.
    qDebug() << "ConfigDialog::onConfigDiagFinished()";
}
