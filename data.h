#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QColor>
#include <QHash>

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
    static const int STATE_WATER = 6;

    static const int WIND_NW = 0;
    static const int WIND_N = 1;
    static const int WIND_NE = 2;
    static const int WIND_W = 3;
    static const int WIND_NO = 4;
    static const int WIND_E = 5;
    static const int WIND_SW = 6;
    static const int WIND_S = 7;
    static const int WIND_SE = 8;

    static const int nb_states = 7; // Number of available states
    static const int grid_size = 64; // Grid size (in units)
    static const int spatialResolution = 30; // = side size of each case in meters
    static const int thread_nb = 2;
    static int grid_state[grid_size][grid_size];
    static double grid_energy[grid_size][grid_size];
    static double grid_tree_height[grid_size][grid_size];
    static double grid_tree_width[grid_size][grid_size];
    static int grid_to_burn[grid_size][grid_size];
    static int grid_flame_temperature[grid_size][grid_size];
    static int grid_case_temperature[grid_size][grid_size];
    static double grid_flame_length[grid_size][grid_size];
    static double grid_moisture[grid_size][grid_size];
    static const QString state_names[nb_states]; // Names of states (on fire, burnt, etc.)
    static const QString wind_directions[9], wind_directions_symbol[9];
    static const QColor state_colors[nb_states];
    static bool isStarted;

    // DATA FOR THE SIMULATION  (S.I.I UNITS)
    static const double min_height_trees, max_height_trees, average_height_trees, standard_deviation_trees_height, average_width_trees, standard_deviation_trees_width, min_width_trees, max_width_trees;
    static const double probability; // for percolation simulation
    static constexpr double average_moisture = 0.42;
    static constexpr double hot_burnt_temperature_limit = 273.15+200.0;
    static constexpr double water_evaporation_temperature = 273.15+100.0;
    static constexpr double pyrolisis_temperature = 273.15+0.0;  // to be completed

    static QHash<int, float> airCp, airCv, airThermalConductivity, airDynamicViscosity, airPrandtl;

    // CALCULUS FUNCTIONS

    static double distance(int i0, int j0, int i1, int j1);
    // GENERATING FUNCTIONS

    static void setupGrass(int i, int j);
    static void setupTree(int i, int j);
    static void setupWater(int i, int j);
    static void setupGround(int i, int j);
    static void setupCase(int i, int j);
    static void setupBurnt(int i, int j);
    static void setupHotBurnt(int i, int j);

    static double generateHumidityLevel(int i, int j);


};

#endif // DATA_H
