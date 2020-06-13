#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLCDNumber>
#include <QGroupBox>
#include <QHBoxLayout>
#include "canvas.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Simulator; }
QT_END_NAMESPACE

class Simulator : public QMainWindow
{
    Q_OBJECT

public:
    Simulator(QWidget *parent = nullptr);
    ~Simulator();
    static const int nb_states = 6; // Number of available states
    static const int grid_size = 16; // Grid size (in units)
    static int grid_state[grid_size*grid_size];
    static const QString state_names[nb_states]; // Names of states (on fire, burnt, etc.)
    static const QColor state_colors[nb_states];

private:


    QWidget *window;
    QPushButton *start_simulation, *abort_simulation, *random_map;
    QCheckBox *record, *save_to_csv;
    QSpinBox *density;
    QLabel *state_label;
    QLCDNumber *steps_number;
    Canvas *canvas;

    static void setRed(QLabel *pointer);
    static void setGreen(QLabel *pointer);
    static QLabel* createLabel(const QString &text);
};
#endif // SIMULATOR_H
