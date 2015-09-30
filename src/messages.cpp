#include "messages.h"

#include "qcode.h"
#include "settings.h"
#include "project.h"
#include "makesyntax.h"
#include "mainwindow.h"

#include <QApplication>

#include <QDebug>
#include <QMouseEvent>
#include <QProcess>

Messages::Messages()
    : QTextEdit()
{
    setReadOnly(true);
    new MakeSyntax(this->document());
}

Messages::~Messages()
{
}

void Messages::testMake()
{
    Project *project = qCode->mainWindow()->project();
    if (!project)
        return;

    //Testing
    QProcess *process = new QProcess(this);
    process->setEnvironment(QProcess::systemEnvironment());
    process->setWorkingDirectory(project->buildDirectory());

    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readStandardError()));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(finished(int, QProcess::ExitStatus)));

    clear();

    QString command = "make";
    append(command);

    process->start(command);

    //FIXME
    parentWidget()->show();
}

void Messages::finished(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(status)
    append("Finished");
    QProcess *process = qobject_cast<QProcess*>(sender());
    delete process;
}

void Messages::readStandardError()
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    QString s = process->readAllStandardError().trimmed();

//     processLine(&s);
// 
//     if (!s.isEmpty())
        append(s);
}

void Messages::readStandardOutput()
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    QString s = process->readAllStandardOutput().trimmed();

//     processLine(&s);
// 
//     if (!s.isEmpty())
        append(s);
}

void Messages::processLine(QString *line)
{
    //Grep the line for file and location info
    QRegExp expression("(\\w+).cpp:(\\d+)|(\\w+).h:(\\d+)");
    if (!line->contains(expression)) {
        *line = QString::null;
    }
}

void Messages::mousePressEvent(QMouseEvent *event)
{
    QTextEdit::mousePressEvent(event);

//     QTextCursor tc = cursorForPosition(event->pos());
//     tc.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
//     tc.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
//     QString selectedLine = tc.selectedText();
//     QRegExp expression("(\\w+).cpp:(\\d+)|(\\w+).h:(\\d+)");
//     if (selectedLine.contains(expression)) {
//         qDebug() << expression.capturedTexts().first() << endl;
//     }
}
