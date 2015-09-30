#ifndef BUILDSOURCE_H
#define BUILDSOURCE_H

#include <QWidget>

namespace Ui { class BuildSource; }

class BuildSource : public QWidget
{
    Q_OBJECT

public:
    BuildSource(QWidget *parent);
    virtual ~BuildSource();

    QString sourceDirectory() const;
    void setSourceDirectory(const QString &path);
    QString buildDirectory() const;
    void setBuildDirectory(const QString &path);

private:
    Ui::BuildSource *m_pod;
};

#endif
