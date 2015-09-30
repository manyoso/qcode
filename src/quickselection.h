#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <QFrame>
#include <QListView>
#include <QEventLoop>

class QLineEdit;
class QAbstractItemModel;
class QSortFilterProxyModel;

class FaderWidget;
class QuickSelection;

class ListView : public QListView
{
    Q_OBJECT
    friend class QuickSelection;
public:
    ListView(QAbstractItemModel *model, QWidget *parent = 0);
    virtual ~ListView();

public Q_SLOTS:
    void setFilter(const QString &filter);

private:
    QSortFilterProxyModel *m_proxyModel;
};

class QuickSelection : public QFrame
{
    Q_OBJECT

public:
    QuickSelection(QAbstractItemModel *model, QAction *action, QWidget *parent = 0);
    virtual ~QuickSelection();

    QLineEdit *lineEdit() const { return m_lineEdit; }
    QListView *lineView() const { return m_listView; }

    QModelIndexList selectedIndexes() const;

    bool exec();

    virtual QSize sizeHint() const;
    virtual void setVisible(bool visible);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

private Q_SLOTS:
    void accept();
    void reject();

private:
    bool m_result;
    QAction *m_action;
    QLineEdit *m_lineEdit;
    ListView *m_listView;
    QEventLoop *m_eventLoop;
    FaderWidget *m_faderWidget;
};

#endif
