#include "openedfilesmodel.h"

#include <QPointer>
#include <QWidget>
#include <QFileInfo>

struct OpenedFile {
    OpenedFile() { status = QCode::FileNormal; }
    QCode::FileStatus status;
    QPointer<QWidget> widget;
};

OpenedFilesModel::OpenedFilesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

OpenedFilesModel::~OpenedFilesModel()
{
    qDeleteAll(m_filesHash);
}

void OpenedFilesModel::addFile(const QString &path)
{
    addFiles(QStringList(path));
}

void OpenedFilesModel::removeFile(const QString &path)
{
    int row = m_files.indexOf(path);
    if (row < 0)
        return;

    beginRemoveRows(QModelIndex(), row, row);

    m_files.removeAt(row);
    OpenedFile *f = m_filesHash.take(path);
    delete f;

    endRemoveRows();
}

void OpenedFilesModel::addFiles(const QStringList &paths)
{
    int row = m_files.count();
    beginInsertRows(QModelIndex(), row, row + paths.count() - 1);

    QStringList::ConstIterator it = paths.begin();
    for(; it != paths.end(); ++it) {
        m_files.append(*it);
        m_filesHash.insert(*it, new OpenedFile());
    }

    endInsertRows();
}

void OpenedFilesModel::removeFiles(const QStringList &paths)
{
    foreach(QString path, paths)
        removeFile(path);
}


bool OpenedFilesModel::contains(const QString &path) const
{
    return m_filesHash.contains(path);
}

QWidget *OpenedFilesModel::widget(const QString &path) const
{
    if (m_filesHash.contains(path))
        return m_filesHash.value(path)->widget;
    else
        return 0;
}

void OpenedFilesModel::setWidget(const QString &path, QWidget *widget)
{
    if (m_filesHash.contains(path)) {
        int row = m_files.indexOf(path);
        m_filesHash.value(path)->widget = widget;
        QModelIndex index = createIndex(row, 0);
        emit dataChanged(index, index);
    }
}

QCode::FileStatus OpenedFilesModel::status(const QString &path) const
{
    if (m_filesHash.contains(path))
        return m_filesHash.value(path)->status;
    else
        return QCode::FileStatus(-1);
}

void OpenedFilesModel::setStatus(const QString &path, QCode::FileStatus status)
{
    if (m_filesHash.contains(path)) {
        int row = m_files.indexOf(path);
        m_filesHash.value(path)->status = status;
        QModelIndex index = createIndex(row, 0);
        emit dataChanged(index, index);
    }
}

int OpenedFilesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_files.count();
}

QVariant OpenedFilesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_files.size())
        return QVariant();

    QFileInfo info(m_files.at(index.row()));

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return info.fileName();
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
        return info.filePath();
    //FIXME DECORATIONS!
    default:
        return QVariant();
    }
}
