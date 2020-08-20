#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QReadWriteLock>
#include <QWaitCondition>
#include "data.h"

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(QObject *parent = nullptr, QReadWriteLock *locker = nullptr, QWaitCondition *condition = nullptr, int station = 0);
    void run() override;
    double convection(int k, int l, int i, int j);
    double internal_convection(int k, int l, int i, int j);
    double ember_radiation(int k, int l, int i, int j);
    double radiation_loss(int k, int l);
    double flame_radiation(int k, int l, int i, int j);
    void requestStop();
    void requestRestart();
signals:
    void calculusEnded();
    void done();
private:
    bool stopRequested;

    QReadWriteLock* lock;
    QWaitCondition* cond;
    double grid_energy[Data::grid_size][Data::grid_size] ={{0}};
    int grid_state[Data::grid_size][Data::grid_size] = {{0}};
    int grid_to_burn[Data::grid_size][Data::grid_size] = {{0}};
    int action;
    int* data[][Data::grid_size];
};

#endif // WORKERTHREAD_H
