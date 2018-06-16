#-------------------------------------------------
#
# Project created by QtCreator 2018-06-15T22:03:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AlternateDisplayWindow
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    subwindow.cpp

HEADERS  += widget.h \
    subwindow.h

FORMS    += widget.ui \
    subwindow.ui
