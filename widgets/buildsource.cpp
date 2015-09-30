#include "buildsource.h"

#include "ui_buildsource.h"

BuildSource::BuildSource(QWidget *parent)
    : QWidget(parent)
{
    m_pod = new Ui::BuildSource;
    m_pod->setupUi(this);
}

BuildSource::~BuildSource()
{
    delete m_pod;
}

QString BuildSource::sourceDirectory() const { return m_pod->sourceEdit->path(); }
void BuildSource::setSourceDirectory(const QString &path) { m_pod->sourceEdit->setPath(path); }

QString BuildSource::buildDirectory() const { return m_pod->buildEdit->path(); }
void BuildSource::setBuildDirectory(const QString &path) { return m_pod->buildEdit->setPath(path); }
