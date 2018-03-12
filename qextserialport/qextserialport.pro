
PROJECT                 = qextserialport
TEMPLATE                = lib

CONFIG                 -= debug_and_release
#CONFIG                 += debug
CONFIG                 += release
CONFIG                 += warn_on qt thread
QT                     -= gui

OBJECTS_DIR             = build/obj
MOC_DIR                 = build/moc
DEPENDDIR               = .
INCLUDEDIR              = .
HEADERS                 = qextserialbase.h \
                          qextserialport.h
SOURCES                 = qextserialbase.cpp \
                          qextserialport.cpp


HEADERS          += win_qextserialport.h
SOURCES          += win_qextserialport.cpp
DEFINES          += _TTY_WIN_

DESTDIR                 = build
TARGET                  = qextserialport

