#-------------------------------------------------
#
# Project created by QtCreator 2016-10-31T17:22:18
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
DESTDIR = bin

TARGET = bat
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    json.cpp \
    port.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    json.h \
    port.h

FORMS    += \
    material.ui \
    mainwindow.ui

RESOURCES += \
    res.qrc

QMAKE_CXXFLAGS += -std=c++0x
