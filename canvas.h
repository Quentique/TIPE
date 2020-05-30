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
    static const int grid_size = 32; // Grid size (in units)
    static const int fixed_size = 920; // Size of grid in pixels
    static const int nb_states = 5; // Number of available states
    int grid_state[grid_size*grid_size] = {0};
    QBrush *whiteBackground;
    QPen *black;
    QString state_names[nb_states]; // Names of states (on fire, burnt, etc.)
};

#endif // CANVAS_H
