#include "qcodewidgets.h"

QCodeWidgets::QCodeWidgets(QObject *parent)
    : QObject(parent) {
  m_plugins.append(new PathSelectorPlugin(this));
  m_plugins.append(new BuildSourcePlugin(this));
}

Q_EXPORT_PLUGIN2(qcodewidgets, QCodeWidgets)

