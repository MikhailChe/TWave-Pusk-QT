#-------------------------------------------------
#
# Project created by QtCreator 2011-06-30T18:41:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pusk
TEMPLATE = app
DEFINES  = _TTY_WIN_

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
LIBS     += qextserialport.dll
