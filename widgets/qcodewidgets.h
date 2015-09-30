#ifndef QCODEWIDGETS_H
#define QCODEWIDGETS_H

#include <QObject>
#include <QDesignerCustomWidgetInterface>

#include <QtPlugin>

class QCodeWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface {
public:
  QCodeWidgetPlugin(QObject *parent = 0) : QObject(parent), _initialized(false) {}

  QString group() const {
    return tr("QCode Widgets");
  }
  QString toolTip() const {
    return QString::null;
  }
  QString whatsThis() const {
    return QString::null;
  }

  QString instanceName() const {
    QChar camel = name().at(0).toLower();
    return name().replace(0,1,camel.toLower());
  }

  QString includeFile() const {
    return name().toLower() + ".h";
  }

  QString domXml() const {
    return QString::fromUtf8("<widget class=\"%1\" name=\"%2\"/>")
           .arg(name()).arg(instanceName().toLower());
  }

  bool isContainer() const {
    return false;
  }
  bool isInitialized() const {
    return _initialized;
  }
  QIcon icon() const {
    return QIcon();
  }

  void initialize(QDesignerFormEditorInterface *) {
    if (_initialized)
      return;

    _initialized = true;
  }

private:
  bool _initialized;
};

#include "pathselector.h"
class PathSelectorPlugin : public QCodeWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  PathSelectorPlugin(QObject *parent = 0) : QCodeWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("PathSelector");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new PathSelector(parent);
  }
};

#include "buildsource.h"
class BuildSourcePlugin : public QCodeWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  BuildSourcePlugin(QObject *parent = 0) : QCodeWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("BuildSource");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new BuildSource(parent);
  }
};

class QCodeWidgets : public QObject, public QDesignerCustomWidgetCollectionInterface {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
public:
  QCodeWidgets(QObject *parent = 0);
  virtual ~QCodeWidgets() {}
  QList<QDesignerCustomWidgetInterface*> customWidgets() const {
    return m_plugins;
  }

private:
  QList<QDesignerCustomWidgetInterface*> m_plugins;
};

#endif
