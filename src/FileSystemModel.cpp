//////////////////////////////////////////////////////////////////////
//!\file FileSystemModel.cpp
//!\brief Subclassed QFileSystemModel.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#include "FileSystemModel.hpp"
#include <QDebug>

// Change file-endings to search for in file-system.
// Note: Changes here will affect whole the application.
const QStringList FileSystemModel::s_FileFilter = QStringList() << QString( "*.flac");

FileSystemModel::FileSystemModel(QObject *parent) :
    QFileSystemModel(parent)
{
}

void FileSystemModel::init( const QString& filePath)
{
    qDebug() << "FileSystemModel::initFSModel";
    setHeaderTxt(tr("Music Collection"));
    this->setRootPath( filePath );
    this->setReadOnly( true );
    this->setFilter
            (
                QDir::AllDirs
                | QDir::Drives
                | QDir::NoSymLinks
                | QDir::NoDotAndDotDot
                | QDir::Files
                );

    this->setNameFilters( FileSystemModel::s_FileFilter );
    this->setNameFilterDisables( false );
    this->sort( 0 );
}

void FileSystemModel::setHeaderTxt(const QString& txt)
{
    m_HeaderText = txt;
}

// In order to change header texts, we have to subclass here.
QVariant FileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((section == 0) && (role == Qt::DisplayRole))
    {
        return m_HeaderText;
    } else {
        return QFileSystemModel::headerData(section,orientation,role);
    }
}
