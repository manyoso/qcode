#include "configpage.h"

ConfigPage::ConfigPage(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

ConfigPage::~ConfigPage()
{
}

void ConfigPage::apply()
{
    emit modified(false);
}

void ConfigPage::restoreDefaults()
{
    emit modified(false);
}

void ConfigPage::showEvent(QShowEvent *event)
{
    restoreDefaults();
    QWidget::showEvent(event);
}
