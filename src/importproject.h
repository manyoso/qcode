#ifndef IMPORTPROJECT_H
#define IMPORTPROJECT_H

#include <QDialog>

#include "ui_importproject.h"

class ImportProject : public QDialog, public Ui::ImportProject
{
    Q_OBJECT

public:
    ImportProject(QWidget *parent);
    virtual ~ImportProject();

    QString fileName() const { return m_fileName; }
    QString sourceDirectory() const { return buildSource->sourceDirectory(); }
    QString buildDirectory() const { return buildSource->buildDirectory(); }

public Q_SLOTS:
    virtual void accept();

private:
    QString m_fileName;
};

#endif
