#-------------------------------------------------
#
# Project created by QtCreator 2011-06-30T18:41:52
#
#-------------------------------------------------

QT       += core gui network

TARGET = pusk
TEMPLATE = app
DEFINES  = _TTY_WIN_

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
LIBS     += qextserialport.dll

OTHER_FILES +=
