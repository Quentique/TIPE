#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class Simulator; }
QT_END_NAMESPACE

class Simulator : public QMainWindow
{
    Q_OBJECT

public:
    Simulator(QWidget *parent = nullptr);
    ~Simulator();

private:
    QLabel *presentation;
};
#endif // SIMULATOR_H
