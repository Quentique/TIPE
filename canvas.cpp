#include "canvas.h"

#include <QPainter>
#include <QPaintEvent>

Canvas::Canvas() : QOpenGLWidget()
{
    whiteBackground = new QBrush(Qt::white);
    black = new QPen(Qt::black);
    QSizePolicy size = *new QSizePolicy;
    size.setHeightForWidth(true);
    setSizePolicy(size);
    setFixedSize(920,920);
    setAutoFillBackground(true);
    paintEvent(nullptr);
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(event->rect(), Qt::white);
    painter.drawLine(0,0,10,10);
    painter.end();
}
