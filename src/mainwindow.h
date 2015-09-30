#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

class Project;
class FileStack;
class FaderWidget;
class DirSearchModel;

class QAction;
class QMenu;
class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();

    Project *project() const { return m_project; }

Q_SIGNALS:
    void activeProjectChanged();

private Q_SLOTS:
    void aboutToQuit();
    void newFile();
    void open();
    bool save();
    bool saveAs();
    bool close();

    void openSourceFile();
    void openBuildFile();
    void selectOpenedFile();

    void splitVertical();
    void splitHorizontal();

    void newProject();
    void openProject();
    bool importProject();
    bool closeProject();

    bool openSettings();

    void about();
    void documentWasModified();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    void readSettings();
    void writeSettings();

    bool maybeSave();

    void loadFiles(const QModelIndexList &list);
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);

    bool openProject(const QString &fileName,
                     const QString &source = QString(),
                     const QString &build = QString());

    QString strippedName(const QString &fullFileName);

private:
    FileStack *m_fileStack;
    Project *m_project;

    //QDockWidget *m_messages;

    QString m_curFile;

    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_projectMenu;
    QMenu *m_settingsMenu;
    QMenu *m_helpMenu;

    QToolBar *m_fileToolBar;
    QToolBar *m_editToolBar;

    QAction *m_newAct;
    QAction *m_openAct;
    QAction *m_saveAct;
    QAction *m_saveAsAct;
    QAction *m_closeAct;

    QAction *m_openSourceFile;
    QAction *m_openBuildFile;
    QAction *m_selectOpenedFile;

    QAction *m_exitAct;
    QAction *m_cutAct;
    QAction *m_copyAct;
    QAction *m_pasteAct;

    QAction *m_splitVertical;
    QAction *m_splitHorizontal;

    QAction *m_newProjectAct;
    QAction *m_openProjectAct;
    QAction *m_importProjectAct;
    QAction *m_closeProjectAct;

    QAction *m_openSettingsAct;

    QAction *m_aboutAct;
    QAction *m_aboutQtAct;
};

#endif
