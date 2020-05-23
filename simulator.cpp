#include "simulator.h"
#include "ui_simulator.h"

Simulator::Simulator(QWidget *parent)
    : QMainWindow(parent)
{
    presentation = new QLabel(this);
}

Simulator::~Simulator()
{

}

