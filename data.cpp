#include "data.h"

// STATIC DATA INITIALIZATION :
const QString Data::state_names[] = {QString("Ground"), QString("Grass"), QString("Tree"), QString("On Fire"), QString("Hot - Burnt"), QString("Burnt")};
const QColor Data::state_colors[] = {QColor("tan"), QColor("chartreuse"), QColor("green"), QColor("red"), QColor("firebrick"), QColor("darkgrey")};
int Data::grid_state[][Data::grid_size] = { {0} };
double Data::grid_energy[][Data::grid_size] = {{0}};
bool Data::isStarted = false;

Data::Data()
{

}
