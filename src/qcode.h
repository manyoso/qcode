#ifndef QCODE_H
#define QCODE_H

#include <QPointer>
#include <QApplication>

#define qCode \
    (static_cast<QCode*>(QCoreApplication::instance()))

class Settings;
class MainWindow;
class DirSearchModel;
class OpenedFilesModel;

class QCode : public QApplication
{
    Q_OBJECT

public:
    enum FileStatus {
        FileNormal,
        FileModified,
        FileExternallyModified
    };

    QCode(int &argc, char **argv);
    virtual ~QCode();

    MainWindow *mainWindow() const;

    Settings *settings() const;

    DirSearchModel *sourceModel() const;
    DirSearchModel *buildModel() const;
    OpenedFilesModel *openedFilesModel() const;

private:
    Settings *m_settings;
    QPointer<MainWindow> m_mainWindow;
    DirSearchModel *m_sourceDirModel;
    DirSearchModel *m_buildDirModel;
    OpenedFilesModel *m_openedFilesModel;
};

#endif
