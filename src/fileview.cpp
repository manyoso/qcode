#include "fileview.h"

#include "qcode.h"
#include "filestack.h"
#include "openedfilesmodel.h"
#include "highlighter.h"

#include "mainwindow.h"

#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QStyle>
#include <QPainter>
#include <QStyleOptionFrame>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QScrollBar>
#include <QTextDocument>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QSettings>

#include <QDebug>

FileEdit::FileEdit(FileWidget *parent, const QString &path)
    : QTextEdit(parent)
{
    m_highlighter = new Highlighter(document());

    setPath(path);
    setObjectName("FileEdit");
    setFrameStyle(QFrame::NoFrame);

    //TODO Make much of this configurable and respect vim/emacs/kate modelines
    setTabStopWidth(fontMetrics().width(' ') * 4);
}

FileEdit::~FileEdit()
{
    qCode->openedFilesModel()->removeFile(m_path);
}

bool FileEdit::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QCode"),
                            tr("Cannot read file %1:\n%2.")
                            .arg(path)
                            .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    setPath(path);
    setPlainText(in.readAll());
    return true;
}

QString FileEdit::path() const
{
    return m_path;
}

void FileEdit::setPath(const QString &path)
{
    QStringList cpp;
    cpp << "cpp" << "c++" << "cp" << "c" << "h";
    bool isCpp = cpp.contains(QFileInfo(path).suffix());
    m_highlighter->setEnabled(isCpp);

    //Remove previous path from the model
    if (!m_path.isEmpty())
        qCode->openedFilesModel()->removeFile(m_path);

    m_path = path;

    //No empty paths allowed
    //FIXME how about using parent view's unique name
    if (m_path.isEmpty())
        m_path = QTime::currentTime().toString();

    //Add the new path to the model
    qCode->openedFilesModel()->addFile(m_path);
    qCode->openedFilesModel()->setWidget(m_path, this);
}

bool FileEdit::isNewFile() const
{
    QFileInfo info(m_path);
    return !info.exists();
}

FileWidget *FileEdit::parentFileWidget() const
{
    return qobject_cast<FileWidget*>(parentWidget());
}

FileView *FileEdit::topLevelView()
{
    QWidget *widget = this;
    while (widget) {
        FileView *fileView = qobject_cast<FileView*>(widget);
        if (fileView && fileView->isTopLevelView())
            return fileView;
        widget = widget->parentWidget();
    }
    return 0;
}

FileLines::FileLines(FileWidget *parent)
    : QFrame(parent)
{
    //Make room for 4 digits
    setFixedWidth(fontMetrics().width( QString("0000")));
    setObjectName("FileLines");
    setFrameStyle(QFrame::NoFrame);
}

FileLines::~FileLines()
{
}

FileWidget *FileLines::parentFileWidget() const
{
    return qobject_cast<FileWidget*>(parentWidget());
}

void FileLines::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QAbstractTextDocumentLayout *layout = parentFileWidget()->textEdit()->document()->documentLayout();
    int contentsY = parentFileWidget()->textEdit()->verticalScrollBar()->value();
    qreal pageBottom = contentsY + parentFileWidget()->textEdit()->viewport()->height();
    const QFontMetrics fm = fontMetrics();
    const int ascent = fontMetrics().ascent() + 1; // height = ascent + descent + 1
    int lineCount = 1;

    QPainter p(this);

    for (QTextBlock block = parentFileWidget()->textEdit()->document()->begin();
         block.isValid(); block = block.next(), ++lineCount) {

        const QRectF boundingRect = layout->blockBoundingRect(block);

        QPointF position = boundingRect.topLeft();
        if (position.y() + boundingRect.height() < contentsY)
            continue;
        if (position.y() > pageBottom)
            break;

        const QString txt = QString::number(lineCount);
        p.drawText(width() - fm.width(txt), qRound(position.y()) - contentsY + ascent, txt);
    }
}

FileWidget::FileWidget(QWidget *parent, const QString &path)
    : QFrame(parent)
{
    m_lines = new FileLines(this);
    m_textEdit = new FileEdit(this, path);

    connect(m_textEdit->document()->documentLayout(), SIGNAL(update(const QRectF &)),
            m_lines, SLOT( update()));
    connect(m_textEdit->verticalScrollBar(), SIGNAL(valueChanged(int)),
            m_lines, SLOT( update()));

    commonConstructor();
}

FileWidget::FileWidget(QWidget *parent, FileLines *lines, FileEdit *textEdit)
    : QFrame(parent)
{
    m_lines = lines;
    m_textEdit = textEdit;

    commonConstructor();
}

void FileWidget::commonConstructor()
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Plain);

    QHBoxLayout *horizontal = new QHBoxLayout(this);
    horizontal->setMargin(0);
    horizontal->setSpacing(0);
    m_lines->setParent(this);
    m_textEdit->setParent(this);
    horizontal->addWidget(m_lines);
    horizontal->addWidget(m_textEdit);
    setLayout(horizontal);

    //textEdit handles our focus
    setFocusProxy(m_textEdit);

    m_textEdit->installEventFilter(this);
}

FileWidget::~FileWidget()
{
}

bool FileWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);
    if (event->type() == QEvent::FocusIn ||
        event->type() == QEvent::FocusOut) {
        update();
    }
    return false;
}

void FileWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (frameStyle() == QFrame::NoFrame)
        return;

    QPainter p(this);
    QStyleOptionFrame option;
    option.initFrom(this);
    option.rect = rect();
    option.palette = palette();

    option.state = QStyle::State_None;

    if (m_textEdit->hasFocus())
        option.state = QStyle::State_Raised | QStyle::State_HasFocus;

    option.lineWidth = 1;
    option.midLineWidth = 1;

    style()->drawPrimitive(QStyle::PE_Frame, &option, &p, this);
}

bool FileWidget::isPlaceHolder() const
{
    return m_textEdit->parent() != this;
}

void FileWidget::embed(FileView *frame)
{
    setFrameStyle(QFrame::NoFrame);
    layout()->addWidget(frame);
}

void FileWidget::saveState(QSettings *settings)
{
    if (isPlaceHolder()) {
        for (int i = 0; i < layout()->count(); ++i) {
            QLayoutItem *item = layout()->itemAt(i);
            Q_ASSERT(item);
            FileView *fileView = qobject_cast<FileView*>(item->widget());
            Q_ASSERT(fileView);
            fileView->saveState(settings);
        }
    }
}

FileView::FileView(QWidget *parent, const QString &path)
    : QFrame(parent)
{
    m_fileWidget = new FileWidget(this, path);

    commonConstructor();
}

FileView::FileView(FileWidget *fileWidget)
    : QFrame(fileWidget)
{
    fileWidget->textEdit()->removeEventFilter(fileWidget);
    m_fileWidget = new FileWidget(this, fileWidget->lines(), fileWidget->textEdit());

    commonConstructor();
}

void FileView::commonConstructor()
{
    setFrameStyle(QFrame::NoFrame);

    m_splitter = new QSplitter(this);
    m_splitter->setChildrenCollapsible(false);
    m_splitter->addWidget(m_fileWidget); //splitter becomes parent

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_splitter);
    setLayout(layout);

    //fileWidget handles our focus
    setFocusProxy(m_fileWidget);

    m_index = 0;

    //Set a unique name based on creation order
    if (!isInnerView())
        m_uniqueName = QString::number(topLevelView()->m_index++);
}

FileView::~FileView()
{
}

FileEdit *FileView::textEdit() const
{
    return m_fileWidget->textEdit();
}

bool FileView::isTopLevelView() const
{
    return (qobject_cast<FileStack*>(parentWidget()) != 0);
}

bool FileView::isInnerView() const
{
    return (!isTopLevelView() && qobject_cast<FileWidget*>(parentWidget()) != 0);
}

FileView *FileView::topLevelView()
{
    QWidget *widget = this;
    while (widget) {
        FileView *fileView = qobject_cast<FileView*>(widget);
        if (fileView && fileView->isTopLevelView())
            return fileView;
        widget = widget->parentWidget();
    }
    Q_ASSERT(false); //Should never reach
    return 0;
}

FileView *FileView::outerView()
{
    QWidget *widget = this;
    while (widget) {
        FileView *fileView = qobject_cast<FileView*>(widget);
        if (fileView && !fileView->isInnerView())
            return fileView;
        widget = widget->parentWidget();
    }
    Q_ASSERT(false); //Should never reach
    return 0;
}

FileView *FileView::parentView() const
{
    if (QSplitter *parent = qobject_cast<QSplitter*>(parentWidget())) {
        //splitter
        return qobject_cast<FileView*>(parent->parentWidget());
    } else if (FileWidget *parent = qobject_cast<FileWidget*>(parentWidget())) {
        //fileWidget->splitter
        return qobject_cast<FileView*>(parent->parentWidget()->parentWidget());
    } else {
        Q_ASSERT(isTopLevelView());
        return 0;
    }
}

FileEdit *FileView::focusEdit() const
{
    //textEdit
    return qobject_cast<FileEdit*>(focusWidget());
}

FileWidget *FileView::focusFrame() const
{
    //textEdit->fileWidget
    return qobject_cast<FileWidget*>(focusWidget()->parentWidget());
}

FileView *FileView::focusView() const
{
    //fileWidget->splitter->fileView
    return qobject_cast<FileView*>(focusFrame()->parentWidget()->parentWidget());
}

FileView *FileView::split(Qt::Orientation orientation, const QString &path)
{
    FileView *view = 0;

//     if (isTopLevelView()) {
//         setUpdatesEnabled(false); //reduces flicker
//     }

    if (m_splitter->count() < 2) {
        m_splitter->setOrientation(orientation);
        view = new FileView(this, path);
        m_splitter->addWidget(view);

        //Reset...
        QList<int> sizes;
        sizes << 0 << 0;
        m_splitter->setSizes(sizes);

        view->textEdit()->setFocus(Qt::OtherFocusReason);
    } else {
        view = new FileView(m_fileWidget);
        m_fileWidget->embed(view);
        view = view->split(orientation, path);
    }

//     if (isTopLevelView()) {
//         QTimer::singleShot(0, this, SLOT(update()));
//     }
    return view;
}

void FileView::update()
{
    setUpdatesEnabled(true);
}

void FileView::saveState(QSettings *settings)
{
    //Don't save reference to an inner view
    if (isInnerView()) {
        saveStateInternal(settings);
        return;
    }

    settings->beginGroup("FileView_" + uniqueName());
    settings->setValue("File", textEdit()->path());

    if (!isTopLevelView()) {

        if (parentView()->m_splitter->orientation() == Qt::Horizontal)
            settings->setValue("Orientation", "Horizontal");
        else
            settings->setValue("Orientation", "Vertical");

        if (parentView()->isInnerView())
            settings->setValue("Parent", parentView()->outerView()->uniqueName());
        else
            settings->setValue("Parent", parentView()->uniqueName());

    } else {
        settings->setValue("Parent", "None");
    }

    settings->endGroup();

    saveStateInternal(settings);
}

void FileView::saveStateInternal(QSettings *settings)
{
    //Go through the split one at a time
    for (int i = 0; i < m_splitter->count(); ++i) {
        QWidget *widget = m_splitter->widget(i);
        if (FileWidget *fileWidget = qobject_cast<FileWidget*>(widget)) {
            fileWidget->saveState(settings);
        } else if (FileView *fileView = qobject_cast<FileView*>(widget)) {
            fileView->saveState(settings);
        }
    }
}
