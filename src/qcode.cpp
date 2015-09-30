#include "qcode.h"

#include "settings.h"
#include "mainwindow.h"
#include "dirsearchmodel.h"
#include "openedfilesmodel.h"

QCode::QCode(int &argc, char **argv)
    : QApplication(argc, argv)
{
    QCoreApplication::setOrganizationDomain("treat.org");
    QCoreApplication::setApplicationName("QCode");

    //Start the models immediately!
    m_sourceDirModel = new DirSearchModel(this);
    m_buildDirModel = new DirSearchModel(this);
    m_openedFilesModel = new OpenedFilesModel(this);

    m_mainWindow = new MainWindow;
    connect(this, SIGNAL(aboutToQuit()), m_mainWindow, SLOT(aboutToQuit()));
    m_mainWindow->show();

    m_settings = new Settings(m_mainWindow);
}

QCode::~QCode()
{
    if (m_mainWindow)
        delete m_mainWindow;

    delete m_sourceDirModel;
    delete m_buildDirModel;
    delete m_openedFilesModel;
}

MainWindow *QCode::mainWindow() const
{
    return m_mainWindow;
}

Settings *QCode::settings() const
{
    return m_settings;
}

DirSearchModel *QCode::sourceModel() const
{
    return m_sourceDirModel;
}

DirSearchModel *QCode::buildModel() const
{
    return m_buildDirModel;
}

OpenedFilesModel *QCode::openedFilesModel() const
{
    return m_openedFilesModel;
}
