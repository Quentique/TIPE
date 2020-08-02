#include "simulator.h"
#include "ui_simulator.h"

#include <cmath>
#include <random>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileDialog>
#include <QRandomGenerator>
#include "workerthread.h"
#include "flowlayout.h"
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QRect>
#include <QPoint>
#include <QPainter>
#include <QProcess>

int Simulator::currently_selected_state = 0;
Simulator::Simulator(QWidget *parent)
    : QMainWindow(parent)
{
    isStarted = false;
    isRunning = false;
    record_bool = false;
    calculusState = 0;
    nbThreadDone = 0;
    QGridLayout *main_layout;
    QGroupBox *actions_layout;
    QHBoxLayout *main_state_layout, *button_act_layout, *legend, *dens1, *dens2;
    FlowLayout *map_edition_layout;
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
    map_edition_layout = new FlowLayout;


    simulation_name = new QLineEdit;
    simulation_name->setReadOnly(true);
    simulation_name->setPlaceholderText("Simulation name");
    start_simulation = new QPushButton("Start");
    abort_simulation = new QPushButton("Abort");
    open_file = new QPushButton("Open map");
    save_file = new QPushButton("Save map");
    restart_button = new QPushButton("Reset");
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

    map_edition_group = new QButtonGroup;
    for (int i = 0 ; i<Data::nb_states ; i++)
    {
        QHBoxLayout *container = new QHBoxLayout;
        QLabel *text = new QLabel(Data::state_names[i]);
        QLabel *color = new QLabel("bla");
        color->setStyleSheet("QLabel { color: "+Data::state_colors[i].name()+";  background-color: "+ Data::state_colors[i].name()+";}");
        container->addWidget(color);
        container->addWidget(text);
        legend->addLayout(container);

        QPushButton* button = new QPushButton("     ");
        button->setStyleSheet("background-color: " + Data::state_colors[i].name() +"; border: none;");
        button->setToolTip(Data::state_names[i]);
        // button->setDisabled(true);
        map_edition_group->addButton(button, i);
        map_edition_layout->addWidget(button);
       // buttons_edition_map.append(button);
    }
    map_edition_group->buttons().first()->setText("  X  ");
    connect(map_edition_group, SIGNAL(buttonClicked(int)), this, SLOT(selectionEditionMapButton(int)));
    simulation_name_record = "";


    button_act_layout->addWidget(start_simulation);
    button_act_layout->addWidget(abort_simulation);
    gen_actions_layout->addLayout(button_act_layout);
    gen_actions_layout->addWidget(record);
    gen_actions_layout->addWidget(simulation_name);
    gen_actions_layout->addWidget(save_to_csv);
    actions_layout->setLayout(gen_actions_layout);
    generation_layout->addWidget(actions_layout);
    generation_layout->addWidget(open_file);
    generation_layout->addWidget(save_file);
    generation_layout->addWidget(restart_button);
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    generation_layout->addWidget(line);
    generation_layout->addWidget(createLabel("Map Edition"));
    generation_layout->addWidget(random_map);
    generation_layout->addLayout(map_edition_layout);
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


    // CALCULUS ENGINE INITIALIZATION :
    thread1 = new WorkerThread(this, &lock, &cond, 0);
    thread2 = new WorkerThread(this, &lock, &cond, 1);

    connect(thread1, SIGNAL(calculusEnded()), this, SLOT(calculusDone()));
    connect(thread2, SIGNAL(calculusEnded()), this, SLOT(calculusDone()));
    connect(thread1, SIGNAL(done()), this, SLOT(updateMap()));
    connect(thread2, SIGNAL(done()), this, SLOT(updateMap()));

    // DATA INITIALIZATION :
    connect(random_map, SIGNAL(clicked()), this, SLOT(generateRandom()));
    connect(canvas, SIGNAL(paintEnd()), this, SLOT(resetStatusBar()));
    connect(canvas, SIGNAL(paintEndedThread()), this, SLOT(refreshDone()));
    connect(start_simulation, SIGNAL(clicked()), this, SLOT(startSimulation()));
    connect(record, SIGNAL(stateChanged(int)), this, SLOT(record_box_change(int)));
    connect(abort_simulation, SIGNAL(clicked()), this, SLOT(abordButton()));
    connect(restart_button, SIGNAL(clicked()), this, SLOT(restart()));
    connect(save_file, SIGNAL(clicked()), this, SLOT(serializeData()));
    connect(open_file, SIGNAL(clicked()), this, SLOT(readData()));
    statusBar()->showMessage("Ready");
    grab().save("test.png");
}

Simulator::~Simulator()
{
    thread1->terminate();
    thread2->terminate();
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
    for (int i = 0 ; i<Data::grid_size; i++) {
        for (int j = 0 ; j <Data::grid_size ; j++) {
            QJsonObject obj;
            obj["x"] = i;
            obj["y"] = j;
            obj["state"] = Data::grid_state[i][j];
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
        Data::grid_state[obj["x"].toInt()][obj["y"].toInt()] = obj["state"].toInt();
    }
    canvas->repaint();
}

void Simulator::selectionEditionMapButton(int button) {
    qDebug() << "CALLED EDITION MAPS";
    for (int i = 0 ; i<map_edition_group->buttons().count() ; i++) {
       map_edition_group->buttons()[i]->setText("     ");
    }
    map_edition_group->button(button)->setText("  X  ");
    Simulator::currently_selected_state = button;
}
void Simulator::generateRandom() {
    statusBar()->showMessage("Generating map...");
    int dgb = density->value();
    int dhb = trees_density->value();
   // std::default_random_engine generator;

    for (int i = 0 ; i<Data::grid_size ; i++) {
        for (int j=0; j<Data::grid_size ; j++) {
            int random = QRandomGenerator::global()->bounded(0,100);
            random -= 7*voisinage(i,j, Data::STATE_GROUND);
            if (random < dgb) {
                random = QRandomGenerator::global()->bounded(0,100);
                if (random < dhb) {
                    Data::setupTree(i, j); // Tree on the case
                } else {
                    Data::setupGrass(i, j); // Grass on the case
                }
            } else {
                Data::setupGround(i,j); // Nothing but environmental parameters being initialised
            }
        }
    }
    canvas->repaint();
}

void Simulator::abordButton() {
    //thread1->requestStop();
    //thread2->requestStop();
    start_simulation->setDisabled(false);
    state_label->setText("Stopped");
    setRed(state_label);
    isRunning = false;
    cond.wait(&lock);
}

void Simulator::restart() {
    if (QMessageBox::critical(this, "Restart", "Are you sure ? No data saved.", QMessageBox::Ok | QMessageBox::Discard) == QMessageBox::Ok) {
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}
void Simulator::calculusDone() {
    nbThreadDone += 1;
    qDebug() << "Calculus done";
    qDebug() << Data::thread_nb;
    qDebug() << nbThreadDone;
    if (nbThreadDone >= Data::thread_nb) {
        qDebug() << "Defreezing called";
        nbThreadDone = 0;
        cond.wakeAll();
    }
}

void Simulator::resetStatusBar() {
    statusBar()->showMessage("Ready");
}

void Simulator::saveScreenshot() {
    qDebug() << "being called save Screenshots";
}

void Simulator::refreshDone() {


}
void Simulator::updateMap() {

    nbThreadDone += 1;
    qDebug() << "Update called";
    if (nbThreadDone >= Data::thread_nb) {
        nbThreadDone =0;
        /*for (int i = 0 ; i<Data::grid_size;i++) {
            for (int j = 0 ; j<Data::grid_size;j++) {

                if (Data::grid_energy[i][j] == 100) {
                    Data::grid_state[i][j] = 1;
                } else if (Data::grid_energy[i][j] == 200) {
                    Data::grid_state[i][j] = 2;
                } else {
                    Data::grid_state[i][j] = 0;
                }
            }
        }*/

        canvas->repaint();
        calculusState += 1;
        steps_number->display(calculusState);
        cond.wait(&lock,10000);
        if (isRunning) {
            cond.wakeAll();
        }
    }
}

void Simulator::startSimulation() {
    isRunning = true;
    simulation_name_record = simulation_name->text();
    state_label->setText("Running...");
    setGreen(state_label);
    simulation_name->setReadOnly(true);
    if (isStarted) {
        qDebug() << "Restart requested";
        cond.wakeAll();
    } else {
        for (int i = 0 ; i<Data::grid_size;i++) {
            for (int j = 0 ; j<Data::grid_size;j++) {
                Data::grid_to_burn[i][j] = static_cast<int>(ceil(Data::grid_tree_height[i][j]));
            }
        }
        thread1->start();
        thread2->start();
        isStarted = true;
    }
    start_simulation->setDisabled(true);
    open_file->setDisabled(true);
    random_map->setDisabled(true);
    for (int i = 0 ; i<map_edition_group->buttons().count() ; i++) {
        map_edition_group->buttons()[i]->setDisabled(true);
    }
}

void Simulator::record_box_change(int state) {
    if (state == Qt::Unchecked) {
        simulation_name->setText("");
        simulation_name->setReadOnly(true);
        record_bool = false;
    } else {
        simulation_name->setReadOnly(false);
        record_bool = true;
    }
}

int Simulator::voisinage(int i, int j, int state = 0) {
    int nb = 0; // voisinage Von Neumann
    if (i > 0) {
        nb += Data::grid_state[i-1][j] != state;
        if (j> 0) {
            nb += Data::grid_state[i-1][j-1] != state;
        }
        if (j < Data::grid_size-1) {
            nb += Data::grid_state[i-1][j+1] != state;
        }
    }
    if (j > 0) {
        nb += Data::grid_state[i][j-1] != state;
    }
    if (i < Data::grid_size-1) {
        nb += Data::grid_state[i+1][j] != state;
        if (j > 0) {
            nb += Data::grid_state[i+1][j-1] != state;
        }
        if (j< Data::grid_size-1) {
            nb += Data::grid_state[i+1][j+1] != state;
        }
    }
    if (j < Data::grid_size-1) {
        nb += Data::grid_state[i][j+1] != state;
    }
    return nb;
}

