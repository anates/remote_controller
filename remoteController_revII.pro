#-------------------------------------------------
#
# Project created by QtCreator 2014-12-12T14:45:07
#
#-------------------------------------------------

QT       += core network testlib

QT       -= gui

TARGET = remoteController_revII
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../BlackLib/v2_0/BlackCore.cpp \
    ../BlackLib/v2_0/BlackGPIO.cpp \
    ../BlackLib/v1_0/BlackLib.cpp \
    dpc.cpp \
    scanner.cpp \
    spectrometer.cpp \
    stepper_control.cpp \
    polarizer_control.cpp \
    ../remoteController/tx_thread.cpp \
    ../tcp_client_test/client.cpp \
    ../tcp_client_test/server.cpp \
    controller.cpp

HEADERS += \
    ../BlackLib/v2_0/BlackLib.h \
    ../BlackLib/v2_0/BlackCore.h \
    ../BlackLib/v2_0/BlackDef.h \
    ../BlackLib/v2_0/BlackGPIO.h \
    dpc.h \
    scanner.h \
    spectrometer.h \
    stepper_control.h \
    polarizer_control.h \
    ../remoteController/tx_thread.h \
    ../tcp_client_test/client.h \
    ../tcp_client_test/server.h \
    controller.h

QMAKE_CXXFLAGS += -std=c++11

OTHER_FILES +=
