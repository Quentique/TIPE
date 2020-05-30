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
    state_label = new QLabel("Not running...");
    setRed(state_label);
    steps_number = new QLCDNumber;
    steps_number->setDigitCount(3);
    steps_number->display(0);

    canvas = new Canvas;

    button_act_layout->addWidget(start_simulation);
    button_act_layout->addWidget(abort_simulation);
    gen_actions_layout->addLayout(button_act_layout);
    gen_actions_layout->addWidget(record);
    gen_actions_layout->addWidget(save_to_csv);
    actions_layout->setLayout(gen_actions_layout);
    generation_layout->addWidget(actions_layout);
    generation_layout->addStretch(1);

    main_state_layout->addWidget(state_label);
    main_state_layout->addStretch(1);
    main_state_layout->addWidget(createLabel("Steps count"));
    main_state_layout->addWidget(steps_number);
    state_layout->addLayout(main_state_layout);
    state_layout->addStretch(1);
    state_layout->addWidget(createLabel("Wild fire simulator"));

    //state_layout->addLayout(main_state_layout);
    main_layout->addLayout(generation_layout, 0, 1);
    main_layout->addLayout(state_layout, 1, 0);
    main_layout->addWidget(canvas, 0, 0);
    window->setLayout(main_layout);
    setCentralWidget(window);
    setStyleSheet("* { font-family: \"CMU Sans Serif\"; font-size: 14pt; }");
}

Simulator::~Simulator()
{

}

QLabel *Simulator::createLabel(const QString &text)
{
    QLabel *label = new QLabel(text);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    return label;
}

void Simulator::setRed(QLabel *pointer) {
    pointer->setStyleSheet(" color: red;");
}
void Simulator::setGreen(QLabel *pointer) {
    pointer->setStyleSheet("color: green;");
}
