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
    static const int spatialResolution = 20; // = side size of each case in meters
    static constexpr double dt = 10.0; // pas de calcul, en s
    static const int thread_nb = 5;
    static int grid_state[grid_size][grid_size];
    static double grid_energy[grid_size][grid_size]; // J
    static double grid_tree_height[grid_size][grid_size]; // m
    static double grid_tree_width[grid_size][grid_size]; // m
    static double grid_mass_to_burn[grid_size][grid_size]; // kg
    static double grid_water_mass[grid_size][grid_size]; // kg
    static double grid_delta_eff[grid_size][grid_size]; // wu
    static double grid_delta[grid_size][grid_size]; // wu
    static double grid_alpha[grid_size][grid_size]; // wu
    static int grid_to_burn[grid_size][grid_size]; // unused
    static double  grid_flame_temperature[grid_size][grid_size]; // K
    static double grid_case_temperature[grid_size][grid_size]; // K
    static double grid_flame_length[grid_size][grid_size]; // m
    static double grid_moisture[grid_size][grid_size]; // %
    static const QString state_names[nb_states]; // Names of states (on fire, burnt, etc.)
    static const QString wind_directions[9], wind_directions_symbol[9];
    static const QColor state_colors[nb_states];
    static bool isStarted;

    // DATA FOR THE SIMULATION  (S.I.I UNITS)
    static const double min_height_trees, max_height_trees, average_height_trees, standard_deviation_trees_height, average_width_trees, standard_deviation_trees_width, min_width_trees, max_width_trees;
    static const double probability; // for percolation simulation
    static constexpr double average_moisture = 0.42; // %
    static constexpr double hot_burnt_temperature_limit = 273.15+200.0; // K
    static constexpr double water_evaporation_temperature = 273.15+100.0; // K
    static constexpr double pyrolisis_temperature = 561.0;  // to be completed
    static constexpr double ignition_temperature = 561.0;

    static constexpr double volumic_mass_DFF = 512.0; //kg/m^3
    static constexpr double surface_mass_DFF = 0.313; // kg/m^2
    static constexpr double surface_mass_water = 0.0182; //kg/m^2
    static constexpr double cp_water = 4180.0; // J/kg/K
    static constexpr double water_latente_heat =  2264760.0; // J/kg
    static constexpr double emissivity = 0.8;
    static constexpr double combustion_enthalpy = 14454000.0; // J/kg
    static constexpr double absorptivity = 0.8; // a
    static constexpr double sigma = 12240.0; // 1/m (rapport surface volume des aiguilles)
    static constexpr double part_of_lost_heat = 0.35;
    static constexpr double diameter_branch = 0.00252; // m

    static constexpr double CONSTANT_Stephane_Boltzmann = 5.670374e-8; // SI UNITS
    static constexpr double CONSTANT_GAS = 8.314462; // SI UNITS

    static constexpr double A = 646934.285; // s^-1
    static constexpr double Ea = 182680.0; // J/mol

    static double lx, ly;

    static QHash<int, double> airCp, airCv, airThermalConductivity, airDynamicViscosity, airPrandtl, airDensity, airKinematicViscosity;

    static int currently_selected_state, wind_direction, wind_strengh_value, ambientTemperatureValue, ambientHumidityValue;

    // CALCULUS FUNCTIONS

    static double distance(int i0, int j0, int i1, int j1);
    static double ambientTemperature(int i, int j);
    // GENERATING FUNCTIONS

    static void setupGrass(int i, int j);
    static void setupTree(int i, int j);
    static void setupWater(int i, int j);
    static void setupGround(int i, int j);
    static void setupCase(int i, int j);
    static void setupBurnt(int i, int j);
    static void setupHotBurnt(int i, int j);
    static void setupFire(int i, int j);

    static double generateHumidityLevel(int i, int j);


};

#endif // DATA_H
