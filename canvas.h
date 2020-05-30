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
private:
    QBrush *whiteBackground;
    QPen *black;
};

#endif // CANVAS_H
