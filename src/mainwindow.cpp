#include "mainwindow.h"

#include "qcode.h"
#include "project.h"
#include "newproject.h"
#include "importproject.h"
#include "settings.h"
#include "messages.h"
#include "quickselection.h"
#include "dirsearchmodel.h"
#include "openedfilesmodel.h"

#include "filestack.h"
#include "fileview.h"

#include <QtGui>

MainWindow::MainWindow()
{
    m_fileStack = new FileStack(this);
    m_project = 0;

#if 0
    m_messages = new QDockWidget(tr("Messages"), this);
    m_messages->setWidget(Messages::self());
    m_messages->hide();

    //Testing make
    QShortcut *s = new QShortcut(QKeySequence("F8"), this);
    QObject::connect(s, SIGNAL(activated()), Messages::self(), SLOT(testMake()));

    addDockWidget(Qt::BottomDockWidgetArea, m_messages);
#endif

    setCentralWidget(m_fileStack);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    setCurrentFile("");

    setWindowIcon(QIcon(":/images/qcode.png"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::aboutToQuit()
{
    maybeSave();
    writeSettings();
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        m_fileStack->newFile();
        setCurrentFile("");
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (m_fileStack->currentFile()->focusEdit()->path().isEmpty()) {
        return saveAs();
    } else {
        return saveFile(m_fileStack->currentFile()->focusEdit()->path());
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool MainWindow::close()
{
    //FIXME
    //Ask if modified...?
    //Need a different action for closing entire view?

    FileView *fileView = m_fileStack->currentFile()->focusView();
    Q_ASSERT(fileView);

    delete fileView;
    fileView = 0;

    return true;
}

void MainWindow::openSourceFile()
{
    QFrame *activeFrame =  m_fileStack->currentFile()->focusFrame();
    if (!activeFrame)
        return;

    QuickSelection dialog(qCode->sourceModel(), m_openSourceFile, activeFrame);
    if (dialog.exec())
        loadFiles(dialog.selectedIndexes());
}

void MainWindow::openBuildFile()
{
    QFrame *activeFrame =  m_fileStack->currentFile()->focusFrame();
    if (!activeFrame)
        return;

    QuickSelection dialog(qCode->buildModel(), m_openBuildFile, activeFrame);
    if (dialog.exec())
        loadFiles(dialog.selectedIndexes());
}

void MainWindow::selectOpenedFile()
{
    QuickSelection dialog(qCode->openedFilesModel(), m_selectOpenedFile, centralWidget());
    if (dialog.exec())
        loadFiles(dialog.selectedIndexes());
}

void MainWindow::splitVertical()
{
    if (FileView *view = m_fileStack->currentFile()->focusView())
        view->split(Qt::Vertical);
}

void MainWindow::splitHorizontal()
{
    if (FileView *view = m_fileStack->currentFile()->focusView())
        view->split(Qt::Horizontal);
}

void MainWindow::newProject()
{
    NewProject dialog(this);
    if (!dialog.exec())
        openProject(dialog.fileName(), dialog.sourceDirectory(), dialog.buildDirectory());
}

void MainWindow::openProject()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString(), QString(),
                                                 tr("Project File (*.qcode)"));
    if (!fileName.isEmpty())
        openProject(fileName);
}

bool MainWindow::importProject()
{
    ImportProject dialog(this);
    if (!dialog.exec())
        return false;

    return openProject(dialog.fileName(), dialog.sourceDirectory(), dialog.buildDirectory());
}

bool MainWindow::closeProject()
{
    if (!m_project)
        return false;

    QSettings settings;
    settings.setValue("lastProject", m_project->fileName());

    //Save the fileStack layout
    m_fileStack->saveState(m_project);

    delete m_project;
    m_project = 0;

    statusBar()->showMessage(tr("Project closed"), 2000);
    return true;
}

bool MainWindow::openSettings()
{
    Settings *dialog = qCode->settings();
    if (!dialog->exec())
        return false;

    statusBar()->showMessage(tr("Settings changed"), 2000);
    return true;
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About QCode"),
            tr("<b>QCode</b> is a simple yet powerful cross-platform source code editor."));
}

void MainWindow::documentWasModified()
{
//     setWindowModified(m_fileView->textEdit()->document()->isModified());
}

void MainWindow::createActions()
{
    m_newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    m_newAct->setShortcut(tr("Ctrl+N"));
    m_newAct->setStatusTip(tr("Create a new file"));
    connect(m_newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    m_openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    m_openAct->setShortcut(tr("Ctrl+O"));
    m_openAct->setStatusTip(tr("Open a file from disk"));
    connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

    m_saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    m_saveAct->setShortcut(tr("Ctrl+S"));
    m_saveAct->setStatusTip(tr("Save the document to disk"));
    connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

    m_saveAsAct = new QAction(tr("Save &As..."), this);
    m_saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    m_closeAct = new QAction(tr("&Close..."), this);
    m_closeAct->setShortcut(tr("Ctrl+W"));
    m_closeAct->setStatusTip(tr("Close the document"));
    connect(m_closeAct, SIGNAL(triggered()), this, SLOT(close()));

    m_openSourceFile = new QAction(tr("&Open Source File"), this);
    m_openSourceFile->setShortcut(tr("Ctrl+Alt+J"));
    m_openSourceFile->setStatusTip(tr("Open a file from the source directory"));
    connect(m_openSourceFile, SIGNAL(triggered()), this, SLOT(openSourceFile()));

    m_openBuildFile = new QAction(tr("&Open Build File"), this);
    m_openBuildFile->setShortcut(tr("Ctrl+Alt+K"));
    m_openBuildFile->setStatusTip(tr("Open a file from the build directory"));
    connect(m_openBuildFile, SIGNAL(triggered()), this, SLOT(openBuildFile()));

    m_selectOpenedFile = new QAction(tr("&Select Opened File"), this);
    m_selectOpenedFile->setShortcut(tr("Ctrl+Space"));
    m_selectOpenedFile->setStatusTip(tr("Select an already opened file"));
    connect(m_selectOpenedFile, SIGNAL(triggered()), this, SLOT(selectOpenedFile()));

    m_exitAct = new QAction(tr("E&xit"), this);
    m_exitAct->setShortcut(tr("Ctrl+Q"));
    m_exitAct->setStatusTip(tr("Exit the application"));
    connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

    m_cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    m_cutAct->setShortcut(tr("Ctrl+X"));
    m_cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(m_cutAct, SIGNAL(triggered()), m_fileStack, SLOT(cut()));

    m_copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    m_copyAct->setShortcut(tr("Ctrl+C"));
    m_copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(m_copyAct, SIGNAL(triggered()), m_fileStack, SLOT(copy()));

    m_pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    m_pasteAct->setShortcut(tr("Ctrl+V"));
    m_pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(m_pasteAct, SIGNAL(triggered()), m_fileStack, SLOT(paste()));

    m_splitVertical = new QAction(tr("Split &Vertical"), this);
    m_splitVertical->setShortcut(tr("Ctrl+Alt+V"));
    m_splitVertical->setStatusTip(tr("Split the focused file view vertically"));
    connect(m_splitVertical, SIGNAL(triggered()), this, SLOT(splitVertical()));

    m_splitHorizontal = new QAction(tr("Split &Horizontal"), this);
    m_splitHorizontal->setShortcut(tr("Ctrl+Alt+H"));
    m_splitHorizontal->setStatusTip(tr("Split the focused file view horizontally"));
    connect(m_splitHorizontal, SIGNAL(triggered()), this, SLOT(splitHorizontal()));

    m_newProjectAct = new QAction(QIcon(":/images/new.png"), tr("&New Project"), this);
    m_newProjectAct->setStatusTip(tr("Create a new project"));
    connect(m_newProjectAct, SIGNAL(triggered()), this, SLOT(newProject()));

    m_openProjectAct = new QAction(QIcon(":/images/open.png"), tr("&Open Project..."), this);
    m_openProjectAct->setStatusTip(tr("Open an existing project"));
    connect(m_openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()));

    m_importProjectAct = new QAction(tr("&Import Project"), this);
    m_importProjectAct->setStatusTip(tr("Import an existing project"));
    connect(m_importProjectAct, SIGNAL(triggered()), this, SLOT(importProject()));

    m_closeProjectAct = new QAction(tr("&Close Project..."), this);
    m_closeProjectAct->setStatusTip(tr("Close the project"));
    connect(m_closeProjectAct, SIGNAL(triggered()), this, SLOT(closeProject()));

    m_openSettingsAct = new QAction(tr("&Configure..."), this);
    m_openSettingsAct->setStatusTip(tr("Configure settings"));
    connect(m_openSettingsAct, SIGNAL(triggered()), this, SLOT(openSettings()));

    m_aboutAct = new QAction(tr("&About"), this);
    m_aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    m_aboutQtAct = new QAction(tr("About &Qt"), this);
    m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    m_cutAct->setEnabled(false);
    m_copyAct->setEnabled(false);
    connect(m_fileStack, SIGNAL(copyAvailable(bool)),
            m_cutAct, SLOT(setEnabled(bool)));
    connect(m_fileStack, SIGNAL(copyAvailable(bool)),
            m_copyAct, SLOT(setEnabled(bool)));
}

void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newAct);
    m_fileMenu->addAction(m_openAct);
    m_fileMenu->addAction(m_saveAct);
    m_fileMenu->addAction(m_saveAsAct);
    m_fileMenu->addAction(m_closeAct);
    m_fileMenu->addSeparator();

    m_fileMenu->addAction(m_openSourceFile);
    m_fileMenu->addAction(m_openBuildFile);
    m_fileMenu->addAction(m_selectOpenedFile);

    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAct);

    m_editMenu = menuBar()->addMenu(tr("&Edit"));
    m_editMenu->addAction(m_cutAct);
    m_editMenu->addAction(m_copyAct);
    m_editMenu->addAction(m_pasteAct);

    m_editMenu->addAction(m_splitVertical);
    m_editMenu->addAction(m_splitHorizontal);

    m_projectMenu = menuBar()->addMenu(tr("&Project"));
    m_projectMenu->addAction(m_newProjectAct);
    m_projectMenu->addAction(m_openProjectAct);
    m_projectMenu->addAction(m_importProjectAct);
    m_projectMenu->addAction(m_closeProjectAct);

    m_settingsMenu = menuBar()->addMenu(tr("&Settings"));
    m_settingsMenu->addAction(m_openSettingsAct);

    menuBar()->addSeparator();

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAct);
    m_helpMenu->addAction(m_aboutQtAct);
}

void MainWindow::createToolBars()
{
#if 0
    m_fileToolBar = addToolBar(tr("File"));
    m_fileToolBar->addAction(m_newAct);
    m_fileToolBar->addAction(m_openAct);
    m_fileToolBar->addAction(m_saveAct);

    m_editToolBar = addToolBar(tr("Edit"));
    m_editToolBar->addAction(m_cutAct);
    m_editToolBar->addAction(m_copyAct);
    m_editToolBar->addAction(m_pasteAct);
#endif
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings;
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);

    bool openLastProject = settings.value("openLastProject", true).toBool();
    if (openLastProject) {
		if (!settings.value("lastProject").toString().isEmpty())
			openProject(settings.value("lastProject").toString());
	}
}

void MainWindow::writeSettings()
{
    //Close active project if any...
    closeProject();

    QSettings settings;
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
//     if (m_fileView->textEdit()->document()->isModified()) {
//         QMessageBox::StandardButton ret;
//         ret = QMessageBox::warning(this, tr("QCode"),
//                      tr("The document has been modified.\n"
//                         "Do you want to save your changes?"),
//                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
//         if (ret == QMessageBox::Save)
//             return save();
//         else if (ret == QMessageBox::Cancel)
//             return false;
//     }
    return true;
}

void MainWindow::loadFiles(const QModelIndexList &list)
{
    foreach (QModelIndex index, list) {
        loadFile(index.data(Qt::ToolTipRole).toString()); //Store the path in the tooltip
    }
}

void MainWindow::loadFile(const QString &path)
{
    //Check to see if the file is already open
    if (qCode->openedFilesModel()->contains(path)) {
        //Switch to already opened file
        FileEdit *fileEdit = qobject_cast<FileEdit*>(qCode->openedFilesModel()->widget(path));
        Q_ASSERT(fileEdit);

        //Need to find the toplevel FileView for this FileEdit, tell FileStack to set that
        //view as current and then give this FileEdit focus..  HUZZAH!
        FileView *topLevel = fileEdit->topLevelView();
        Q_ASSERT(topLevel);

        m_fileStack->setCurrentWidget(topLevel);
        fileEdit->setFocus(Qt::OtherFocusReason);
    } else {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        //Find the currently focused edit widget
        FileEdit *fileEdit = m_fileStack->currentFile()->focusEdit();
        Q_ASSERT(fileEdit);

        //Current edit widget might already be a real file
        //If so create a new buffer for this file
        if (!fileEdit->isNewFile()) {
            m_fileStack->newFile();
            fileEdit = m_fileStack->currentFile()->focusEdit();
            Q_ASSERT(fileEdit);
        }

        fileEdit->loadFile(path);

        QApplication::restoreOverrideCursor();
        statusBar()->showMessage(tr("File loaded"), 2000);
    }

    setCurrentFile(path);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QCode"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << m_fileStack->currentFile()->focusEdit()->toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    m_curFile = fileName;
//     m_fileStack->currentFile()->focusEdit()->document()->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (m_curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(m_curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("QCode")));
}

bool MainWindow::openProject(const QString &fileName,
                             const QString &source, const QString &build)
{
    closeProject();

    m_project = new Project(fileName, this);
    QSettings::Status status = m_project->status();
    if (status != QSettings::NoError) {
        QString errorString;
        switch(status) {
        case QSettings::AccessError:
            errorString = tr("AccessError");
            break;
        case QSettings::FormatError:
            errorString = tr("FormatError");
            break;
        default:
            break;
        }
        QMessageBox::warning(this, tr("QCode"),
                             tr("Cannot open project %1\n:%2")
                             .arg(fileName).arg(errorString));

        delete m_project;
        m_project = 0;
        return false;
    }

    if (!source.isEmpty() || !build.isEmpty()) {
        m_project->setSourceDirectory(source);
        m_project->setBuildDirectory(build);
        m_project->sync();
    }

    qCode->sourceModel()->setRootPath(m_project->sourceDirectory());
    qCode->buildModel()->setRootPath(m_project->buildDirectory());

    m_fileStack->restoreState(m_project);

    emit activeProjectChanged();

    statusBar()->showMessage(tr("Project opened"), 2000);
    return true;
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
