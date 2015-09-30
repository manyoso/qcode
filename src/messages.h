#ifndef MESSAGES_H
#define MESSAGES_H

#include <QTextEdit>

#include <QProcess>

class Messages : public QTextEdit
{
    Q_OBJECT

public:
    Messages();
    virtual ~Messages();

public Q_SLOTS:
    void testMake();

private Q_SLOTS:
    void finished(int exitCode, QProcess::ExitStatus status);
    void readStandardError();
    void readStandardOutput();

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    void processLine(QString *line);
};

#endif
