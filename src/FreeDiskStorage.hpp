#ifndef FREEDISKSTORAGE_HPP
#define FREEDISKSTORAGE_HPP

#ifdef WIN32
#include "Windows.h"
#elif unix
#include <sys/statvfs.h>
#endif

#include <QString>


struct FreeDiskStorageInfo
{
    FreeDiskStorageInfo(unsigned long long tAS, unsigned long long fAS):
        totalAmountOfSpace(tAS),
        freeAmountOfSpace(fAS)
    {}
    unsigned long long totalAmountOfSpace;
    unsigned long long freeAmountOfSpace;
};

class FreeDiskStorage
{
public:
    FreeDiskStorage();
    FreeDiskStorageInfo getFreeStorageInfo(const QString& path) const;

private:
#ifdef WIN32
    FreeDiskStorageInfo getFreeStorageInfoWin( LPCSTR drive )const;
#elif unix
    FreeDiskStorageInfo getFreeStorageInfoUnix(const QString &path)const;
#endif

};

#endif // FREEDISKSTORAGE_HPP
