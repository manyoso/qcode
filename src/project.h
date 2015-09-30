#ifndef PROJECT_H
#define PROJECT_H

#include <QSettings>

#include <QDir>

class Project : public QSettings
{
    Q_OBJECT

public:
    Project(const QString &filename, QObject *parent);
    virtual ~Project();

    QString projectDirectory() const { return fileName(); }

    QString sourceDirectory() const
    { return value("sourceDirectory").toString(); }

    void setSourceDirectory(const QDir &dir)
    {
        setValue("sourceDirectory", dir.canonicalPath());
        emit sourceDirectoryChanged();
    }

    QString buildDirectory() const
    { return value("buildDirectory").toString(); }

    void setBuildDirectory(const QDir &dir)
    {
        setValue("buildDirectory", dir.canonicalPath());
        emit buildDirectoryChanged();
    }

Q_SIGNALS:
    void sourceDirectoryChanged();
    void buildDirectoryChanged();
};

#endif
