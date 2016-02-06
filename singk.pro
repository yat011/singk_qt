#-------------------------------------------------
#
# Project created by QtCreator 2016-01-31T01:22:08
#
#-------------------------------------------------

QT       += core gui webkitwidgets network multimedia multimediawidgets qml quick declarative

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
    tmpl.html \
    video.qml

RESOURCES += \
    res.qrc

QML2_IMPORT_PATH=C:\Qt\5.5\msvc2013_64\qml
unix:!macx:!symbian: LIBS += -L$${QML2_IMPORT_PATH}
unix:!android {
    isEmpty(target.path) {
        qnx {
            target.path = /tmp/$${TARGET}/bin
        } else {
            target.path = /opt/$${TARGET}/bin
        }
        export(target.path)
    }
    INSTALLS += target
}

export(INSTALLS)
