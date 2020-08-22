#include "workerthread.h"
#include "Data.h"
#include <QRandomGenerator>
#include <QtDebug>
WorkerThread::WorkerThread(QObject *parent, QReadWriteLock *locker, QWaitCondition *condition, int station) : QThread(parent)
{
    lock = locker;
    cond = condition;
    action = station;
    stopRequested = false;
}

void WorkerThread::run() {
    forever{
        lock->lockForRead();
        if (stopRequested) {
            break;
        }
        switch (action) {
            case 0:
            for (int k = 0; k<Data::grid_size; k++) {
                for (int l = 0; l<Data::grid_size; l++) { // CASE EN FEU
                    for (int i =0 ;i<Data::grid_size; i++) { // CASE RECEVANT L'ENERGIE
                        for (int j = 0 ;j<Data::grid_size ; j++ ) {
                            grid_energy[i][j] += convection(k, l, i,j);
                        }
                    }
                }
            }

            break;
        case 1:
            for (int k = 0; k<Data::grid_size; k++) {
                for (int l = 0; l<Data::grid_size; l++) { // CASE EN FEU
                    for (int i =0 ;i<Data::grid_size; i++) { // CASE RECEVANT L'ENERGIE
                        for (int j = 0 ;j<Data::grid_size ; j++ ) {
                            grid_energy[i][j] += internal_convection(k, l, i,j);
                        }
                    }
                }
            }
            break;
        case 2:
            for (int k = 0; k<Data::grid_size; k++) {
                for (int l = 0; l<Data::grid_size; l++) { // CASE EN FEU
                    for (int i =0 ;i<Data::grid_size; i++) { // CASE RECEVANT L'ENERGIE
                        for (int j = 0 ;j<Data::grid_size ; j++ ) {
                            grid_energy[i][j] += ember_radiation(k, l, i,j);
                           // qDebug() << ember_radiation(k,l,i,j);
                        }
                    }
                }
            }
            break;
        case 3:
            for (int i =0 ;i<Data::grid_size; i++) {
                for (int j = 0 ;j<Data::grid_size ; j++ ) {
                    grid_energy[i][j] += radiation_loss(i,j);
                }
            }
            break;
        case 4:
            for (int k = 0; k<Data::grid_size; k++) {
                for (int l = 0; l<Data::grid_size; l++) { // CASE EN FEU
                    for (int i =0 ;i<Data::grid_size; i++) { // CASE RECEVANT L'ENERGIE
                        for (int j = 0 ;j<Data::grid_size ; j++ ) {
                            grid_energy[i][j] += flame_radiation(k,l,i,j);
                           // qDebug() << ember_radiation(k,l,i,j);
                        }
                    }
                }
            }
            break;
        }
        emit(calculusEnded());
        cond->wait(lock);
        lock->unlock();
        lock->lockForWrite();
       /* switch (action) {
            case 0:
            for (int i =0 ;i<Data::grid_size; i++) {
                for (int j = 0 ;j<Data::grid_size ; j++ ) {
                    if (grid_state[i][j] != -1) {
                        Data::grid_energy[i][j] += grid_energy[i][j];
                    }
                }
            }
            break;

        case 1:
            for (int i =0 ;i<Data::grid_size; i++) {
                for (int j = 0 ;j<Data::grid_size ; j++ ) {
                    Data::grid_energy[i][j] += grid_energy[i][j];
                }
            }
            break;
        }*/
        for (int i =0 ;i<Data::grid_size; i++) {
            for (int j = 0 ;j<Data::grid_size ; j++ ) {
                if (grid_state[i][j] != -1) {
                    Data::grid_energy[i][j] += grid_energy[i][j];
                }
            }
        }
        emit(done());
        cond->wait(lock);
        lock->unlock();
    }
}

void WorkerThread::requestStop() {
    lock->lockForWrite();
    stopRequested = true;
    lock->unlock();
}

void WorkerThread::requestRestart() {
    lock->lockForWrite();
    stopRequested = false;
    lock->unlock();
    run();
}
double WorkerThread::convection(int k, int l, int i, int j) {
    // FROM SITE kl on fire to site ij
  /*  // SET ON FIRE ACCORDING TO CURRENT STATE
    if ((Data::grid_state[i][j] == Data::STATE_GRASS || Data::grid_state[i][j] == Data::STATE_TREES) && Data::voisinage(i, j, Data::STATE_ON_FIRE) >= 1 && QRandomGenerator::global()->generateDouble() <= Data::probability) {
        return Data::STATE_ON_FIRE;
    } else if (Data::grid_state[i][j] == Data::STATE_ON_FIRE && Data::grid_to_burn[i][j] == 0){
        return Data::STATE_HOT_BURNT;
    } else if (Data::grid_state[i][j] == Data::STATE_HOT_BURNT) {
        return Data::STATE_BURNT;
    } else {
        return -1;
    }*/

   // CONVECTION
    if (Data::grid_state[k][l] == Data::STATE_ON_FIRE && k != i && l != j) {
        double dist = Data::distance(k,l,i,j);
        double h = 0.037*Data::airThermalConductivity[Data::grid_case_temperature[k][l]]/dist*pow(Data::wind_strengh_value*dist/Data::airKinematicViscosity[Data::grid_case_temperature[k][l]], 0.8)*pow(Data::airPrandtl[Data::grid_case_temperature[k][l]], 1/3);

        if (i == 1 && j ==1) {
            qDebug() << "Convection de surface : " << (h/Data::grid_delta_eff[i][j]*(Data::grid_flame_temperature[k][l]-Data::grid_case_temperature[i][j])*exp(-dist/(3.33*Data::grid_flame_length[k][l])));
        }
        double conv = ((h/Data::grid_delta_eff[i][j])*(Data::grid_flame_temperature[k][l]-Data::grid_case_temperature[i][j])*exp(-dist/(3.33*Data::grid_flame_length[k][l])));
        if (conv > 0) {
            return conv;
        } else { return 0.0; }

    } else {
        return 0.0;
    }
}

double WorkerThread::internal_convection(int k, int l, int i, int j) {
    /*
    // BURNING...
    if (Data::grid_to_burn[i][j] > 0 && Data::grid_state[i][j] == Data::STATE_ON_FIRE) {
        return -1;
    } else {
        return 0;
    }
    */
    // INTERNAL CONVECTION
    if (Data::grid_state[k][l] == Data::STATE_ON_FIRE && k != i && l != j) {
        double dist = Data::distance(k,l,i,j);
        double h = 0.911*Data::airThermalConductivity[Data::grid_case_temperature[i][j]]/Data::diameter_branch*pow(Data::wind_strengh_value*Data::diameter_branch/Data::airKinematicViscosity[Data::grid_case_temperature[i][j]], 0.385)*pow(Data::airPrandtl[Data::grid_case_temperature[i][j]], 1/3);

        if (i == 1 && j == 1) {
            qDebug() << "INTERNAL CONVECTION ----- " << k << " & " << l;
            qDebug() << "Distance : " << dist;
            qDebug() << "h : " << h;
            qDebug() << "Delta : " << Data::grid_delta[i][j];
            qDebug() << "Temperature flamme : " << Data::grid_flame_temperature[k][l];
            qDebug() << "Temperature case " << Data::grid_case_temperature[i][j];
            qDebug() << "Convection interne : " << (4/Data::grid_delta[i][j]*h*(Data::grid_flame_temperature[k][l]-Data::grid_case_temperature[i][j])*exp(-dist/Data::grid_delta[i][j]));
            qDebug() << "END INTERNAL CONVECTION ";
        }
        double conv = (4/Data::grid_delta[i][j]*h*(Data::grid_flame_temperature[k][l]-Data::grid_case_temperature[i][j])*exp(-dist/Data::grid_delta[i][j]));
        if (conv > 0) {
            return conv;
        } else { return 0.0; }

    } else {
        return 0.0;
    }

}

double WorkerThread::ember_radiation(int k, int l, int i, int j) {
    if (Data::grid_state[k][l] == Data::STATE_ON_FIRE && k != i && l != j){
        double dist = Data::distance(k,l,i,j);
        if (i == 1 && j== 1) {
           // qDebug() << "radiation des braises " << 1/Data::grid_delta[i][j]*Data::emissivity*Data::CONSTANT_Stephane_Boltzmann*pow(Data::grid_flame_temperature[k][l],4)*exp(-dist/Data::grid_delta[i][j]);
        }
        return 1/Data::grid_delta[i][j]*Data::emissivity*Data::CONSTANT_Stephane_Boltzmann*pow(Data::grid_flame_temperature[k][l],4)*exp(-dist/Data::grid_delta[i][j]); // always > 0
    } else {
        return 0.0;
    }
}

double WorkerThread::radiation_loss(int k, int l) {
    if (Data::grid_state[k][l] == Data::STATE_ON_FIRE) {

        return -1/Data::grid_delta_eff[k][l]*Data::CONSTANT_Stephane_Boltzmann*Data::emissivity*(pow(Data::grid_flame_temperature[k][l],4)-pow(Data::ambientTemperature(k,l),4)); // always < 0
    } else {return 0.0; }
}

double WorkerThread::flame_radiation(int k, int l, int i, int j) {
    if (Data::grid_state[k][l] == Data::STATE_ON_FIRE) {
        double tau = 75600/Data::sigma;
       // double k = Data::A*exp(-Data::Ea/(Data::CONSTANT_GAS*Data::grid_case_temperature[i][j])); // constante cinétique
        //double new_mass = Data::grid_mass_to_burn[i][j]*exp(-k*Data::dt); // calcul de la nouvelle masse et du delta
        double mass_loss = Data::grid_mass_to_burn[k][l]/tau;
        double Qheat = mass_loss*Data::combustion_enthalpy;


        double flame_power = Data::part_of_lost_heat*Qheat/(3.14159*Data::grid_tree_width[k][l]*Data::grid_flame_length[k][l]);
       // double flame_emissivity = 1-exp(-0.6*Data::grid_flame_length[k][l]);// < 1

        double power = flame_power*3.14159*Data::grid_tree_width[k][l]*Data::grid_flame_length[k][l]; // no use zurzeit


        if (i == 1 && j == 1) {
            qDebug() << "Radiations flamme " << power*(pow(((k-i)/Data::lx),2)+pow(((l-j)/Data::ly),2));
        }
         return power*(pow(((k-i)/Data::lx),2)+pow(((l-j)/Data::ly),2));
       // return 0.0;
    } else { return 0.0; }
}
