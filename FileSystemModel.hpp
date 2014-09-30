//////////////////////////////////////////////////////////////////////
//!\file FileSystemModel.hpp
//!\brief Subclassed QFileSystemModel.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef FILESYSTEMMODEL_HPP
#define FILESYSTEMMODEL_HPP

#include <QFileSystemModel>
#include <QString>

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit FileSystemModel(QObject *parent = 0);
    static const QStringList s_FileFilter;
    void init(const QString& filePath);
    void setHeaderTxt(const QString& txt);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public slots:

private:
    QString m_HeaderText;
};

#endif // FILESYSTEMMODEL_HPP
