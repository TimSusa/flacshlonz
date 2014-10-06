//////////////////////////////////////////////////////////////////////
//!\file FreeDiskStorage.cpp
//!\brief Shows free amount of disk space on win32 and unix.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#include "FreeDiskStorage.hpp"
#include <QDebug>
#include <QStringList>

FreeDiskStorage::FreeDiskStorage()
{
}

FreeDiskStorageInfo FreeDiskStorage::getFreeStorageInfo(const QString &path) const
{
    //qDebug() << "FreeDiskStorage::getFreeStorageInfo: " << path;
    QString tmp;
#ifdef WIN32
    if (path.isEmpty())
        tmp = "C:\\";
    else
        tmp = path;
    return getFreeStorageInfoWin(tmp.left(2).toStdString().c_str());
#elif unix
    if (path.isEmpty())
        tmp= "/";
    else
        tmp = path;
    return getFreeStorageInfoUnix(tmp);
#endif
    return FreeDiskStorageInfo(0,0);
}

#ifdef unix
FreeDiskStorageInfo FreeDiskStorage::getFreeStorageInfoUnix(const QString &path) const
{
    //qDebug()  << "FreeDiskStorage::getFreeStorageInfoUnix() " << path;

    struct statvfs buf;
    unsigned long long bitWidth= 1024*1024; // MByte

    if (!statvfs(path.toStdString().c_str(), &buf))
    {
        unsigned long long blksize, blocks, freeblks, disk_size, free; //,used;
        blksize = buf.f_bsize;
        blocks = buf.f_blocks;
        freeblks = buf.f_bfree;

        disk_size = (blocks * blksize) / bitWidth;
        free = (freeblks * blksize) / bitWidth;
        //used = ( disk_size - free) / bitWidth;

        return FreeDiskStorageInfo( disk_size , free );
    }
    else
    {
        return FreeDiskStorageInfo(0,0);
    }
}
#endif

#ifdef WIN32
/*
 * getDiskFreeSpaceInGB
 *
 * Returns the amount of free drive space for the given drive in GB. The
 * value is rounded to the nearest integer value.
 */
FreeDiskStorageInfo FreeDiskStorage::getFreeStorageInfoWin( LPCSTR drive )const
{
    ULARGE_INTEGER freeBytesToCaller;
    ULARGE_INTEGER freeBytesTotal;
    freeBytesToCaller.QuadPart = 0L;
    unsigned long long B_per_GB = 1024*1024; // MByte

    if( !GetDiskFreeSpaceEx( drive, &freeBytesToCaller, &freeBytesTotal, NULL ) )
    {
        qDebug() << "ERROR: Call to GetDiskFreeSpaceEx() failed.";
    }

    unsigned long long freeSpace_gb = freeBytesToCaller.QuadPart / B_per_GB;
    unsigned long long totalSpace_gb = freeBytesTotal.QuadPart / B_per_GB;
    return FreeDiskStorageInfo(totalSpace_gb, freeSpace_gb);
}
#endif
