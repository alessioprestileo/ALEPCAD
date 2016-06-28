#-------------------------------------------------
#
# Project created by QtCreator 2016-05-30T09:10:32
#
#-------------------------------------------------

CONFIG += c++11

QT       += core \
            gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ALEPCAD
TEMPLATE = app

HEADERS += glwidget.h \
           mainwindow.h \
           shaderhelper.h \
           geometry.h


SOURCES += main.cpp \
           glwidget.cpp \
           mainwindow.cpp \
           shaderhelper.cpp \
           geometry.cpp

FORMS += mainwindow.ui

RESOURCES += resources.qrc
