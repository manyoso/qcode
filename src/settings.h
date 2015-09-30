#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

#include "ui_settings.h"

class Project;
class ConfigPage;

template<class Key, class Value>
class QHash;

class Settings : public QDialog, public Ui::Settings
{
    Q_OBJECT

public:
    Settings(QWidget *parent = 0);
    virtual ~Settings();

public Q_SLOTS:
    virtual void accept();
    virtual void reject();

private Q_SLOTS:
    void selectPageForItem(QListWidgetItem *item);
    void buttonClicked(QAbstractButton *button);
    void modified(bool isModified);

Q_SIGNALS:
    void apply();
    void restoreDefaults();

private:
    void addConfigPage(ConfigPage *page);

private:
    QHash<QListWidgetItem*, ConfigPage*> m_itemHash;
};

#endif
