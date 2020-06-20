#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QColor>

class Data
{
public:
    Data();
    static const int nb_states = 6; // Number of available states
    static const int grid_size = 64; // Grid size (in units)
    static const int thread_nb = 2;
    static int grid_state[grid_size][grid_size];
    static double grid_energy[grid_size][grid_size];
    static const QString state_names[nb_states]; // Names of states (on fire, burnt, etc.)
    static const QColor state_colors[nb_states];
    static bool isStarted;
};

#endif // DATA_H
