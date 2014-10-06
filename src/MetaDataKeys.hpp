//////////////////////////////////////////////////////////////////////
//!\file MetaDataKeys.hpp
//!\brief Used as LUT.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef METADATAKEYS_HPP
#define METADATAKEYS_HPP

#include <QVector>
#include <QString>

class MetaDataKeys
{
public:
    MetaDataKeys();

    QVector< QString > m_metadataKeys;
    QString m_LabelFileName;
    QString m_Path;
    QString m_LabelAudioMd5;
    QString m_LabelChannels;
    QString m_LabelBitPerSample;
    QString m_LabelSampleRate;
    QString m_LabelTotSamples;
    QString m_LabelBlockSizeMin;
    QString m_LabelBlockSizeMax;
    QString m_LabelFrameSizeMin;
    QString m_LabelFrameSizeMax;
    QString m_LabelEncoder;
    QString m_LabelHasInvalidMetaData;
    QString m_LabelHasPicture;
    QString m_LabelHasCueSheet;
    QString m_LabelVorbisComments;
    QString m_LabelNrOfSeekPoints;
    //QString m_LabelSeekpoints;
};

#endif // METADATAKEYS_HPP
