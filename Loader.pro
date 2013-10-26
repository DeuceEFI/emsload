#-------------------------------------------------
#
# Project created by QtCreator 2013-10-09T20:15:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += debug
TARGET = Loader
TEMPLATE = app

include(serialport/apmserial.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    loaderthread.cpp \
    s19file.cpp

HEADERS  += mainwindow.h \
    loaderthread.h \
    s19file.h

FORMS    += mainwindow.ui
