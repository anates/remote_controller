#include "controller.h"

controller::controller()
{
    qDebug() << "New controller created!";
    newSpectrometer = new Spectrometer_Control(&mutex, &waitCond);
    qCritical() << "Initializing controller";
    MainServer = new Server("127.0.0.1", port, "MAIN");
    FileServer = new Server("127.0.0.1", port + 2, "FILE");
    mainTX = NULL;
    connect(this, &controller::sendDataMain, MainServer, &Server::sendData);
    connect(this, &controller::sendFile, FileServer, &Server::sendFile);
    connect(MainServer, &Server::gotNewConnection, this, &controller::gotNewConnection);
    connect(FileServer, &Server::gotNewConnection, this, &controller::FSgotNewconnection);

}

controller::~controller()
{
    delete MainServer;
    delete FileServer;
    delete newSpectrometer;
    delete mainTX;
}

void controller::writeConfig()
{
    QString filename = "Config.txt";
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << file.errorString();
        return;
    }
    QTextStream out(&file);
    QString CalibrationData;
    if(calibrated == true)
    {

        for(int i = 0; i < CorrectionValues.size(); i++)
        {
            CalibrationData += QString::number(CorrectionValues[i].first) + " " + QString::number(CorrectionValues[i].second) + " ";
        }
    }
    out << newSpectrometer->getMonoPos() << '\n';
    //out << newSpectrometer->getMonoSpeed() << '\n';
    out << (int)(newSpectrometer->getPolarizers()[0])*1 + (int)(newSpectrometer->getPolarizers()[1])*2 + (int)(newSpectrometer->getPolarizers()[2])*4 << '\n';
    if(calibrated == true)
        out << CalibrationData;
    file.close();
}

void controller::gotNewConnection(QVariant data)
{
    qDebug() << "MainServer got new connection from " << data.toString();
    controller::ipAddr = data.toString();
    mainTX = new TX_master(ipAddr, port + 1, "MAIN");
    qDebug() << "Created new Client, client is gonna started!";
    qDebug() << "Connecting 1";
    connect(this, &controller::connectTX, mainTX, &TX_master::connectTX);
//    qDebug() << "Connecting 2";
//    connect(this, &controller::DataConnectTX, dataTX, &TX_master::connectTX);
    qDebug() << "Connecting 3";
    connect(mainTX, &TX_master::gotNewData, this, &controller::gotData);
    qDebug() << "Connecting 4";
    connect(mainTX, &TX_master::wrongIP, this, &controller::wrongIP);
    qDebug() << "Connecting finished";
    emit connectTX();
}

void controller::gotData(QPair<QString, QVariant> data)
{
    if(data.first == "STP")
    {
        qDebug() << "Moving stepper: " << data.second.toInt();
        newSpectrometer->moveStepper(data.second.toInt(), data.second.toInt() >= 0);
    }
    if(data.first == "SCN")
    {
        //Somehow I have to get data over here for scan...
    }
    if(data.first == "POL")
    {
        int newPolarizerState = data.second.toInt();
        if(newPolarizerState == 0)
        {
            newSpectrometer->setPolarizers(xPol, !newSpectrometer->getPolarizerState(xPol));
            emit sendDataMain(qMakePair(QString("POL"), QVariant(xPol)));
        }
        if(newPolarizerState == 1)
        {
            newSpectrometer->setPolarizers(yPol, !newSpectrometer->getPolarizerState(yPol));
            emit sendDataMain(qMakePair(QString("POL"), QVariant(yPol)));
        }
        if(newPolarizerState == 2)
        {
            newSpectrometer->setPolarizers(zPol, !newSpectrometer->getPolarizerState(zPol));
            emit sendDataMain(qMakePair(QString("POL"), QVariant(zPol)));
        }
    }
    if(data.first == "CTL")
    {
        switch(data.second.toInt())
        {
            case 0://Return current MonoPos
            {
                QVariant tmp(newSpectrometer->getMonoPos());
                emit sendDataMain(qMakePair(QString("STP"), tmp));
                break;
            }
            case 1://return current DPC value
            {
                QVariant tmp(currentCounts);
                emit sendDataMain(qMakePair(QString("DPC"), tmp/*Hier muss der aktuelle Zählerstand eingebaut werden*/));
                break;
            }
            case 2://return Polarizer state
            {
                int currentPos = 1*(int)newSpectrometer->getPolarizers()[0] + 2*(int)newSpectrometer->getPolarizers()[1] + 4*(int)newSpectrometer->getPolarizers()[2];
                QVariant tmp(currentPos);
                emit sendDataMain(qMakePair(QString("POL"), tmp));
                break;
            }
            default:
            {
                //do nothing
            }
        }
    }
    if(data.first == "FIL")
    {
        //Schick das aktuelle Datenfile rüber
    }
    //everything else is missing
}

void controller::DataGotData(QPair<QString, QVariant> data)
{
    //do something with data here
}

void controller::DataWrongIP()
{
    qDebug() << "No connection to target ip!";
}

void controller::FSgotNewconnection(QVariant data)
{
    if(data.toString() != ipAddr)
        qDebug() << "An error occured!";
}

void controller::wrongIP()
{
    qDebug() << "No connection to target ip!";
}

void controller::sortPoints()
{
    qSort(CorrectionValues);
}

void controller::readConfig()
{
    QString filename = "Config.txt";
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "No config file found!";
        return;
    }
    QTextStream in(&file);
    QString monoPos, monoSpeed;
    QString calibData;
    QStringList Data;
    int PolarizerConfig;
    monoPos = in.readLine();//in >> monoPos;
    qDebug() << "MonoPos from Config: " << monoPos;
    //in >> monoSpeed;
    PolarizerConfig = in.readLine().toInt();//in >> PolarizerConfig;
    qDebug() << "PolarizerConfig from Config: " << PolarizerConfig;
    calibData = in.readLine();
    qDebug() << "Calibration data: " << calibData;
    if(calibData.isEmpty())
        calibrated = false;
    else
    {
        Data = calibData.split(" ");
        qDebug() << "Data size is: " << Data.size();
        if(Data.size()%2 == 0)
        {
            for(int i = 0; i < Data.size(); i+=2)
            {
                CorrectionValues.push_back(qMakePair(Data[i].toInt(), Data[i+1].toInt()));
            }
        }
        else
        {
            Data.pop_back();
            for(int i = 0; i < Data.size(); i+=2)
            {
                CorrectionValues.push_back(qMakePair(Data[i].toInt(), Data[i+1].toInt()));
            }
        }
        qDebug() << "Size of the corrected values: " << CorrectionValues.size();
        if(CorrectionValues.size() > 0)
        {
            sortPoints();
            calibrated = true;
            qDebug() << "Yay, new correction values!";
        }
    }


    newSpectrometer->setMonoPos(monoPos.toDouble());
    //newSpectrometer->setMonoSpeed(monoSpeed.toDouble());
    newSpectrometer->setPolarizers(xPol, (PolarizerConfig % 2 == 1)?true:false );
    newSpectrometer->setPolarizers(yPol, (PolarizerConfig == 2 || PolarizerConfig == 3 || PolarizerConfig == 6)?true:false);
    newSpectrometer->setPolarizers(zPol, (PolarizerConfig >= 4)?true:false);
}

void controller::scanFinished()
{
    QVariant tmp(0);
    emit sendDataMain(qMakePair(QString("SCN"), tmp));
}

void controller::positionChanged()
{
    QVariant tmp(0);//Bullshit, muss steps ausgeben
    emit sendDataMain(qMakePair(QString("STP"), tmp));
}

void controller::currentCounterData(int counts)
{
    QVariant tmp(counts);
    emit sendDataMain(qMakePair(QString("DPC"), tmp));
}

void controller::currentScanPosition(double position)
{
    QVariant tmp(position);
    emit sendDataMain(qMakePair(QString("SCN"), tmp));//Muss überprüft werden
}

void controller::currentData(QPair<int, int> data)
{
    QStringList tmp;
    tmp.push_back(QString::number(data.first));
    tmp.push_back(QString::number(data.second));
    QVariant tmp2(tmp);
    emit sendDataMain(qMakePair(QString("SCN"), tmp2));
}

void controller::stepperMoving()
{
    QVariant tmp(1);
    emit sendDataMain(qMakePair(QString("STP"), tmp));
}
//Polarizer fehlt noch
