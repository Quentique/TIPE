#include "workerthread.h"
#include "Data.h"
#include <QRandomGenerator>
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
                    grid_state[i][j] = test1(i,j);
                }
            }
            break;
        case 1:
            for (int i =0 ;i<Data::grid_size; i++) {
                for (int j = 0 ;j<Data::grid_size ; j++ ) {
                    grid_to_burn[i][j] = test2(i,j);
                }
            }
            break;
        }
        emit(calculusEnded());
        cond->wait(lock);
        lock->unlock();
        lock->lockForWrite();
        switch (action) {
            case 0:
            for (int i =0 ;i<Data::grid_size; i++) {
                for (int j = 0 ;j<Data::grid_size ; j++ ) {
                    if (grid_state[i][j] != -1) {
                        Data::grid_state[i][j] = grid_state[i][j];
                    }
                }
            }
            break;

        case 1:
            for (int i =0 ;i<Data::grid_size; i++) {
                for (int j = 0 ;j<Data::grid_size ; j++ ) {
                    Data::grid_to_burn[i][j] += grid_to_burn[i][j];
                }
            }
            break;
        }
        emit(done());
        cond->wait(lock);
        lock->unlock();
    }
}
double WorkerThread::test1(int i, int j) {
    // SET ON FIRE ACCORDING TO CURRENT STATE
    if ((Data::grid_state[i][j] == Data::STATE_GRASS || Data::grid_state[i][j] == Data::STATE_TREES) && Data::voisinage(i, j, Data::STATE_ON_FIRE) > 1 && QRandomGenerator::global()->generateDouble() <= Data::probability) {
        return Data::STATE_ON_FIRE;
    } else if (Data::grid_state[i][j] == Data::STATE_ON_FIRE && Data::grid_to_burn[i][j] == 0){
        return Data::STATE_HOT_BURNT;
    } else if (Data::grid_state[i][j] == Data::STATE_HOT_BURNT) {
        return Data::STATE_BURNT;
    } else {
        return -1;
    }
}

double WorkerThread::test2(int i, int j) {
    // BURNING...
    if (Data::grid_to_burn[i][j] > 0 && Data::grid_state[i][j] == Data::STATE_ON_FIRE) {
        return -1;
    } else {
        return 0;
    }
}
