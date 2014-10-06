//////////////////////////////////////////////////////////////////////
//!\file FileStorageFlac.hpp
//!\brief Stores a FlacTrack.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////


#ifndef PERSISTENCE_HPP
#define PERSISTENCE_HPP

#include <QSettings>

class Persistence
{
public:
    Persistence();
    void initByPath(const QString& path);


    // GENERAL_SETTINGS -> ENABLE_LAST_OPEN_FILE.
    void persistLastOpenCollectionPathEnabled(bool enable);
    void setLastOpenCollectionPathEnabled(bool enable);
    bool getLastOpenCollectionPathEnabled()const;

    // GENERAL_SETTINGS -> LAST_OPEN_FILE.
    void persistLastOpenCollectionPath(const QString& path);
    void setLastOpenCollectionPath(const QString& path);
    const QString& getLastOpenCollectionPath()const;

    // GENERAL_SETTINGS -> ENABLE_WIZARD.
    void persistEnableWizard(bool enable);
    void setEnableWizard(bool enable);
    bool getEnableWizard()const;

    // GENERAL_SETTINGS -> ENABLE_DB_IMPORT_TO
//    void persistEnableDbImportTo(bool enable);
//    void setEnableDbImportTo(bool enable);
//    bool getEnableDbImportTo()const;

    // GENERAL_SETTINGS -> ENABLE_HYPNOAD
    void persistEnableHypnotoad(bool enable);
    void setEnableHypnoad(bool enable);
    bool getEnableHypnotoad()const;

    // ALGORITHM -> ENABLE_DEEP_SCAN.
    void persistEnableDeepScan(bool enable);
    void setEnableDeepScan(bool enable);
    bool getEnableDeepScan()const;

private:
    static const char* s_FileName;
    static const char* s_SectionGeneral; // Section
    static const char* s_KeyEnableLastOpenFile;
    static const char* s_KeyLastOpenFile;
    static const char* s_KeyEnableWizard;
    static const char* s_KeyEnableDbImportTo;
    static const char* s_KeyEnableHypnoad;

    static const char* s_SectionAlgorithm; // Section
    static const char* s_KeyEnableDeepScan;

    QSettings m_Settings;

    // Key-Values:
    bool    m_LastOpenFileEnabled;
    QString m_LastOpenFile;
    bool    m_EnableWizard;
    bool    m_EnableDbImportTo;
    bool    m_EnableHypnoad;

    bool    m_EnableDeepScan;
};

#endif // PERSISTENCE_HPP
