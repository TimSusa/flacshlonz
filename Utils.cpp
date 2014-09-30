//////////////////////////////////////////////////////////////////////
//!\file Utils.cpp
//!\brief Contains some helpers and utilities.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////


#include "Utils.hpp"
#include <QMessageBox>
#include <QDebug>
#include <QStringList>

#include <string>
#include <QTextCodec>
#include <QTextStream>
#include <iostream>

Utils::Utils()
{    
}

void Utils::onError( const char *msg )
{
    qDebug() << "Utils::onErrorAndExit: " << msg;

    QMessageBox *msgBox = new QMessageBox;
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setText("Error! Please copy this message and send it to your vendor: "  + QString(msg));
    msgBox->setInformativeText("Would you really like to continue?");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::No);

    int ret = msgBox->exec();
    switch (ret)
    {
    case QMessageBox::Yes:
        qDebug() << "Yes was clicked!";
        break;
    case QMessageBox::No:
        qDebug() << "No was clicked!";
        exit( EXIT_FAILURE );
        break;
    default:
        qDebug() << ret << " was clicked... this should never been happen!";
        // should never be reached
        break;
    }
}

void Utils::vecToStr(const QVector< QString >& vec, QString& str)
{
    for ( int i = 1; i< vec.size(); ++i )
    {
        str += vec[i];
    }
}

bool Utils::compareVectorOfStrings(const QVector< QString > &l, const QVector< QString > &r)
{
    return compareVectors(l,r);
}

bool Utils::compareVectorOfFlacTracks(const QVector< QVector< QString > >&l, const QVector< QVector< QString > >&r)
{
    return compareVectors(l,r);
}

QString Utils::getBaseFromPath(const QString &path)
{
    QStringList list = path.split("/");
    QString name = list.last();
    return name;
}

const QString Utils::getBuildDate() const
{
    QString dat=QString::fromLocal8Bit(__DATE__) + " " + QString::fromLocal8Bit(__TIME__);
    return dat;
}
/*
const char* Utils::getUtf8Thing(QString path)
{

    QTextCodec *codec = QTextCodec::codecForName("UTF8");
    QByteArray encodedString = codec->fromUnicode(path);

    return encodedString;
}
*/

template < typename T >
bool Utils::compareVectors(const QVector< T >&l, const QVector< T >&r)
{
    bool isEqual = false;
    if ( l.size() < r.size() )
        isEqual = std::equal ( l.begin(), l.end(), r.begin() );
    else
        isEqual = std::equal ( r.begin(), r.end(), l.begin() );
    return isEqual;
}


void Utils::testVectorComparison()
{
    QVector< QVector< QString > > l;
    QVector< QVector< QString > > r;
    l.push_back(FlacTrack("xyz","200","300","400").getAsVec());
    r.push_back(FlacTrack("xyz","200","300","400").getAsVec());

    l.push_back(FlacTrack("5","6","7","8").getAsVec());
    r.push_back(FlacTrack("5","6","7","8").getAsVec());

    l.push_back(FlacTrack("90000","100000","110000","120000").getAsVec());
    r.push_back(FlacTrack("90000","100000","110000","120000").getAsVec());

    qDebug() << "COMPARE VECTORS: true? = " << compareVectorOfFlacTracks(l,r);

    l.clear();
    r.clear();

    l.push_back(FlacTrack("1","2","3","4").getAsVec());
    r.push_back(FlacTrack("5","6","7","8").getAsVec());

    l.push_back(FlacTrack("9","10","11","12").getAsVec());
    r.push_back(FlacTrack("5","6","7","8").getAsVec());

    l.push_back(FlacTrack("9","10","11","12").getAsVec());
    r.push_back(FlacTrack("5","6","7","8").getAsVec());

    qDebug() << "COMPARE VECTORS: false? = " << compareVectorOfFlacTracks(l,r);
}
