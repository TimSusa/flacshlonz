#include "MetaDataKeys.hpp"

MetaDataKeys::MetaDataKeys():
    m_metadataKeys(),
    m_LabelFileName( "File" ),
    m_LabelAudioMd5( "Audio MD5" ),
    m_LabelChannels( "Channels" ),
    m_LabelBitPerSample( "Bits per sample" ),
    m_LabelSampleRate( "Samplerate" ),
    m_LabelTotSamples( "Total samples" ),
    m_LabelBlockSizeMin( "Min. blocksize" ),
    m_LabelBlockSizeMax( "Max. blocksize" ),
    m_LabelFrameSizeMin( "Min. framesize" ),
    m_LabelFrameSizeMax( "Max. framesize" ),
    m_LabelEncoder( "Encoder" ),
    m_LabelHasInvalidMetaData( "Has invalid metadata" ),
    m_LabelHasPicture( "Has picture" ),
    m_LabelHasCueSheet( "Has CUE sheet" ),
    m_LabelVorbisComments( "Vorbis Comments" ),
    m_LabelNrOfSeekPoints( "Number of seekpoints" ),
    // 	m_seekpointsHdrLabel( "Seekpoints ( sample number/stream offset/frame samples)" )
    m_LabelSeekpoints( "Seekpoints" )
{
    m_metadataKeys
            << m_LabelFileName
            << m_LabelAudioMd5
            << m_LabelChannels
            << m_LabelBitPerSample
            << m_LabelSampleRate
            << m_LabelTotSamples
            << m_LabelBlockSizeMin
            << m_LabelBlockSizeMax
            << m_LabelFrameSizeMin
            << m_LabelFrameSizeMax
            << m_LabelEncoder
            << m_LabelHasInvalidMetaData
            << m_LabelHasPicture
            << m_LabelHasCueSheet
            << m_LabelVorbisComments
            << m_LabelNrOfSeekPoints
            << m_LabelSeekpoints;
}
