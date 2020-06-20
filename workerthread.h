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
    double test1(int i, int j);
    double test2(int i, int j);

signals:
    void calculusEnded();
    void done();
private:
    QReadWriteLock* lock;
    QWaitCondition* cond;
    double grid[Data::grid_size][Data::grid_size] ={{0}};
    int action;
    int* data[][Data::grid_size];
};

#endif // WORKERTHREAD_H
