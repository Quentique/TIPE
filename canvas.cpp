#include "canvas.h"
#include "Simulator.h"

#include <QPainter>
#include <QPaintEvent>
#include <QMessageBox>
#include<QDebug>

Canvas::Canvas() : QOpenGLWidget()
{
    whiteBackground = new QBrush(Qt::white);
    black = new QPen(Qt::black);
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
    paintGrid(&painter, Simulator::grid_size);
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

void Canvas::updateGrid(QPainter *painter, int grid[]){
    for (int i = 0 ; i < Simulator::grid_size; i++) {
        for (int j = 0; j<Simulator::grid_size; j++) {

        }
    }
}
void Canvas::mousePressEvent(QMouseEvent *event) {

}

int Canvas::caseNumber(int xpos, int ypos, int grid_size, int widget_size) {
    return xpos/(widget_size/grid_size)+ypos/(widget_size/grid_size)*grid_size;
}
