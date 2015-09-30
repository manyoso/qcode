#include "dirsearchmodel.h"

#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QFileSystemWatcher>

DirSearchModel::DirSearchModel(QObject *parent)
    : QAbstractListModel(parent)
{
    commonConstructor();
}

DirSearchModel::DirSearchModel(const QString &rootPath, QObject *parent)
    : QAbstractListModel(parent)
{
    commonConstructor();
    resetFileList(rootPath);
}

void DirSearchModel::commonConstructor()
{
    m_rootWatch = new QFileSystemWatcher(this);
    connect(m_rootWatch, SIGNAL(directoryChanged(const QString &)),
            this, SLOT(resetFileList(const QString &)));
}

DirSearchModel::~DirSearchModel()
{
}

int DirSearchModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_files.count();
}

int DirSearchModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant DirSearchModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_files.size())
        return QVariant();

    QFileInfo info(m_files.at(index.row()));
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (!index.column())
            return info.fileName();
        else
            return info.filePath();
    } else if (role == Qt::ToolTipRole || role == Qt::StatusTipRole) {
        if (!index.column())
            return info.filePath();
        else
            return info.fileName();
    }

    return QVariant();
}

void DirSearchModel::setRootPath(const QString &rootPath)
{
    resetFileList(rootPath);
    reset();
}

void DirSearchModel::resetFileList(const QString &rootPath)
{
    //reset the watch
    if (!m_rootWatch->directories().isEmpty())
        m_rootWatch->removePaths(m_rootWatch->directories());
    m_rootWatch->addPath(rootPath);

    //clear the files
    m_files.clear();

    //reset the list
    QDirIterator root(m_rootWatch->directories().first(),
                      QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files,
                      QDirIterator::Subdirectories);

    while (root.hasNext()) {
        root.next();
        QFileInfo info(root.filePath());
        if (info.isFile()) {
            m_files << info.filePath();
        } else if (info.isDir()) {
            m_rootWatch->addPath(info.filePath());
        }
    }
}
