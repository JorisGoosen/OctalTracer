#-------------------------------------------------
#
# Project created by QtCreator 2017-10-06T21:18:08
#
#-------------------------------------------------

QT	+= core gui opengl quickwidgets qml
#CONFIG	+= console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TexRen
TEMPLATE = app
DESTDIR=..
DEPENDPATH = ..

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        hoofdscherm.cpp \
    mijnglwidget.cpp 

HEADERS += \
        hoofdscherm.h \
    mijnglwidget.h 

FORMS += \
        hoofdscherm.ui

DISTFILES += \
    Shaders/TexRen.vert \
    Shaders/TexRen.frag

RESOURCES += \
    shaders.qrc

INCLUDEPATH += ../Octal

LIBS += -L.. -lOctal



