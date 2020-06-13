#include "simulator.h"
#include "ui_simulator.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileDialog>
#include <QRandomGenerator>

// STATIC DATA INITIALIZATION :
const QString Simulator::state_names[] = {QString("Ground"), QString("Grass"), QString("Tree"), QString("On Fire"), QString("Hot - Burnt"), QString("Burnt")};
const QColor Simulator::state_colors[] = {QColor("tan"), QColor("chartreuse"), QColor("green"), QColor("red"), QColor("firebrick"), QColor("darkgrey")};
int Simulator::grid_state[][Simulator::grid_size] = { {0} };
bool Simulator::isStarted = false;

Simulator::Simulator(QWidget *parent)
    : QMainWindow(parent)
{
    QGridLayout *main_layout;
    QGroupBox *actions_layout;
    QHBoxLayout *main_state_layout, *button_act_layout, *legend, *dens1, *dens2;
    QVBoxLayout *state_layout, *generation_layout, *gen_actions_layout;
    window = new QWidget;
    main_layout = new QGridLayout;
    actions_layout = new QGroupBox("Actions");
    generation_layout = new QVBoxLayout;
    state_layout = new QVBoxLayout;
    gen_actions_layout = new QVBoxLayout;
    main_state_layout = new QHBoxLayout;
    button_act_layout = new QHBoxLayout;
    legend = new QHBoxLayout;
    dens1 = new QHBoxLayout;
    dens2 = new QHBoxLayout;

    start_simulation = new QPushButton("Start");
    abort_simulation = new QPushButton("Abort");
    random_map = new QPushButton("Random map");
    record = new QCheckBox("Record");
    save_to_csv = new QCheckBox("Save data to CSV");
    state_label = new QLabel("Not running...");
    setRed(state_label);
    density = new QSpinBox;
    trees_density = new QSpinBox;
    density->setRange(0,100);
    trees_density->setRange(0,100);
    steps_number = new QLCDNumber;
    steps_number->setDigitCount(3);
    steps_number->display(0);

    canvas = new Canvas;

    for (int i = 0 ; i<Simulator::nb_states ; i++)
    {
        QHBoxLayout *container = new QHBoxLayout;
        QLabel *text = new QLabel(Simulator::state_names[i]);
        QLabel *color = new QLabel("bla");
        color->setStyleSheet("QLabel { color: "+Simulator::state_colors[i].name()+";  background-color: "+ Simulator::state_colors[i].name()+";}");
        container->addWidget(color);
        container->addWidget(text);
        legend->addLayout(container);
    }

    button_act_layout->addWidget(start_simulation);
    button_act_layout->addWidget(abort_simulation);
    gen_actions_layout->addLayout(button_act_layout);
    gen_actions_layout->addWidget(random_map);
    gen_actions_layout->addWidget(record);
    gen_actions_layout->addWidget(save_to_csv);
    actions_layout->setLayout(gen_actions_layout);
    generation_layout->addWidget(actions_layout);
    dens1->addWidget(Simulator::createLabel("Ratio Trees/Ground"));
    dens1->addWidget(density);
    dens2->addWidget(Simulator::createLabel("Ratio Trees/Grass"));
    dens2->addWidget(trees_density);
    generation_layout->addLayout(dens1);
    generation_layout->addLayout(dens2);
    generation_layout->addStretch(1);

    main_state_layout->addWidget(state_label);
    main_state_layout->addStretch(1);
    main_state_layout->addWidget(createLabel("Steps count"));
    main_state_layout->addWidget(steps_number);
    state_layout->addLayout(main_state_layout);
    state_layout->addStretch(1);
    state_layout->addLayout(legend);
    state_layout->addWidget(createLabel("Wild fire simulator"));

    //state_layout->addLayout(main_state_layout);
    main_layout->addLayout(generation_layout, 0, 1);
    main_layout->addLayout(state_layout, 1, 0);
    main_layout->addWidget(canvas, 0, 0);
    main_layout->setColumnStretch(2,1);
    window->setLayout(main_layout);
    setCentralWidget(window);
    setStyleSheet("* { font-family: \"CMU Sans Serif\"; font-size: 14pt; }");

    // DATA INITIALIZATION :
    connect(random_map, SIGNAL(clicked()), this, SLOT(generateRandom()));
    connect(canvas, SIGNAL(paintEnd()), this, SLOT(resetStatusBar()));
    statusBar()->showMessage("Ready");
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
    pointer->setStyleSheet("color: red;");
}
void Simulator::setGreen(QLabel *pointer) {
    pointer->setStyleSheet("color: green;");
}

void Simulator::serializeData() {
    statusBar()->showMessage("Saving data...");
    QJsonArray data;
    for (int i = 0 ; i<Simulator::grid_size; i++) {
        for (int j = 0 ; j <Simulator::grid_size ; j++) {
            QJsonObject obj;
            obj["x"] = i;
            obj["y"] = j;
            obj["state"] = Simulator::grid_state[i][j];
            data.append(obj);
        }
    }
    QString filename = QFileDialog::getSaveFileName(this, "Select file");
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(data).toJson());
    resetStatusBar();
}

void Simulator::readData() {
    QString filename = QFileDialog::getOpenFileName(this, "Open data save");
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QByteArray rawData = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    QJsonArray array = doc.array();
    for (int i = 0 ; i<array.count() ; i++) {
        QJsonObject obj = array[i].toObject();
        grid_state[obj["x"].toInt()][obj["y"].toInt()] = obj["state"].toInt();
    }
    canvas->repaint();
}

void Simulator::generateRandom() {
    statusBar()->showMessage("Generating map...");
    int dgb = density->value();
    int dhb = trees_density->value();
    for (int i = 0 ; i<grid_size ; i++) {
        for (int j=0; j<grid_size ; j++) {
            int random = QRandomGenerator::global()->bounded(0,100);
            random -= 5*voisinage(i,j);
            if (random < dgb) {
                random = QRandomGenerator::global()->bounded(0,100);
                if (random < dhb) {
                    grid_state[i][j] = 2;
                } else {
                    grid_state[i][j] = 1;
                }
            } else {
                grid_state[i][j] = 0;
            }
        }
    }
    canvas->repaint();
}

void Simulator::resetStatusBar() {
    statusBar()->showMessage("Ready");
}

int Simulator::voisinage(int i, int j) {
    int nb = 0; // voisinage Von Neumann
    if (i > 0) {
        nb += grid_state[i-1][j] != 0;
        if (j> 0) {
            nb += grid_state[i-1][j-1] != 0;
        }
        if (j < grid_size-1) {
            nb += grid_state[i-1][j+1] != 0;
        }
    }
    if (j > 0) {
        nb += grid_state[i][j-1] != 0;
    }
    if (i < grid_size-1) {
        nb += grid_state[i+1][j] != 0;
        if (j > 0) {
            nb += grid_state[i+1][j-1] != 0;
        }
        if (j< grid_size-1) {
            nb += grid_state[i+1][j+1] != 0;
        }
    }
    if (j < grid_size-1) {
        nb += grid_state[i][j+1] != 0;
    }
    return nb;
}
