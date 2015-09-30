#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QFrame>
#include <QTextEdit>

class QSplitter;
class QSettings;

class FileWidget;
class FileView;
class Highlighter;

class FileEdit : public QTextEdit
{
    Q_OBJECT
public:
    FileEdit(FileWidget *parent, const QString &path);
    virtual ~FileEdit();

    bool loadFile(const QString &path);

    QString path() const;
    void setPath(const QString &path);

    bool isNewFile() const;

    FileWidget *parentFileWidget() const;
    FileView *topLevelView();

private:
    QString m_path;
    Highlighter *m_highlighter;
};

class FileLines : public QFrame
{
    Q_OBJECT
public:
    FileLines(FileWidget *parent);
    virtual ~FileLines();

    FileWidget *parentFileWidget() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
};

class FileWidget : public QFrame
{
    friend class FileView;
    Q_OBJECT
public:
    FileWidget(QWidget *parent, const QString &path);
    virtual ~FileWidget();

    FileLines *lines() const { return m_lines; }
    FileEdit *textEdit() const { return m_textEdit; }

    bool isPlaceHolder() const;
    void embed(FileView *frame);

    void saveState(QSettings *settings);

protected:
    FileWidget(QWidget *parent, FileLines *lines, FileEdit *textEdit);
    void commonConstructor();

    virtual bool eventFilter(QObject *watched, QEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
    FileLines *m_lines;
    FileEdit *m_textEdit;
};

class FileView : public QFrame
{
    Q_OBJECT
public:
    FileView(QWidget *parent, const QString &path = QString());
    virtual ~FileView();

    FileEdit *textEdit() const;

    bool isTopLevelView() const;
    bool isInnerView() const;

    FileView *topLevelView();
    FileView *outerView();
    FileView *parentView() const;

    FileEdit *focusEdit() const;
    FileWidget *focusFrame() const;
    FileView *focusView() const;

    FileView *split(Qt::Orientation orientation, const QString &path = QString());

    QString uniqueName() const { return m_uniqueName; }

    void saveState(QSettings *settings);

private Q_SLOTS:
    void update();

protected:
    FileView(FileWidget *fileWidget);
    void commonConstructor();
    void saveStateInternal(QSettings *settings);

private:
    uint m_index;
    QString m_uniqueName;
    FileWidget *m_fileWidget;
    QSplitter *m_splitter;
};

#endif
