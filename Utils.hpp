//////////////////////////////////////////////////////////////////////
//!\file Utils.hpp
//!\brief Contains some helpers and utilities.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef UTILS_HPP
#define UTILS_HPP

#include <QString>
#include <QVector>
#include "FlacTrack.hpp"

class Utils
{
public:
    Utils();
    static void onError( const char *msg );
    void vecToStr(const QVector< QString >& vec, QString& str);
    bool compareVectorOfStrings(const QVector< QString >& l, const QVector< QString >& r);
    bool compareVectorOfFlacTracks(const QVector< QVector< QString > >& l, const QVector< QVector< QString > >& r);
    static QString getBaseFromPath(const QString& path);
    const QString getBuildDate()const;
    //const char* getUtf8Thing(QString path);

private:
    template < typename T >
    bool compareVectors(const QVector< T >&l, const QVector< T >&r);
    void testVectorComparison();
};

#endif // UTILS_HPP
