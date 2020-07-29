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
    void refresh();
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintGrid(QPainter *painter, int n);
    void updateGrid(QPainter *painter);
signals:
    void paintEnd();
    void paintEndedThread();
private:

    static const int fixed_size = 920; // Size of grid in pixels
    QBrush *whiteBackground;
    QPen *black;
    bool trigger;

};

#endif // CANVAS_H
