#ifndef PROJECTCONFIG_H
#define PROJECTCONFIG_H

#include "configpage.h"

namespace Ui { class ProjectConfig; }

class ProjectConfig : public ConfigPage
{
    Q_OBJECT

public:
    ProjectConfig(QWidget *parent);
    virtual ~ProjectConfig();

protected Q_SLOTS:
    virtual void apply();
    virtual void restoreDefaults();

private:
    Ui::ProjectConfig *m_pod;
};

#endif
