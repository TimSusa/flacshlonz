//////////////////////////////////////////////////////////////////////
//!\file  ConfigDialog.hpp
//!\brief definition of class ConfigDialog.
//////////////////////////////////////////////////////////////////////

#ifndef CONFIGDIALOG_HPP
#define CONFIGDIALOG_HPP

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include "ConfigPageGeneral.hpp"
#include "ConfigPageAlgorithm.hpp"

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigDialog();

signals:
    void sigEnableWizard(bool isChecked);
    void sigEnableLastOpenedFile(bool isChecked);
    void sigEnableHypnotoad(bool isChecked);

public slots:
    void slotChangePage(QListWidgetItem *current, QListWidgetItem *previous);
    void slotConfigDiagFinished();

private:
    void createLeftPane();

    QListWidget *m_pContentsWidget;
    QStackedWidget *m_pPagesWidget;
    ConfigPageGeneral *m_pConfigPageGeneral;
    ConfigPageAlgorithm *m_pConfigPageAlgorithm;
};

#endif
