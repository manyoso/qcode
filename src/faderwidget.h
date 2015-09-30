#ifndef FADERWIDGET_H
#define FADERWIDGET_H

#include <QWidget>

class FaderWidget : public QWidget
{
    Q_OBJECT
public:
    FaderWidget(QWidget *parent);

    void setVisible(bool visible);

    QColor fadeColor() const { return m_color; }
    void setFadeColor(const QColor &color) { m_color = color; m_color.setAlpha(33); }

protected:
    void paintEvent(QPaintEvent *event);

private:
    QColor m_color;
};

#endif
