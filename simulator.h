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
#include <QReadWriteLock>
#include <QWaitCondition>
#include <QThread>
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
    static int voisinage(int i, int j, int state);
    ~Simulator();

public slots:
    void generateRandom();
    void resetStatusBar();
    void updateMap();
    void calculusDone();
    void startSimulation();
    void record_box_change(int state);

    void refreshDone();

private:

    QReadWriteLock lock;
    QWaitCondition cond;
    QWidget *window;
    QPushButton *start_simulation, *abort_simulation, *random_map;
    QCheckBox *record, *save_to_csv;
    QSpinBox *density, *trees_density;
    QLabel *state_label;
    QLCDNumber *steps_number;
    Canvas *canvas;
    WorkerThread *thread1, *thread2;
    QLineEdit *simulation_name;

    static void setRed(QLabel *pointer);
    static void setGreen(QLabel *pointer);
    static QLabel* createLabel(const QString &text);

    void serializeData();
    void readData();

    void saveScreenshot();



    int calculusState, nbThreadDone;

    QString simulation_name_record;

    bool record_bool;

};


#endif // SIMULATOR_H
