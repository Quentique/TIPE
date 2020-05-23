#include "simulator.h"
#include "ui_simulator.h"

Simulator::Simulator(QWidget *parent)
    : QMainWindow(parent)
{
    QGridLayout *main_layout;
    QGroupBox *actions_layout;
    QHBoxLayout *main_state_layout, *button_act_layout;
    QVBoxLayout *state_layout, *generation_layout, *gen_actions_layout;
    window = new QWidget;
    main_layout = new QGridLayout;
    actions_layout = new QGroupBox("Actions");
    generation_layout = new QVBoxLayout;
    state_layout = new QVBoxLayout;
    gen_actions_layout = new QVBoxLayout;
    main_state_layout = new QHBoxLayout;
    button_act_layout = new QHBoxLayout;

    start_simulation = new QPushButton("Start");
    abort_simulation = new QPushButton("Abort");
    record = new QCheckBox("Record");
    save_to_csv = new QCheckBox("Save data to CSV");

    button_act_layout->addWidget(start_simulation);
    button_act_layout->addWidget(abort_simulation);
    gen_actions_layout->addLayout(button_act_layout);
    gen_actions_layout->addWidget(record);
    gen_actions_layout->addWidget(save_to_csv);
    actions_layout->setLayout(gen_actions_layout);
    generation_layout->addWidget(actions_layout);

    //state_layout->addLayout(main_state_layout);
    main_layout->addLayout(generation_layout, 1, 0);
    //main_layout->addLayout(state_layout, 0, 1);
   // main_layout->addLayout(gen_actions_layout, 2,1);
    window->setLayout(main_layout);
    setCentralWidget(window);
}

Simulator::~Simulator()
{

}

