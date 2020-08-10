#include "data.h"
#include <QRandomGenerator>

// STATIC DATA INITIALIZATION :
const QString Data::state_names[] = {QString("Ground"), QString("Grass"), QString("Tree"), QString("On Fire"), QString("Hot - Burnt"), QString("Burnt"), QString("Water")};
const QString Data::wind_directions[] = { QString("NW"),  QString("N"),QString("NE"),  QString("W"),QString("No"),QString("E"), QString("SW"), QString("S"), QString("SE") };
const QString Data::wind_directions_symbol[] = {   QString("↖"),QString("↑"),QString("↗"), QString("←"),QString("O"), QString("→"), QString("↙"), QString("↓"), QString("↘") };
const QColor Data::state_colors[] = {QColor("tan"), QColor("chartreuse"), QColor("green"), QColor("red"), QColor("firebrick"), QColor("darkgrey"), QColor("blue")};
int Data::grid_state[][Data::grid_size] = { {0} };
int Data::grid_case_temperature[][Data::grid_size] = { {-1 } };
int Data::grid_flame_temperature[][Data::grid_size] = { {-1 } };

int Data::currently_selected_state = 0;
int Data::wind_direction = Data::WIND_NO;
int Data::wind_strengh_value = 0;

double Data::grid_energy[][Data::grid_size] = {{0}};
double Data::grid_tree_height[][Data::grid_size] = {{0}};
double Data::grid_tree_width[][Data::grid_size] = {{0}};
double Data::grid_moisture[][Data::grid_size] = {{0}};
double Data::grid_flame_length[][Data::grid_size] = {{0}};
double Data::grid_delta[][Data::grid_size] = {{0}};
double Data::grid_delta_eff[][Data::grid_size] = {{0}};
double Data::grid_alpha[][Data::grid_size] = {{0}};
bool Data::isStarted = false;

QHash<int, float> Data::airCp = QHash<int, float>();
QHash<int, float> Data::airCv = QHash<int, float>();
QHash<int, float> Data::airThermalConductivity = QHash<int, float>();
QHash<int, float> Data::airDynamicViscosity = QHash<int, float>();
QHash<int, float> Data::airPrandtl = QHash<int, float>();
int Data::grid_to_burn[][Data::grid_size] = { {0} }; // FOR PERCOLATION MODELING ONLY

const double Data::min_height_trees = 2.0; // in meters
const double Data::max_height_trees = 5.0;
const double Data::min_width_trees = 1.0;
const double Data::max_width_trees = 5.0;
const double Data::average_height_trees = (Data::min_height_trees+Data::max_height_trees)/2;
const double Data::average_width_trees = (Data::min_width_trees+Data::max_width_trees)/2;
const double Data::standard_deviation_trees_height = 0.8;
const double Data::standard_deviation_trees_width = 1.0;

const double Data::probability = 0.2; // PERCOLATION MODELING
Data::Data()
{

}

void Data::setupCase(int i, int j) {
    Data::grid_moisture[i][j] = generateHumidityLevel(i,j);
}
void Data::setupTree(int i, int j) {
    Data::grid_state[i][j] = 2;
    std::normal_distribution<double> distribution(Data::average_height_trees, Data::standard_deviation_trees_height);
    double height = distribution(*QRandomGenerator::global());
    if (height < Data::min_height_trees) {
        Data::grid_tree_height[i][j] = Data::min_height_trees;
    } else if (height > Data::max_height_trees) {
        Data::grid_tree_height[i][j] = Data::max_height_trees;
    } else {
        Data::grid_tree_height[i][j] = height;
    }

    std::normal_distribution<double> distribution2(Data::average_width_trees, Data::standard_deviation_trees_width);

}

void Data::setupGrass(int i, int j) {
    Data::grid_state[i][j] = Data::STATE_GRASS;
    Data::grid_tree_height[i][j] = 0.45;
}

void Data::setupGround(int i, int j) {
    Data::grid_state[i][j] = Data::STATE_GROUND;
}

void Data::setupWater(int i, int j){
    Data::grid_state[i][j] = Data::STATE_WATER;
    Data::grid_moisture[i][j] = 1.0;
}

int Data::voisinage(int i, int j, int state = 0) {
    int nb = 0; // voisinage Von Neumann
    if (i > 0) {
        nb += Data::grid_state[i-1][j] == state;
        if (j> 0) {
            nb += Data::grid_state[i-1][j-1] == state;
        }
        if (j < Data::grid_size-1) {
            nb += Data::grid_state[i-1][j+1] == state;
        }
    }
    if (j > 0) {
        nb += Data::grid_state[i][j-1] == state;
    }
    if (i < Data::grid_size-1) {
        nb += Data::grid_state[i+1][j] == state;
        if (j > 0) {
            nb += Data::grid_state[i+1][j-1] == state;
        }
        if (j< Data::grid_size-1) {
            nb += Data::grid_state[i+1][j+1] == state;
        }
    }
    if (j < Data::grid_size-1) {
        nb += Data::grid_state[i][j+1] == state;
    }
    return nb;
}

double Data::generateHumidityLevel(int i, int j){
    int voi = voisinage(i, j, Data::STATE_WATER);
    if (voi == 0) {
        return average_moisture;
    } else {
        double level = exp(-1/voi);
        if (level < average_moisture) {
            return average_moisture;
        } else {
            return level;
        }

    }
}

double Data::distance(int i0, int j0, int i1, int j1) {
    return hypot(abs(j1-j0), abs(i1-i0))*Data::spatialResolution;
}

double Data::ambientTemperature(int i, int j) {
    return Data::ambientTemperatureValue;
}
