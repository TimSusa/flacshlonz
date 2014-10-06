#include "Persistence.hpp"
#include <QFile>
#include <QStringList>
#include <QDebug>

// Filename for ini-File.
const char* Persistence::s_FileName = "settings.ini";

// Section 'GENERAL_SETTINGS'.
const char* Persistence::s_SectionGeneral = "GENERAL_SETTINGS";
const char* Persistence::s_KeyEnableLastOpenFile = "ENABLE_LAST_OPEN_FILE";
const char* Persistence::s_KeyLastOpenFile = "LAST_OPEN_FILE";
const char* Persistence::s_KeyEnableWizard = "ENABLE_WIZARD";
const char* Persistence::s_KeyEnableDbImportTo = "ENABLE_DB_IMPORT_TO";
const char* Persistence::s_KeyEnableHypnoad = "ENABLE_HYPNOAD";

// Section 'ALGORITHM'
const char* Persistence::s_SectionAlgorithm = "ALGORITHM";
const char* Persistence::s_KeyEnableDeepScan = "ENABLE_DEEP_SCAN";

// CTOR
Persistence::Persistence():
    m_Settings(s_FileName, QSettings::IniFormat),
    m_LastOpenFileEnabled(),
    m_LastOpenFile(),
    m_EnableWizard(true),
    m_EnableDbImportTo(false),
    m_EnableDeepScan(true),
    m_EnableHypnoad(false)
{
    initByPath(s_FileName);
}

void Persistence::initByPath(const QString& path)
{
    QFile file(path);

    if(file.exists())
    {
        // Section 'GENERAL_SETTINGS'
        m_Settings.beginGroup(s_SectionGeneral);
        const QStringList childKeyListGeneral = m_Settings.childKeys();
        foreach (const QString &childKey, childKeyListGeneral)
        {
            if ( childKey == s_KeyEnableLastOpenFile ) // ENABLE_LAST_OPEN_FILE
            {
                setLastOpenCollectionPathEnabled( m_Settings.value(childKey).value<bool>() );
            }

            if ( childKey == s_KeyLastOpenFile ) // LAST_OPEN_FILE
            {
                setLastOpenCollectionPath( m_Settings.value(childKey).value<QString>() );
            }

            if ( childKey == s_KeyEnableWizard ) // ENABLE_WIZARD
            {
                setEnableWizard( m_Settings.value(childKey).value<bool>() );
            }
/*
            if ( childKey == s_KeyEnableDbImportTo ) // ENABLE_DB_IMPORT_TO
            {
                setEnableDbImportTo( m_Settings.value(childKey).value<bool>() );
            }
*/
            if ( childKey == s_KeyEnableHypnoad ) // ENABLE_HYPNOAD
            {
                setEnableHypnoad( m_Settings.value(childKey).value<bool>() );
            }

            // more keys...
        }
        m_Settings.endGroup();

        // Section 'ALGORITHM'
        m_Settings.beginGroup(s_SectionAlgorithm);
        const QStringList childKeyListAlgorithm = m_Settings.childKeys();
        foreach (const QString &childKey, childKeyListAlgorithm)
        {
            if ( childKey == s_KeyEnableDeepScan ) // ENABLE_DEEP_SCAN
            {
                setEnableDeepScan( m_Settings.value(childKey).value<bool>() );
            }

            // more keys...
        }
        m_Settings.endGroup();
    }
    file.close();
}

// GENERAL_SETTINGS -> ENABLE_LAST_OPEN_FILE.
void Persistence::persistLastOpenCollectionPathEnabled(bool enable)
{
    m_Settings.beginGroup(s_SectionGeneral);
    m_Settings.setValue(s_KeyEnableLastOpenFile, enable);
    setLastOpenCollectionPathEnabled(enable);
    m_Settings.endGroup();
}
void Persistence::setLastOpenCollectionPathEnabled(bool enable)
{
    m_LastOpenFileEnabled = enable;
}
bool Persistence::getLastOpenCollectionPathEnabled()const
{
    return m_LastOpenFileEnabled;
}

// GENERAL_SETTINGS -> LAST_OPEN_FILE.
void Persistence::persistLastOpenCollectionPath(const QString& path)
{
    qDebug() << "Persistence::persistLastOpenFile";
    m_Settings.beginGroup(s_SectionGeneral);
    m_Settings.setValue(s_KeyLastOpenFile, path);
    setLastOpenCollectionPath(path);
    m_Settings.endGroup();
}

void Persistence::setLastOpenCollectionPath(const QString& path)
{
    m_LastOpenFile = path;
}

const QString& Persistence::getLastOpenCollectionPath()const
{
    return m_LastOpenFile;
}

void Persistence::persistEnableWizard(bool enable)
{
    qDebug() << "Persistence::persistEnableWizard";
    m_Settings.beginGroup(s_SectionGeneral);
    m_Settings.setValue(s_KeyEnableWizard, enable);
    setEnableWizard(enable);
    m_Settings.endGroup();
}

void Persistence::setEnableWizard(bool enable)
{
    m_EnableWizard = enable;
}

bool Persistence::getEnableWizard() const
{
    return m_EnableWizard;
}
/*
void Persistence::persistEnableDbImportTo(bool enable)
{
    qDebug() << "Persistence::persistEnableDbQueryBox";
    m_Settings.beginGroup(s_SectionGeneral);
    m_Settings.setValue(s_KeyEnableDbImportTo, enable);
    setEnableDbImportTo(enable);
    m_Settings.endGroup();
}

void Persistence::setEnableDbImportTo(bool enable)
{
    m_EnableDbImportTo = enable;
}

bool Persistence::getEnableDbImportTo() const
{
    return m_EnableDbImportTo;
}
*/
void Persistence::persistEnableHypnotoad(bool enable)
{
    m_Settings.beginGroup(s_SectionGeneral);
    m_Settings.setValue(s_KeyEnableHypnoad, enable);
    setEnableHypnoad(enable);
    m_Settings.endGroup();
}

void Persistence::setEnableHypnoad(bool enable)
{
    m_EnableHypnoad = enable;
}

bool Persistence::getEnableHypnotoad()const
{
    return m_EnableHypnoad;
}

// ALGORITHM -> ENABLE_DEEP_SCAN.
void Persistence::persistEnableDeepScan(bool enable)
{
    qDebug() << "Persistence::persistEnableDeepScan";
    m_Settings.beginGroup(s_SectionAlgorithm);
    m_Settings.setValue(s_KeyEnableDeepScan, enable);
    setEnableDeepScan(enable);
    m_Settings.endGroup();
}

void Persistence::setEnableDeepScan(bool enable)
{
    m_EnableDeepScan = enable;
}

bool Persistence::getEnableDeepScan() const
{
    return m_EnableDeepScan;
}

