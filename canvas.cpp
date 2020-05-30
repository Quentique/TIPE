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
    setFixedSize(fixed_size+2,fixed_size+2);
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
    paintGrid(&painter, grid_size);
    painter.end();
}

void Canvas::paintGrid(QPainter *painter, int n){
    double pas = fixed_size/n;
    for (int i = 0 ; i<n+1 ; i++){
        painter->drawLine(0, pas*i, fixed_size, pas*i);
        for (int j = 0; j<n+1; j++) {
            painter->drawLine(pas*i, 0, pas*i, fixed_size);
        }
    }
}
