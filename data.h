#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QColor>

class Data
{
public:
    Data();
    static int voisinage(int i, int j, int state); //Von Neumann

    // STATE DECLARATIONS FOR BETTER UNDERSTANDING
    static const int STATE_GROUND = 0;
    static const int STATE_GRASS = 1;
    static const int STATE_TREES = 2;
    static const int STATE_ON_FIRE = 3;
    static const int STATE_HOT_BURNT = 4;
    static const int STATE_BURNT = 5;

    static const int nb_states = 6; // Number of available states
    static const int grid_size = 64; // Grid size (in units)
    static const int thread_nb = 2;
    static int grid_state[grid_size][grid_size];
    static double grid_energy[grid_size][grid_size];
    static double grid_tree_height[grid_size][grid_size];
    static int grid_to_burn[grid_size][grid_size];
    static const QString state_names[nb_states]; // Names of states (on fire, burnt, etc.)
    static const QColor state_colors[nb_states];
    static bool isStarted;

    // DATA FOR THE SIMULATION  (S.I.I UNITS)
    static const double min_height_trees, max_height_trees, average_height_trees, standard_deviation_trees_height;
    static const double probability; // for percolation simulation

};

#endif // DATA_H
