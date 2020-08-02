#include "data.h"
#include <QRandomGenerator>

// STATIC DATA INITIALIZATION :
const QString Data::state_names[] = {QString("Ground"), QString("Grass"), QString("Tree"), QString("On Fire"), QString("Hot - Burnt"), QString("Burnt"), QString("Water")};
const QColor Data::state_colors[] = {QColor("tan"), QColor("chartreuse"), QColor("green"), QColor("red"), QColor("firebrick"), QColor("darkgrey"), QColor("blue")};
int Data::grid_state[][Data::grid_size] = { {0} };

double Data::grid_energy[][Data::grid_size] = {{0}};
double Data::grid_tree_height[][Data::grid_size] = {{0}};
double Data::grid_moisture[][Data::grid_size] = {{0}};
bool Data::isStarted = false;

int Data::grid_to_burn[][Data::grid_size] = { {0} }; // FOR PERCOLATION MODELING ONLY

const double Data::min_height_trees = 2.0;
const double Data::max_height_trees = 5.0;
const double Data::average_height_trees = (Data::min_height_trees+Data::max_height_trees)/2;
const double Data::standard_deviation_trees_height = 0.8;

const double Data::probability = 0.4;
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
