#include "filestack.h"

#include "qcode.h"
#include "fileview.h"
#include "openedfilesmodel.h"

#include <QTime>
#include <QDebug>
#include <QSettings>

#include <QFileInfo>

FileStack::FileStack(QWidget *parent)
    : QStackedWidget(parent)
{
    newFile();
    connect(this, SIGNAL(widgetRemoved(int)), this, SLOT(checkEmpty()));
}

FileStack::~FileStack()
{
}

void FileStack::clear()
{
    for (int i = 0; i < count(); ++i) {
        FileView *fileView = qobject_cast<FileView*>(widget(i));
        Q_ASSERT(fileView);
        removeWidget(fileView);
        delete fileView;
        fileView = 0;
    }
}

FileView *FileStack::currentFile() const
{
    return qobject_cast<FileView*>(currentWidget());
}

bool FileStack::restoreState(QSettings *settings)
{
//     setUpdatesEnabled(false);

    blockSignals(true);
    clear(); // don't worry about checking for empty
    blockSignals(false);

    QHash<QString, FileView*> views;

    int size = settings->beginReadArray("FileStack");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);

        foreach (QString group, settings->childGroups()) {

            QString path = settings->value(group + "/File").toString();
            QString parent = settings->value(group + "/Parent").toString();
            QString orientation = settings->value(group + "/Orientation").toString();

            QString key = group.remove("FileView_");

            if (orientation.isEmpty()) {

                newFile();

            } else if (views.contains(parent)) {

                FileView *p = views.value(parent);
                p->setFocus(Qt::OtherFocusReason);
                Qt::Orientation o = orientation == "Horizontal" ? Qt::Horizontal : Qt::Vertical;
                p->focusView()->split(o);
            }

            views.insert(key, currentFile());
            QFileInfo info(path);
            if (info.exists())
                currentFile()->focusEdit()->loadFile(path);
        }
    }
    settings->endArray();

//     setUpdatesEnabled(true);

    return true;
}

void FileStack::saveState(QSettings *settings)
{
    //Go through the views one at a time
    settings->remove("FileStack");
    settings->beginWriteArray("FileStack");

    for (int i = 0; i < count(); ++i) {
        settings->setArrayIndex(i);

        FileView *fileView = qobject_cast<FileView*>(widget(i));
        Q_ASSERT(fileView);
        fileView->saveState(settings);
    }

    settings->endArray();
}

void FileStack::newFile()
{
    int index = addWidget(new FileView(this));
    setCurrentIndex(index);

    //Grab focus
    currentWidget()->setFocus(Qt::OtherFocusReason);
}

void FileStack::checkEmpty()
{
    //Always leave one file in the stack
    if (!count())
        newFile();
}
