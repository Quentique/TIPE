#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QLCDNumber>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QReadWriteLock>
#include <QWaitCondition>
#include <QThread>
#include <QVector>
#include "canvas.h"
#include "workerthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Simulator; }
QT_END_NAMESPACE


class Simulator : public QMainWindow
{
    Q_OBJECT

public:
    Simulator(QWidget *parent = nullptr);
    ~Simulator();



public slots:
    void generateRandom();
    void resetStatusBar();
    void updateMap();
    void calculusDone();
    void startSimulation();
    void record_box_change(int state);

    void refreshDone();
    void abordButton();

    void restart();

    void serializeData();
    void readData();

    void selectionEditionMapButton(int button);
    void selectionWindDirection(int button);

private:

    QButtonGroup *map_edition_group, *wind_direction_group;
    QReadWriteLock lock;
    QWaitCondition cond;
    QWidget *window;
    QPushButton *start_simulation, *abort_simulation, *random_map, *restart_button, *open_file, *save_file;
    QCheckBox *record, *save_to_csv;
    QSpinBox *density, *trees_density, *wind_strengh, *ambientHumidity, *ambientTemperature;
    QLabel *state_label, *wind_name;
    QLCDNumber *steps_number;
    Canvas *canvas;
    WorkerThread *thread1, *thread2, *thread3, *thread4, *thread5;
    QLineEdit *simulation_name;
   // QVector<QPushButton*> buttons_edition_map;

    static void setRed(QLabel *pointer);
    static void setGreen(QLabel *pointer);
    static QLabel* createLabel(const QString &text);



    void saveScreenshot();

    int voisinage(int i, int j, int state);

    int calculusState, nbThreadDone;


    QString simulation_name_record;

    bool record_bool;
    bool isStarted, isRunning;
};


#endif // SIMULATOR_H
