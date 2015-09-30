#include "faderwidget.h"

#include <QPainter>
#include <QPaintEvent>

FaderWidget::FaderWidget(QWidget *parent)
    : QWidget(parent)
{
    m_color = parent->palette().window().color();
    m_color.setAlpha(100);
}

void FaderWidget::setVisible(bool visible)
{
    if (visible)
        resize(parentWidget()->size());
    QWidget::setVisible(visible);
}

void FaderWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(), m_color);
}
