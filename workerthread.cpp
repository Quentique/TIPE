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
      //  double tau = 75600/Data::sigma;
        double kcoeff = Data::A*exp(-Data::Ea/(Data::CONSTANT_GAS*Data::grid_case_temperature[k][l])); // constante cinétique
      double new_mass = Data::grid_mass_to_burn[i][j]*exp(-kcoeff*Data::dt); // calcul de la nouvelle masse et du delta
      // double mass_loss = Data::grid_mass_to_burn[k][l]/tau;
        double Qheat = (Data::grid_mass_to_burn[i][j] - new_mass)*Data::combustion_enthalpy;
      //  qDebug
      //  qDebug() << "K coeff, flame radiation calculus " << kcoeff;

        double flame_power = Data::part_of_lost_heat*Qheat/(3.14159*Data::grid_tree_width[k][l]*Data::grid_flame_length[k][l]);
        double flame_emissivity = 1-exp(-0.6*Data::grid_flame_length[k][l]);// < 1

        double power = flame_power*3.14159*Data::grid_tree_width[k][l]*Data::grid_flame_length[k][l]*flame_emissivity;
       // double coeff = sqrt(2)*sqrt(pow(Data::lxm/(2*std::max((double)(abs(k-i)), Data::lxm+1)),2)+pow(Data::lym/(2*std::max((double)(abs(l-j)),Data::lym+1)),2)+pow(Data::lyp/(2*std::max((double)(abs(l-j)),Data::lyp+1)),2)+pow(Data::lxp/(2*std::max((double)(abs(k-i)),Data::lxp+1)),2));
        // POWER DIMINUTION COEFFICIENT CALCULUS
        double to_add = pow(Data::lxm/(4*std::max((double)abs(k-i), Data::lxm+10)), 2)+pow(Data::lym/(4*std::max((double)abs(l-j),Data::lym+10)),2);

        if (k-i >= 0) { // signifie que le feu est à droite de la case
            if (Data::wind_direction == Data::WIND_W) {
                to_add += pow(Data::lxp/std::max((double)(abs(k-i)), Data::lxm+1), 2);
            }
            if (Data::wind_direction == Data::WIND_NW || Data::wind_direction == Data::WIND_SW) {
                to_add += 2*pow(Data::lxp/(2*std::max((double)abs(k-i), Data::lxp+1)),2);
            }
        } else { // feu à gauche de la case
            if (Data::wind_direction == Data::WIND_E) {
                to_add += pow(Data::lxp/std::max((double)(abs(k-i)), Data::lxm+1), 2);
            }
            if (Data::wind_direction == Data::WIND_NE || Data::wind_direction == Data::WIND_SE) {
                to_add += 2*pow(Data::lxm/(2*std::max((double)abs(k-i), Data::lxm+1)),2);
            }
        }

        if (l-j >= 0) { // feu en bas de la case
            if (Data::wind_direction == Data::WIND_N){
                to_add += pow(Data::lyp/std::max((double)abs(l-j), Data::lyp+1),2);
            }
            if (Data::wind_direction == Data::WIND_NW || Data::wind_direction == Data::WIND_NE) {
                to_add += 2*pow(Data::lyp/(2*std::max((double)abs(l-j), Data::lyp+1)),2);
            }
        } else {
            if (Data::wind_direction == Data::WIND_S) {
                to_add += pow(Data::lym/std::max((double)abs(l-j), Data::lym+1),2);
            }
            if (Data::wind_direction == Data::WIND_SE || Data::wind_direction == Data::WIND_SW) {
                to_add += 2*pow(Data::lym/(2*std::max((double)abs(l-j), Data::lym+1)),2);
            }
        }
        //to_add /= 4;

        double coeff = sqrt(to_add)*exp(-(Data::distance(i,j,k,l))/(Data::spatialResolution));
        if (i == 1 && j == 1) {
           // qDebug() << "test 2 " << std::max((double)(abs(k-i)), Data::lx+1);
            qDebug() << "Nouveau coefficient de radiations des flammes : " << coeff;

          //  qDebug() << "Radiations flamme " << power*(pow(((k-i)/Data::lx),2)+pow(((l-j)/Data::ly),2));
            qDebug() << "Radiations de la flamme : " << power*coeff;
        }
         return power*coeff;
       // return 0.0;
    } else { return 0.0; }
}
