#ifndef CANVAS_H
#define CANVAS_H

#include <QOpenGLWidget>
#include <QWidget>

class Canvas : public QOpenGLWidget
{
    Q_OBJECT

public:
    Canvas();
    static int caseNumber(int xpos, int ypos, int grid_size, int widget_size);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintGrid(QPainter *painter, int n);
    void updateGrid(QPainter *painter, int grid[]);
private:

    static const int fixed_size = 920; // Size of grid in pixels
    QBrush *whiteBackground;
    QPen *black;

};

#endif // CANVAS_H
