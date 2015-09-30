#include "importproject.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>

ImportProject::ImportProject(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    buildSource->setSourceDirectory(QDir::homePath());
    buildSource->setBuildDirectory(tr("Same as source..."));
}

ImportProject::~ImportProject()
{
}

void ImportProject::accept()
{
    QDir dir(sourceDirectory());
    if (!dir.exists()) {
        QMessageBox::warning(this, tr("QCode"),
                             tr("Source directory %1: does not exist!\n")
                             .arg(sourceDirectory()));
        QDialog::reject();
    }

    m_fileName = dir.canonicalPath() + QDir::separator() + dir.dirName() + ".qcode";

    QFile file(m_fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << tr("## QCode project file for %1\n").arg(dir.dirName()) << endl;
        file.flush();
    }
    file.close();

    QDialog::accept();
}
