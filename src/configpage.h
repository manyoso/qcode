#ifndef CONFIGPAGE_H
#define CONFIGPAGE_H

#include <QWidget>

#include "ui_configpage.h"

class ConfigPage : public QWidget, public Ui::ConfigPage
{
    Q_OBJECT

public:
    ConfigPage(QWidget *parent);
    virtual ~ConfigPage();

protected Q_SLOTS:
    virtual void apply();
    virtual void restoreDefaults();

protected:
    virtual void showEvent(QShowEvent *event);

Q_SIGNALS:
    void modified(bool isModified);
};

#endif
