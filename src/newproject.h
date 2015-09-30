#ifndef NEWPROJECT_H
#define NEWPROJECT_H

#include <QDialog>

#include "ui_newproject.h"

class NewProject : public QDialog, public Ui::NewProject
{
    Q_OBJECT

public:
    NewProject(QWidget *parent);
    virtual ~NewProject();

    QString fileName() const { return m_fileName; }
    QString sourceDirectory() const { return buildSource->sourceDirectory(); }
    QString buildDirectory() const { return buildSource->buildDirectory(); }

public Q_SLOTS:
    virtual void accept();

private:
    QString m_fileName;
};

#endif
