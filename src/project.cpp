#include "project.h"

Project::Project(const QString &fileName, QObject *parent)
    : QSettings(fileName, QSettings::IniFormat, parent)
{
}

Project::~Project()
{
}
