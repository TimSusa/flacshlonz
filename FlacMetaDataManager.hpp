//////////////////////////////////////////////////////////////////////
//!\file FlacMetaDataManager.hpp
//!\brief Reads out Metadata from FLAC files.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef FLACMETADATAMANAGER_HPP
#define FLACMETADATAMANAGER_HPP

#include "Utils.hpp"
#include <QString>
#include <QVector>
#include <QFileInfo>
#include <QHash>
#include "MetaDataKeys.hpp"
#include <FLAC++/metadata.h>

class FlacMetadataManager: public MetaDataKeys, public Utils
{
public:
    FlacMetadataManager(const QString& path);
    void operator()(const QString& path);
    bool isFlacValid();
    QString getAudioMd5();
    QVector< QString > getMetaDataValues();
    QString getTrackMd5By();
    QHash< QString, int > m_LUT;
    QVector< QString > m_MetaDataValues;

private:
    QString m_Path;
    bool m_isFlacValid;
    QString m_TrackMd5;

    void initMetaDataHashSet();
    void readMetaData();
    bool isFlacValidByPath(const char* filepath);
    void setPath(const QString& path);
    void calcFlacMetaData();
    void calcTrackMd5();
    void clearAll();
    void setStreamInfo( FLAC::Metadata::Prototype *block);
    void setSeekTable( FLAC::Metadata::Prototype *block);
    void setVorbisComments(FLAC::Metadata::Prototype *block);
    void extractAudioMd5();
};

#endif // FLACMETADATAMANAGER_HPP

