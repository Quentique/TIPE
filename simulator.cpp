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
#include <QHash>

int Simulator::currently_selected_state = 0;
int Simulator::wind_direction = Data::WIND_NO;
int Simulator::wind_strengh_value = 0;
Simulator::Simulator(QWidget *parent)
    : QMainWindow(parent)
{
    isStarted = false;
    isRunning = false;
    record_bool = false;
    calculusState = 0;
    nbThreadDone = 0;
    QGridLayout *main_layout, *wind_direction;
    QGroupBox *actions_layout;
    QHBoxLayout *main_state_layout, *button_act_layout, *legend, *dens1, *dens2, *wind_layout;
    FlowLayout *map_edition_layout;
    QVBoxLayout *state_layout, *generation_layout, *gen_actions_layout;
    window = new QWidget;
    main_layout = new QGridLayout;
    wind_direction = new QGridLayout;
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
    wind_layout = new QHBoxLayout;


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

    wind_direction_group = new QButtonGroup;

    for (int i = 0; i<9 ; i++) {
        QPushButton *button = new QPushButton(Data::wind_directions_symbol[i]);
        button->setToolTip(Data::wind_directions[i]);
        button->setFlat(true);
        button->setCheckable(true);
        wind_direction_group->addButton(button, i);
        wind_direction->addWidget(button, i/3, i%3);
    }
    wind_direction_group->buttons()[4]->setChecked(true);
    wind_direction_group->setExclusive(true);

    wind_strengh = new QSpinBox;
    wind_strengh->setSuffix("m/s");
    wind_strengh->setToolTip("Wind strengh");
    wind_name = new QLabel("Wind");

    wind_layout->addLayout(wind_direction);
    wind_layout->addSpacing(2);
    wind_layout->addWidget(wind_strengh);

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

    generation_layout->addWidget(line);
    generation_layout->addWidget(createLabel("Wind"));
    generation_layout->addLayout(wind_layout);

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
    connect(map_edition_group, SIGNAL(buttonClicked(int)), this, SLOT(selectionEditionMapButton(int)));
    connect(wind_direction_group, SIGNAL(buttonClicked(int)), this, SLOT(selectionWindDirection(int)));
    statusBar()->showMessage("Loading data...");


    // READING SPECIFIC HEAT CP FROM FILE
    QFile f("cp.txt");
    QHash<int, float> cp;
    if (!f.open(QFile::ReadOnly)) {
        qDebug() << f.errorString();
        QMessageBox::critical(this, "Error", "Can't parse data. Exiting....");
        exit(1);
    } else {
        while (!f.atEnd()) {
            QByteArray line = f.readLine();
            QList<QByteArray> list = line.split(',');
            cp[list.value(0).toInt()] = list.value(1).toFloat();
        }
    }
    f.close();
    Data::airCp = cp;

    // READING SPECIFIC HEAT CV FROM FILE
    QFile ff("cv.txt");
    QHash<int, float> cv;
    if (!ff.open(QFile::ReadOnly)) {
        qDebug() << ff.errorString();
        QMessageBox::critical(this, "Error", "Can't parse data. Exiting....");
        exit(1);
    } else {
        while (!ff.atEnd()) {
            QByteArray line = ff.readLine();
            QList<QByteArray> list = line.split(',');
            cv[list.value(0).toInt()] = list.value(1).toFloat();
        }
    }
    ff.close();
    Data::airCv = cv;

    // READING DYNAMIC VISCOSITY FROM FILE
    QFile fff("dynamic_viscosity.txt");
    QHash<int, float> vs;
    if (!fff.open(QFile::ReadOnly)) {
        qDebug() << fff.errorString();
        QMessageBox::critical(this, "Error", "Can't parse data. Exiting....");
        exit(1);
    } else {
        while (!fff.atEnd()) {
            QByteArray line = fff.readLine();
            QList<QByteArray> list = line.split(',');
            vs[list.value(0).toInt()] = list.value(1).toFloat();
        }
    }
    fff.close();
    Data::airDynamicViscosity = vs;

    // READING THERMAL CONDUCTIVITY FROM FILE
    QFile ffff("thermal_conductivity.txt");
    QHash<int, float> tc;
    if (!ffff.open(QFile::ReadOnly)) {
        qDebug() << ffff.errorString();
        QMessageBox::critical(this, "Error", "Can't parse data. Exiting....");
        exit(1);
    } else {
        while (!ffff.atEnd()) {
            QByteArray line = ffff.readLine();
            QList<QByteArray> list = line.split(',');
            tc[list.value(0).toInt()] = list.value(1).toFloat();
        }
    }
    ffff.close();
    Data::airThermalConductivity = tc;
    statusBar()->showMessage("Computing data...");
    // COMPUTING PRANDTL NUMBER
    QHash<int, float> prandtl;
    for (int i = 273 ; i< 2000;i++) {
        prandtl[i] = cp[i]*vs[i]/tc[i];
    }
    Data::airPrandtl = prandtl;
    qDebug() << Data::airThermalConductivity[1400];
    qDebug() << Data::airCp[1400];
    qDebug() << Data::airDynamicViscosity[1400];
    qDebug() << Data::airPrandtl[1400];
    statusBar()->showMessage("Ready.");
    //grab().save("test.png");
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
            obj["tree_height"] = Data::grid_tree_height[i][j];
            obj["moisture"] = Data::grid_moisture[i][j];
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
        Data::grid_tree_height[obj["x"].toInt()][obj["y"].toInt()] = obj["tree_height"].toDouble();
        Data::grid_moisture[obj["x"].toInt()][obj["y"].toInt()] = obj["moisture"].toDouble();
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

void Simulator::selectionWindDirection(int button) {
    Simulator::wind_direction = button;
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
    if (QMessageBox::critical(this, "Restart", "Are you sure ? No data was saved.", QMessageBox::Ok | QMessageBox::Discard) == QMessageBox::Ok) {
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
    Simulator::wind_strengh_value = wind_strengh->value();
    wind_strengh->setDisabled(true);
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
    for (int i = 0 ; i<wind_direction_group->buttons().count() ; i++) {
        wind_direction_group->buttons()[i]->setDisabled(true);
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

