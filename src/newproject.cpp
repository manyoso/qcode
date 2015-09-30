#include "newproject.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

NewProject::NewProject(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    buildSource->setSourceDirectory(QDir::homePath());
    buildSource->setBuildDirectory(tr("Same as source..."));
}

NewProject::~NewProject()
{
}

void NewProject::accept()
{
    QDir dir(sourceDirectory());
    if (!dir.exists())
        dir.mkpath(sourceDirectory());

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
