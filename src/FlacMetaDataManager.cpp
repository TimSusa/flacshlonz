//////////////////////////////////////////////////////////////////////
//!\file FlacMetaDataManager.cpp
//!\brief Reads out Metadata from FLAC files.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#include "FlacMetaDataManager.hpp"
#include <QDebug>
#include <QCryptographicHash>

#include <string>
#include <QTextCodec>
#include <QTextStream>
#include <iostream>

FlacMetadataManager::FlacMetadataManager(const QString& path):  
    m_MetaDataValues(m_metadataKeys.size(),""),
    m_Path(path),
    m_isFlacValid(false)
{    
    initMetaDataHashSet();
    readMetaData();
}

void FlacMetadataManager::operator()(const QString& path)
{
    // try not to set clearAll in here!
    setPath(path);
    readMetaData();
}

void FlacMetadataManager::initMetaDataHashSet()
{
    //qDebug() << "FlacMetadataManager::initMetadataKeys";

    // the following is supposed to emulate an ordered hash / a list with string index
    // there might be better ways to do this

    m_LUT[ m_LabelFileName ] = m_metadataKeys.indexOf( m_LabelFileName );
    m_LUT[ m_LabelAudioMd5 ] = m_metadataKeys.indexOf( m_LabelAudioMd5 );
    m_LUT[ m_LabelChannels ] = m_metadataKeys.indexOf( m_LabelChannels );
    m_LUT[ m_LabelBitPerSample ] = m_metadataKeys.indexOf( m_LabelBitPerSample );
    m_LUT[ m_LabelSampleRate ] = m_metadataKeys.indexOf( m_LabelSampleRate );
    m_LUT[ m_LabelTotSamples ] = m_metadataKeys.indexOf( m_LabelTotSamples );
    m_LUT[ m_LabelBlockSizeMin ] = m_metadataKeys.indexOf( m_LabelBlockSizeMin );
    m_LUT[ m_LabelBlockSizeMax ] = m_metadataKeys.indexOf( m_LabelBlockSizeMax );
    m_LUT[ m_LabelFrameSizeMin ] = m_metadataKeys.indexOf( m_LabelFrameSizeMin );
    m_LUT[ m_LabelFrameSizeMax ] = m_metadataKeys.indexOf( m_LabelFrameSizeMax );
    m_LUT[ m_LabelEncoder ] = m_metadataKeys.indexOf( m_LabelEncoder );
    m_LUT[ m_LabelHasInvalidMetaData ] = m_metadataKeys.indexOf( m_LabelHasInvalidMetaData );
    m_LUT[ m_LabelHasPicture ] = m_metadataKeys.indexOf( m_LabelHasPicture );
    m_LUT[ m_LabelHasCueSheet ] = m_metadataKeys.indexOf( m_LabelHasCueSheet );
    m_LUT[ m_LabelVorbisComments ] = m_metadataKeys.indexOf( m_LabelVorbisComments );
    m_LUT[ m_LabelNrOfSeekPoints ] = m_metadataKeys.indexOf( m_LabelNrOfSeekPoints );
    //m_LUT[ m_LabelSeekpoints ] = m_metadataKeys.indexOf( m_LabelSeekpoints );

}

void FlacMetadataManager::readMetaData()
{
    if (m_Path.isEmpty())
    {
        clearAll();
        return;
    }

    if (isFlacValidByPath(QFile::encodeName(m_Path)))
    {
        calcFlacMetaData();
        calcTrackMd5();
    }
    else
    {
        clearAll();
    }
}

bool FlacMetadataManager::isFlacValidByPath(const char* filepath)
{
    //    qDebug() << "FlacMetadataManager::isFlacValid";
    bool ret = false;
    FLAC::Metadata::SimpleIterator iter;

    // Proof iterator
    if( ! iter.is_valid() )
    {
        ret = false;
        qDebug() << "Iterator is not valid -> " << filepath;
    }
    else
    {
        if( ! iter.init( filepath, true, true ) )
        {
            ret=false;
            qDebug() << "Cannot open Flac: " << filepath;
        }
        else
        {
            ret=true;
        }
    }
    m_isFlacValid = ret;
    return ret;
}

void FlacMetadataManager::setPath(const QString &path)
{
    m_Path = path;
}

void FlacMetadataManager::calcFlacMetaData()
{
    m_MetaDataValues[ m_LUT[ m_LabelHasCueSheet ] ] = QString::fromUtf8( "no" );
    m_MetaDataValues[ m_LUT[ m_LabelHasInvalidMetaData ] ] = QString::fromUtf8( "no" );
    m_MetaDataValues[ m_LUT[ m_LabelHasPicture ] ] = QString::fromUtf8( "no" );

    // Strip out basename
    if (m_MetaDataValues[ m_LUT[ m_LabelFileName ] ].isEmpty())
    {
        QString name = Utils::getBaseFromPath(m_Path);
        m_MetaDataValues[ m_LUT[ m_LabelFileName ] ] = name;
    }

    FLAC::Metadata::SimpleIterator iter;

    // Proof iterator
    if( ! iter.is_valid() )
    {
        onError( iter.status().as_cstring() );
    }
    else
    {
        if( !iter.init( QFile::encodeName(m_Path), true, true ) )
        {
            // TODO: Check, why kyrillic is not working for filepaths!
            qDebug() << "FlacMetadataManager::getMetaDataByFilePath: INIT FAILED FOR: " << m_Path;
        }
        else
        {
            // Iterate and extract blockwise
            while( ! iter.is_last() )
            {
                ::FLAC__MetadataType blockType = iter.get_block_type();
                FLAC::Metadata::Prototype *block = iter.get_block();

                if( ! block )
                {
                    onError( iter.status().as_cstring() );
                }
                if( blockType == ::FLAC__METADATA_TYPE_STREAMINFO )
                {
                    setStreamInfo(block);
                }
                else if( blockType == ::FLAC__METADATA_TYPE_PADDING )
                {
                    // qDebug() << "FLAC__METADATA_TYPE_PADDING" << "\n";
                }
                else if( blockType == ::FLAC__METADATA_TYPE_APPLICATION )
                {
                    FLAC::Metadata::Application *app =
                            dynamic_cast< FLAC::Metadata::Application * >( block );

                    QString appId = QString::fromLocal8Bit( reinterpret_cast< const char * >( app->get_id() ), sizeof( char ) * 4 );
                    QString appData = QString::fromLocal8Bit( reinterpret_cast< const char * >( app->get_data() ) );
                    /*
                    qDebug()
                            << "FLAC__METADATA_TYPE_APPLICATION: "
                            << appId
                            << " "
                            << "("
                            << appData
                            << ")"
                            << "\n";
        */
                    //m_MetaDataValues[ m_LUT[ m_LabelEncoder ] ].clear();

                    QTextStream textStream( &m_MetaDataValues[ m_LUT[ m_LabelEncoder ] ] );

                    textStream
                            << appId
                            << " "
                            << "("
                            << appData
                            << ")";
                }
                /*
                else if( blockType == ::FLAC__METADATA_TYPE_SEEKTABLE )
                {
                    setSeekTable(block);
                }
                */
                else if( blockType == ::FLAC__METADATA_TYPE_VORBIS_COMMENT )
                {
                    setVorbisComments(block);
                }
                else if( blockType == ::FLAC__METADATA_TYPE_CUESHEET )
                {
                    // qDebug() << "FLAC__METADATA_TYPE_CUESHEET" << "\n";

                    m_MetaDataValues[ m_LUT[ m_LabelHasCueSheet ] ] = QString::fromUtf8( "yes" );
                }
                else if( blockType == ::FLAC__METADATA_TYPE_PICTURE )
                {
                    // qDebug() << "FLAC__METADATA_TYPE_PICTURE" << "\n";

                    m_MetaDataValues[ m_LUT[ m_LabelHasPicture ] ] = QString::fromUtf8( "yes" );
                }
                else if( blockType == ::FLAC__METADATA_TYPE_UNDEFINED )
                {
                    //  qDebug() << "FLAC__METADATA_TYPE_UNDEFINED" << "\n";

                    m_MetaDataValues[ m_LUT[ m_LabelHasInvalidMetaData ] ] = QString::fromUtf8( "yes" );
                }

                delete block;

                if( ! iter.next() )
                {
                    onError( iter.status().as_cstring() );
                }
            }// End of filling metadataValues
        }
    }
}

void FlacMetadataManager::calcTrackMd5()
{
    QString metaTmp;
    vecToStr(m_MetaDataValues, metaTmp);
    QByteArray val = QCryptographicHash::hash((metaTmp.toUtf8()),QCryptographicHash::Md5).toHex();
    m_TrackMd5 = QString(val);
    //qDebug() << "FlacMetadataManager::getTrackMd5ByPath: " << filePath << " -> HASH: " << val.toHex() << " " << QString(val);
}

void FlacMetadataManager::clearAll()
{
    // Never ever call clear for m_LUT or m_MetadataValues!
    m_TrackMd5.clear();
    m_isFlacValid = false;
    for(int i = 0; i < m_MetaDataValues.size(); ++i)
    {
        m_MetaDataValues[i].clear();
    }
}

void FlacMetadataManager::setStreamInfo(FLAC::Metadata::Prototype *block)
{
    // Extract FLAC stream-info from block

    //      qDebug() << "FLAC__METADATA_TYPE_STREAMINFO" << "\n";

    if (!block)
        return;

    FLAC::Metadata::StreamInfo *streaminfo =
            dynamic_cast< FLAC::Metadata::StreamInfo* >( block );

    // Extract md5sum
    const FLAC__byte *md5sum = streaminfo->get_md5sum();

    // Format md5 to textstream
    QString md5sumString;
    QTextStream textStream( &md5sumString );

    textStream
            << hex
            << qSetPadChar( '0' )
            << qSetFieldWidth( 2 );

    for( int i = 0; i < 16; i++ )
    {
        textStream << static_cast< unsigned short >( md5sum[i] );  // do me better
    }

    // Set stream-info
    m_MetaDataValues[ m_LUT[ m_LabelAudioMd5 ] ] = md5sumString;
    m_MetaDataValues[ m_LUT[ m_LabelChannels ] ] = QString::number( streaminfo->get_channels() );
    m_MetaDataValues[ m_LUT[ m_LabelBitPerSample ] ] = QString::number( streaminfo->get_bits_per_sample() );
    m_MetaDataValues[ m_LUT[ m_LabelSampleRate ] ] = QString::number( streaminfo->get_sample_rate() );
    m_MetaDataValues[ m_LUT[ m_LabelTotSamples ] ] = QString::number( streaminfo->get_total_samples() );
    m_MetaDataValues[ m_LUT[ m_LabelBlockSizeMin ] ] = QString::number( streaminfo->get_min_blocksize() );
    m_MetaDataValues[ m_LUT[ m_LabelBlockSizeMax ] ] = QString::number( streaminfo->get_max_blocksize() );
    m_MetaDataValues[ m_LUT[ m_LabelFrameSizeMin ] ] = QString::number( streaminfo->get_min_framesize() );
    m_MetaDataValues[ m_LUT[ m_LabelFrameSizeMax ] ] = QString::number( streaminfo->get_max_framesize() );
}
/*
void FlacMetadataManager::setSeekTable(FLAC::Metadata::Prototype *block)
{
    //  qDebug() << "FLAC__METADATA_TYPE_SEEKTABLE" << "\n";

    FLAC::Metadata::SeekTable *seekTable =
            dynamic_cast< FLAC::Metadata::SeekTable * >( block );

    unsigned int numPoints = seekTable->get_num_points();
    m_MetaDataValues[ m_LUT[ m_LabelNrOfSeekPoints ] ] = QString::number( numPoints );

    m_MetaDataValues[ m_LUT[ m_LabelSeekpoints ] ].clear();
    for( unsigned int i = 0; i < numPoints; i++ )
    {
        FLAC__StreamMetadata_SeekPoint seekPoint = seekTable->get_point( i );
        QTextStream textStream( &m_MetaDataValues[ m_LUT[ m_LabelSeekpoints ] ] );
        QString lineEnding( "\n" );
        textStream
                << QString::number( seekPoint.sample_number )
                << QString::fromUtf8( "/" )
                << QString::number( seekPoint.stream_offset )
                << QString::fromUtf8( "/" )
                << QString::number( seekPoint.frame_samples )
                << lineEnding;
    }
    m_MetaDataValues[ m_LUT[ m_LabelSeekpoints ] ].chop( 1 );
}
*/
void FlacMetadataManager::setVorbisComments(FLAC::Metadata::Prototype *block)
{
    // qDebug() << "FLAC__METADATA_TYPE_VORBIS_COMMENT";

    FLAC::Metadata::VorbisComment *tags =
            dynamic_cast< FLAC::Metadata::VorbisComment * >( block );

    QString vorbisComments;

    for( unsigned int i = 0; i < tags->get_num_comments(); i++ )
    {
        FLAC::Metadata::VorbisComment::Entry entry = tags->get_comment( i );
        QTextStream textStream ( &vorbisComments );
        QString lineEnding( "\n" );
        textStream
                << QString::fromUtf8( entry.get_field_name() )
                << QString::fromUtf8( " : \"" )
                << QString::fromUtf8( entry.get_field_value() )
                << "\""
                << lineEnding;
    }
    vorbisComments.chop( 1 );
    m_MetaDataValues[ m_LUT[ m_LabelVorbisComments ] ] = vorbisComments;
}

bool FlacMetadataManager::isFlacValid()
{
    return m_isFlacValid;
}

QString FlacMetadataManager::getAudioMd5()
{
    return m_MetaDataValues[m_LUT[m_LabelAudioMd5]];
}

QVector< QString > FlacMetadataManager::getMetaDataValues()
{    
    return m_MetaDataValues;
}

QString FlacMetadataManager::getTrackMd5By()
{    
    return m_TrackMd5;
}

void FlacMetadataManager::extractAudioMd5()
{
    QString retVal = "";

    // Create FLAC metadata iterator to extract a block per cycle
    FLAC::Metadata::SimpleIterator iter;

    // Proof iterator
    if( ! iter.is_valid() )
    {
        Utils::onError( iter.status().as_cstring() );
        retVal="";
    }
    else
    {
        // Initialize iterator
        if( (!iter.init( QFile::encodeName(m_Path), true, true )))// && (iter.status() != ::FLAC__METADATA_SIMPLE_ITERATOR_STATUS_NOT_A_FLAC_FILE) )
        {
            onError( iter.status().as_cstring() );
            retVal="";
        }
        else
        {
            ::FLAC__MetadataType blockType = iter.get_block_type();
            FLAC::Metadata::Prototype *block = iter.get_block();

            if( ! block )
            {
                onError( iter.status().as_cstring() );
                retVal="";
            }
            else
            {
                // Extract FLAC stream-info from block
                if ( blockType == ::FLAC__METADATA_TYPE_STREAMINFO )
                {
                    FLAC::Metadata::StreamInfo *streaminfo =
                            dynamic_cast< FLAC::Metadata::StreamInfo * >( block );

                    // Extract md5sum
                    const FLAC__byte *md5sum = streaminfo->get_md5sum();

                    // Format md5 to textstream
                    QString md5sumString;
                    QTextStream textStream( &md5sumString );

                    textStream
                            << hex
                            << qSetPadChar( '0' )
                            << qSetFieldWidth( 2 );

                    for( size_t i = 0; i < 16; i++ )
                    {
                        textStream << static_cast< unsigned short >( md5sum[i] );  // do me better
                    }
                    retVal = md5sumString;
                }
            }

            // clean up
            if (block)
                delete block;
            else
                qDebug() << "Block invalid!\n";
        }
    }
}
