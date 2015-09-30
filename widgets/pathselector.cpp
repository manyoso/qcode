#include "pathselector.h"

#include <QFileDialog>

#include "ui_pathselector.h"

PathSelector::PathSelector(QWidget *parent)
    : QWidget(parent), m_directoryMode(false)
{
    m_pod = new Ui::PathSelector;
    m_pod->setupUi(this);

    connect(m_pod->selector, SIGNAL(clicked()), this, SLOT(getPath()));
}

PathSelector::~PathSelector()
{
    delete m_pod;
}

QString PathSelector::path() const { return m_pod->pathEdit->text(); }
void PathSelector::setPath(const QString &path) { m_pod->pathEdit->setText(path); }

void PathSelector::getPath()
{
    QString path;
    if (m_directoryMode) {
        path = QFileDialog::getExistingDirectory(this, QString(),
                                                 m_pod->pathEdit->text(),
                                                 QFileDialog::ShowDirsOnly);
    } else {
        path = QFileDialog::getOpenFileName(this, QString(), m_pod->pathEdit->text());
    }

    if (!path.isEmpty())
        m_pod->pathEdit->setText(path);
}
