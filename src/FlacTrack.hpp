//////////////////////////////////////////////////////////////////////
//!\file FlacTrack.hpp
//!\brief Contains metadata for comparison.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef FLACTRACK_HPP
#define FLACTRACK_HPP

#include <QFileInfo>
#include <QString>
#include <QVector>

class FlacTrack
{
public:
    FlacTrack():
        m_AlbumMd5(),
        m_AudioMd5(),
        m_File(),
        m_MetaDataMd5()
    {
        // CTOR
    }
    FlacTrack(const QString albumMd5, const QString audioMd5 ,const QString& fi, const QString& metaDataMd5):
        m_AlbumMd5(albumMd5),
        m_AudioMd5(audioMd5),
        m_File(fi),
        m_MetaDataMd5(metaDataMd5)
    {
        // CCTOR
    }
    void operator()(const QString trackId, const QString md5 ,const QString& fi, const QString& stamp)
    {
        m_AlbumMd5=trackId;
        m_AudioMd5=md5;
        m_File=fi;
        m_MetaDataMd5=stamp;
    }
    const FlacTrack& get()const
    {
        return *this;
    }
    bool compare(const FlacTrack &t)const
    {
        bool retVal = false;
        if ( m_AlbumMd5 == t.m_AlbumMd5 )
        {
            if ( m_AudioMd5 == t.m_AudioMd5 )
            {
                if ( m_File == t.m_File )
                {
                    if ( m_MetaDataMd5 == t.m_MetaDataMd5 )
                    {
                        retVal = true;
                    }
                }
            }
        }
        return retVal;
    }
    QVector< QString > getAsVec()const
    {
        QVector< QString > l;
        l.push_back(m_AlbumMd5);
        l.push_back(m_AudioMd5);
        l.push_back(m_File);
        l.push_back(m_MetaDataMd5);
        return l;
    }
    QString m_AlbumMd5;
    QString m_AudioMd5;
    QString m_File;
    QString m_MetaDataMd5;
};

#endif // FLACTRACK_HPP
