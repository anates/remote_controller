#ifndef SPECTROMETER_H
#define SPECTROMETER_H
#include <QVector>
#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>
#include "dpc.h"
#include "scanner.h"
#include "stepper_control.h"
#include <polarizer_control.h>

class Spectrometer: public QObject
{
    Q_OBJECT
private:
    QMutex *MovingMutex;
    QWaitCondition *MovingCond;
    QMutex *CountingMutex;
    QWaitCondition *CountingCond;
    //int currentMonoPos;

    DPC_Master *dpcControl;
    polarizer_control_master polarizerControl;
    Scanner_Master *scannerControl;
    Stepper_Control_Master *stepperControl;
private slots:
    //Inside wiring
    //Polarizer
    void switchingSuccess(Polarizer pol);
    //Stepper
    void stepperMoved(int steps, bool dir);
    //DPC
    void currentCounts(int counts);
    //Scanner
    void currentData(QPair<int, int> data);
    void updatePosition(int steps, bool dir);
    void currentScanPosition(qreal position);
    void scanFinished(void);
public slots:
    //From outside
    void switchPol(Polarizer pol);
    void moveToTarget(int steps, bool dir);
    void runScan(int start, int stop, int accuracy);
    //void changePosition(int MonoPos);
signals:
    //Internal
    //To polarizer
    void switchPolarizer(Polarizer);
    //to Stepper
    void moveStepperToTarget(int, bool);
    //to scanner
    void scanNow(int, int, int);
    void interruptScan(void);
    //to extern
    //from scanner
    void Data(QPair<int, int> data);
    void DPCCounts(int counts);
    void scanFinish(void);
    void ScanPos(qreal position);
    //From stepper
    void currentPosition(int steps, bool dir);
    void stepperMoving(void);
    //from polarizercontrol
    void switchingSucceed(Polarizer pol);
public:
    Spectrometer(QMutex *mutex, QWaitCondition *WaitCond);
    ~Spectrometer();

};

class Spectrometer_Control:public QObject
{
    Q_OBJECT
    QThread workerThread;
private:
    int MonoPos = 0;
    QVector<bool> polarizerSetting;
    QMutex *MovingMutex, *CountMutex;
    QWaitCondition *MovingCond, *CountCond;
public slots:
    //Internal connections
    //to DPC
    void scanData(QPair<int, int> data);
    void counterData(int counts);
    //to stepper
    void updateCurrentPosition(int steps, bool dir);
    void stepperIsMoving(void);
    //to polarizer controller
    void updatePolarizers(Polarizer pol);
    //to scanner
    void scanFinish(void);
    void scanPosition(qreal position);
signals:
    //Internal
    void switchPolarizer(Polarizer pol);
    void moveStepperToTarget(int steps, bool dir);
    void runScan(int start, int stop, int accuracy);
    void initMonoPos(int MonoPos);
    //External
    void currentData(QPair<int, int> data);
    void scanFinished(void);
    void currentScanPosition(qreal position);
    void currentCounterData(int counts);
    void positionChanged(void);
    void stepperMoving(void);
public:
    Spectrometer_Control(QMutex *mutex, QWaitCondition *WaitForEngine);
    ~Spectrometer_Control();

    void setMonoPos(int pos);
    int getMonoPos(void);
    void setPolarizers(Polarizer pol, bool state);
    void setPolarizers(QVector<bool> polarizers);
    bool getPolarizerState(Polarizer pol);
    QVector<bool> getPolarizers(void);
    void scan(int start, int stop, int accuracy);
    void moveStepper(int steps, bool dir);
};

#endif // SPECTROMETER_H
