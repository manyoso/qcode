#ifndef OPENEDFILESMODEL_H
#define OPENEDFILESMODEL_H

#include <QHash>
#include <QAbstractListModel>

#include "qcode.h"

class OpenedFile;

class OpenedFilesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    OpenedFilesModel(QObject *parent = 0);
    virtual ~OpenedFilesModel();

    void addFile(const QString &path);
    void removeFile(const QString &path);
    void addFiles(const QStringList &paths);
    void removeFiles(const QStringList &paths);

    bool contains(const QString &path) const;

    QWidget *widget(const QString &path) const;
    void setWidget(const QString &path, QWidget *widget);

    QCode::FileStatus status(const QString &path) const;
    void setStatus(const QString &path, QCode::FileStatus status);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role) const;

private:
    QList<QString> m_files;
    QHash<QString, OpenedFile*> m_filesHash;
};

#endif
