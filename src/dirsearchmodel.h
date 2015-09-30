#ifndef DIRSEARCHMODEL_H
#define DIRSEARCHMODEL_H

#include <QAbstractListModel>

class QFileSystemWatcher;

class DirSearchModel : public QAbstractListModel
{
    Q_OBJECT
public:
    DirSearchModel(QObject *parent = 0);
    DirSearchModel(const QString &rootPath, QObject *parent = 0);
    virtual ~DirSearchModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role) const;

    void setRootPath(const QString &rootPath);

private Q_SLOTS:
    void resetFileList(const QString &rootPath);

private:
    void commonConstructor();

private:
    QList<QString> m_files;
    QFileSystemWatcher *m_rootWatch;
};

#endif
