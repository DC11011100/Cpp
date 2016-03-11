#-------------------------------------------------
#
# Project created by QtCreator 2015-12-29T17:12:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DataMan
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    option.cpp \
    setting.cpp \
    eventhandler.cpp \
    result.cpp \
    filemanager.cpp

HEADERS  += mainwindow.h \
    option.h \
    setting.h \
    eventhandler.h \
    result.h \
    filemanager.h

INCLUDEPATH += C:/boost/boost_1_60_0/
LIBS += "-LC:/boost/boost_1_60_0"

FORMS    += mainwindow.ui
