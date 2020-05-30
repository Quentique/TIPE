#ifndef CANVAS_H
#define CANVAS_H

#include <QOpenGLWidget>
#include <QWidget>

class Canvas : public QOpenGLWidget
{
    Q_OBJECT
public:
    Canvas();
protected:
    void paintEvent(QPaintEvent *event) override;
    void paintGrid(QPainter *painter, int n);
private:
    int grid_size = 5;
    int fixed_size = 920;
    QBrush *whiteBackground;
    QPen *black;
};

#endif // CANVAS_H
