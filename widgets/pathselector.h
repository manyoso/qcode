#ifndef PATHSELECTOR_H
#define PATHSELECTOR_H

#include <QDialog>

namespace Ui { class PathSelector; }

class PathSelector : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( bool directoryMode READ directoryMode WRITE setDirectoryMode )
public:
    PathSelector(QWidget *parent);
    virtual ~PathSelector();

    QString path() const;
    void setPath(const QString &path);

    bool directoryMode() const { return m_directoryMode; }
    void setDirectoryMode(bool directoryMode) { m_directoryMode = directoryMode; }

private Q_SLOTS:
    void getPath();

private:
    bool m_directoryMode;
    Ui::PathSelector *m_pod;
};

#endif
