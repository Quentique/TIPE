#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLCDNumber>
#include <QGroupBox>
#include <QHBoxLayout>

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
    QWidget *window;
    QPushButton *start_simulation, *abort_simulation;
    QCheckBox *record, *save_to_csv;
    QSpinBox *density;
    QLabel *state;

    QLabel createLabel(const QString &text);
};
#endif // SIMULATOR_H
