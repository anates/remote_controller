#include <QCoreApplication>
#include "controller.h"
#include <QFile>
#include <QTextStream>



void myMessageHandler(QtMsgType type, const char *msg)
{
QString txt;
switch (type) {
case QtDebugMsg:
txt = QString("Debug: %1").arg(msg);
break;
case QtWarningMsg:
txt = QString("Warning: %1").arg(msg);
break;
case QtCriticalMsg:
txt = QString("Critical: %1").arg(msg);
break;
case QtFatalMsg:
txt = QString("Fatal: %1").arg(msg);
abort();
}
QFile outFile("log");
outFile.open(QIODevice::WriteOnly | QIODevice::Append);
QTextStream ts(&outFile);
ts << txt << endl;
}

int main(int argc, char *argv[])
{
    qRegisterMetaType<Polarizer>();
    qRegisterMetaType<QPair<QString, QVariant> >();
    qRegisterMetaType<QPair<int, int> >();
    QCoreApplication a(argc, argv);
    qDebug() << "Test!";
    controller newController;
    return a.exec();
}
