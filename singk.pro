#-------------------------------------------------
#
# Project created by QtCreator 2016-01-31T01:22:08
#
#-------------------------------------------------

QT       += core gui webkitwidgets network  multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = singk
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    jstoqtapi.cpp \
    videocontroller.cpp \
    networkcontroller.cpp \
    onlinedialog.cpp \
    serializableobject.cpp \
    message.cpp \
    user.cpp \
    videodownloader.cpp \
    videoplayer.cpp \
    videodestroyer.cpp

HEADERS  += mainwindow.h \
    jstoqtapi.h \
    videocontroller.h \
    networkcontroller.h \
    onlinedialog.h \
    serializableobject.h \
    message.h \
    user.h \
    videodownloader.h \
    videoplayer.h \
    videodestroyer.h \
    definition.h

FORMS    += mainwindow.ui \
    onlinedialog.ui

DISTFILES += \
    tmpl.html
