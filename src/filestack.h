#ifndef FILESTACK_H
#define FILESTACK_H

#include <QStackedWidget>

class QSettings;

class FileView;

class FileStack : public QStackedWidget
{
    Q_OBJECT
public:
    FileStack(QWidget *parent = 0);
    virtual ~FileStack();

    void clear();

    FileView *currentFile() const;

    bool restoreState(QSettings *settings);
    void saveState(QSettings *settings);

public Q_SLOTS:
    void newFile();
    void cut(){}
    void copy(){}
    void paste(){}

//     void splitVertical();
//     void splitHorizontal();

//     void loadFile(const QString &fileName);
//     bool saveFile(const QString &fileName);
//     bool maybeSave();

Q_SIGNALS:
    void copyAvailable(bool);

private Q_SLOTS:
    void checkEmpty();
};

#endif
