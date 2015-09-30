#include "projectconfig.h"

#include "qcode.h"
#include "settings.h"
#include "mainwindow.h"
#include "project.h"

#include <QSettings>

#include "ui_projectconfig.h"

ProjectConfig::ProjectConfig(QWidget *parent)
    : ConfigPage(parent)
{
    setWindowTitle(tr("Project Configuration"));
    label->setText(windowTitle());

    m_pod = new Ui::ProjectConfig;
    m_pod->setupUi(frame);
}

ProjectConfig::~ProjectConfig()
{
    delete m_pod;
}

void ProjectConfig::apply()
{
    if (Project *project = qCode->mainWindow()->project()) {
        project->setSourceDirectory(m_pod->buildSource->sourceDirectory());
        project->setBuildDirectory(m_pod->buildSource->buildDirectory());
    }

    QSettings settings;
    settings.setValue("projectDirectory", m_pod->projectDirectory->path());
    settings.setValue("sourceDirectory", m_pod->sourceDirectory->path());
    settings.setValue("buildDirectory", m_pod->buildDirectory->path());
    settings.setValue("openLastProject", m_pod->lastProject->isChecked());

    ConfigPage::apply();
}

void ProjectConfig::restoreDefaults()
{
    if (Project *project = qCode->mainWindow()->project()) {
        m_pod->buildSource->setEnabled(true);
        m_pod->buildSource->setSourceDirectory(project->sourceDirectory());
        m_pod->buildSource->setBuildDirectory(project->buildDirectory());
    } else {
        m_pod->buildSource->setEnabled(false);
    }

    QSettings settings;
    m_pod->projectDirectory->setPath(settings.value("projectDirectory", QDir::homePath()).toString());
    m_pod->sourceDirectory->setPath(settings.value("sourceDirectory", QDir::homePath()).toString());
    m_pod->buildDirectory->setPath(settings.value("buildDirectory", tr("Same as source...")).toString());
    m_pod->lastProject->setChecked(settings.value("openLastProject", true).toBool());

    ConfigPage::restoreDefaults();
}
