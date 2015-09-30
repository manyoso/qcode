#include "quickselection.h"

#include "faderwidget.h"

#include <QDebug>
#include <QAction>
#include <QPointer>
#include <QShortcut>
#include <QKeyEvent>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QApplication>
#include <QSortFilterProxyModel>

ListView::ListView(QAbstractItemModel *model, QWidget *parent)
    : QListView(parent)
{
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(model);

//     m_proxyModel->setFilterCaseSensitivity(Qt::CaseSensitivity cs)
//     m_proxyModel->setFilterKeyColumn(int column)

    setAlternatingRowColors(true);

    setModel(m_proxyModel);
}

ListView::~ListView()
{
}

void ListView::setFilter(const QString &filter)
{
    m_proxyModel->setFilterRegExp(QRegExp("^"+filter+"\\w*"));
}

QuickSelection::QuickSelection(QAbstractItemModel *model, QAction *action, QWidget *parent)
    : QFrame(parent), m_result(false), m_action(action), m_eventLoop(0)
{
    setFrameStyle(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setAutoFillBackground(true);

    //So we can move when the frame resizes
    parent->installEventFilter(this);

    //Fader widget is sibling class...
    m_faderWidget = new FaderWidget(parent);

    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setFrame(false);
    m_lineEdit->setAutoFillBackground(true);
    m_lineEdit->installEventFilter(this);
    setFocusProxy(m_lineEdit);

    m_listView = new ListView(model, this);
    m_listView->setFrameStyle(QFrame::NoFrame);
    m_listView->setFocusProxy(m_lineEdit);
    m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(m_lineEdit, SIGNAL(textChanged(const QString &)),
            m_listView, SLOT(setFilter(const QString &)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_listView);
    setLayout(layout);
}

QuickSelection::~QuickSelection()
{
    delete m_faderWidget;
}

QModelIndexList QuickSelection::selectedIndexes() const
{
    return m_listView->selectedIndexes();
}

bool QuickSelection::exec()
{
    m_result = false;

    //Record who had focus before we show
    QWidget *focus = qApp->focusWidget();

    show();

    //Disable the action that got us here...
    m_action->setEnabled(false);

    //Steal the action's shortcut to cancel this selection
    QShortcut *s = new QShortcut(m_action->shortcut(), this);
    connect(s, SIGNAL(activated()), this, SLOT(reject()));

    //Start our own event loop
    QEventLoop eventLoop;
    m_eventLoop = &eventLoop;
    QPointer<QuickSelection> guard = this;
    (void) eventLoop.exec();
    if (guard.isNull())
        return false;
    m_eventLoop = 0;

    //Re-enable our dear shortcut
    m_action->setEnabled(true);

    //Give focus back to the previous widget
    if (focus)
        focus->setFocus(Qt::OtherFocusReason);

    return m_result;
}

QSize QuickSelection::sizeHint() const
{
    //Fourty percent of the parent's size sounds right to me
    return QSize(int(parentWidget()->width() * 0.4), int(parentWidget()->height() * 0.4));
}

void QuickSelection::setVisible(bool visible)
{
    if (visible == isVisible())
        return;

    //Move the selection to the center of the parent widget
    if (QWidget *parent = parentWidget()) {
        if (visible) {
            resize(sizeHint());
            move( (parent->width() / 2) - width() / 2,
                (parent->height() / 2) - height() / 2 );
        }
    }

    parentWidget()->setUpdatesEnabled(false);

    //Hide fader first
    if (!visible)
        m_faderWidget->setVisible(visible);

    QWidget::setVisible(visible);

    //Show fader after
    if (visible)
        m_faderWidget->setVisible(visible);

    //Bring dialog to top
    raise();

    parentWidget()->setUpdatesEnabled(true);

    if (visible) {
        setFocus(Qt::OtherFocusReason);
    }

    if (!visible) {
        if (m_eventLoop)
            m_eventLoop->exit();
    }
}

bool QuickSelection::eventFilter(QObject *watched, QEvent *event)
{
    if (!watched || !event)
        return true;

    if (watched == m_lineEdit && event->type() == QEvent::KeyPress) {

        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        switch (ke->key())
        {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            //Deliver navigation key events to the listView
            QApplication::sendEvent(m_listView, event);
            m_lineEdit->blockSignals(true);
            m_lineEdit->setText(m_listView->currentIndex().data().toString());
            m_lineEdit->blockSignals(false);
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            accept();
            return true;
        case Qt::Key_Escape:
        case Qt::Key_Cancel:
            reject();
            return true;
        default:
            break;
        }

    } else if (watched == parentWidget() && event->type() == QEvent::Resize) {

        //Make sure to resize selection when parent is resized
        QResizeEvent *re = static_cast<QResizeEvent*>(event);
        move( (re->size().width() / 2) - width() / 2,
            (re->size().height() / 2) - height() / 2 );

        //Scale with the parent widget
        resize(sizeHint());

        //Resize fader too
        m_faderWidget->resize(re->size());

    } else if (watched == m_lineEdit && event->type() == QEvent::FocusOut) {
        //If the lineEdit is visible and receives a focus out...
        if (m_lineEdit->isVisible()) {
            //Outa here...
            reject();
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void QuickSelection::accept()
{
    m_result = true;
    hide();
}

void QuickSelection::reject()
{
    m_result = false;
    hide();
}
