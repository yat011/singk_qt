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
    networkcontroller.cpp \
    clienthandler.cpp \
    onlinedialog.cpp \
    serializableobject.cpp \
    message.cpp

HEADERS  += mainwindow.h \
    jstoqtapi.h \
    videocontroller.h \
    networkcontroller.h \
    clienthandler.h \
    onlinedialog.h \
    serializableobject.h \
    message.h

FORMS    += mainwindow.ui \
    onlinedialog.ui

DISTFILES += \
    tmpl.html
