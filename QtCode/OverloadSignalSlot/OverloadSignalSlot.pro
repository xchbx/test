#-------------------------------------------------
#
# Project created by QtCreator 2018-06-16T11:16:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OverloadSignalSlot
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    subwindow.cpp

HEADERS  += widget.h \
    subwindow.h

FORMS    += widget.ui \
    subwindow.ui

CONFIG += C++11
