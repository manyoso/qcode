#include "settings.h"

#include "qcode.h"
#include "project.h"
#include "configpage.h"
#include "projectconfig.h"

#include <QHash>
#include <QPushButton>

Settings::Settings(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    connect(listWidget, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(selectPageForItem(QListWidgetItem *)));

    connect(buttonBox, SIGNAL(clicked(QAbstractButton *)),
            this, SLOT(buttonClicked(QAbstractButton *)));

    //create the pages
    addConfigPage(new ProjectConfig(this));

    //select the first page
    selectPageForItem(listWidget->item(0));

    setMinimumSize(QSize(640, 480));
}

Settings::~Settings()
{
}

void Settings::accept()
{
    modified(false);
    QDialog::accept();
}

void Settings::reject()
{
    modified(false);
    QDialog::reject();
}

void Settings::addConfigPage(ConfigPage *page)
{
    connect(page, SIGNAL(modified(bool)), this, SLOT(modified(bool)));
    connect(this, SIGNAL(apply()), page, SLOT(apply()));
    connect(this, SIGNAL(restoreDefaults()), page, SLOT(restoreDefaults()));
    QListWidgetItem *item = new QListWidgetItem(page->windowTitle(), listWidget);
    listWidget->addItem(item);
    stackedWidget->addWidget(page);
    m_itemHash.insert(item, page);
}

void Settings::selectPageForItem(QListWidgetItem *item)
{
    if (m_itemHash.contains(item))
        stackedWidget->setCurrentWidget(m_itemHash.value(item));
}

void Settings::buttonClicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton std = buttonBox->standardButton(button);
    switch(std) {
    case QDialogButtonBox::Apply:
        emit apply();
        break;
    case QDialogButtonBox::RestoreDefaults:
        emit restoreDefaults();
        break;
    default:
        break;
    }
}

void Settings::modified(bool isModified)
{
    Q_UNUSED(isModified)
//FIXME
//     buttonBox->button(QDialogButtonBox::Apply)->setEnabled(isModified);
//     buttonBox->button(QDialogButtonBox::RestoreDefaults)->setEnabled(isModified);
}

