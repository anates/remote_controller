#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QFile>
#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include "spectrometer.h"
#include "../remoteController/tx_thread.h"


class controller:public QObject
{
    Q_OBJECT
private:
    QMutex mutex;
    QWaitCondition waitCond;
    QString ipAddr;
    quint32 port = 50000;
    Spectrometer_Control *newSpectrometer;
    TX_master *mainTX, *dataTX;
    Server *MainServer, *FileServer;
    QVector<QPair<int, int> > CorrectionValues;
    bool calibrated = false;
    int currentCounts = 0;
private slots:
    //From Mainserver:
    void gotNewConnection(QVariant data);
    //From FileServer:
    void FSgotNewconnection(QVariant data);
    //from mainTX
    void gotData(QPair<QString, QVariant> data);
    void wrongIP(void);
    //from dataTX
    void DataGotData(QPair<QString, QVariant> data);
    void DataWrongIP(void);
    //From spectrometer
    void currentData(QPair<int, int> data);
    void scanFinished(void);
    void currentScanPosition(double position);
    void currentCounterData(int counts);
    void positionChanged(void);
    void stepperMoving(void);
signals:
    //To MainServer:
    void sendDataMain(QPair<QString, QVariant>);

    //To FileServer:
    void sendFile(QVariant);
    //To mainTX
    void connectTX(void);
    //To dataTX
    void DataConnectTX(void);
public:
    controller();
    ~controller();

    void sortPoints();
    void writeConfig();
    void readConfig();
};

#endif // CONTROLLER_H
