#-------------------------------------------------
#
# Project created by QtCreator 2016-01-31T01:22:08
#
#-------------------------------------------------

QT       += core gui webkitwidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = singk
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    jstoqtapi.cpp \
    videocontroller.cpp \
    networkcontroller.cpp

HEADERS  += mainwindow.h \
    jstoqtapi.h \
    videocontroller.h \
    networkcontroller.h

FORMS    += mainwindow.ui

DISTFILES += \
    tmpl.html
