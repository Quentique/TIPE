#include "workerthread.h"
#include "Data.h"

WorkerThread::WorkerThread(QObject *parent, QReadWriteLock *locker, QWaitCondition *condition, int station) : QThread(parent)
{
    lock = locker;
    cond = condition;
    action = station;
}

void WorkerThread::run() {
    forever{
        lock->lockForRead();

        switch (action) {
            case 0:
            for (int i =0 ;i<Data::grid_size; i++) {
                for (int j = 0 ;j<Data::grid_size ; j++ ) {
                    grid[i][j] = test1(i,j);
                }
            }
            break;
        case 1:
            for (int i =0 ;i<Data::grid_size; i++) {
                for (int j = 0 ;j<Data::grid_size ; j++ ) {
                    grid[i][j] = test2(i,j);
                }
            }
            break;
        }
        emit(calculusEnded());
        cond->wait(lock);
        lock->unlock();
        lock->lockForWrite();
        for (int i =0 ;i<Data::grid_size; i++) {
            for (int j = 0 ;j<Data::grid_size ; j++ ) {
                Data::grid_energy[i][j] += grid[i][j];
            }
        }
        emit(done());
        cond->wait(lock);
        lock->unlock();
    }
}
double WorkerThread::test1(int i, int j) {
    if (i%2 ==0) {
        return 100;
    } else {
        return 0;
    }
}

double WorkerThread::test2(int i, int j) {
    if (j%2 == 0) {
        return 0;
    } else {
        return 100;
    }
}
