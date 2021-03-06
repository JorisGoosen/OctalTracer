#-------------------------------------------------
#
# Project created by QtCreator 2017-10-06T21:18:08
#
#-------------------------------------------------

QT		+= core gui opengl quickwidgets qml
#CONFIG	+= console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Raycaster
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
    RayCast.frag \
    neutraal.frag \
    neutraal.vert \
    Shaders/neutraal.frag \
    Shaders/RayCast.frag \
    Shaders/neutraal.vert \
    Shaders/RayCast.vert \
    Shaders/Octal.frag \
    Shaders/Octal.vert \
    Shaders/octalstackless.frag

RESOURCES += \
    shaders.qrc

INCLUDEPATH += C:\Users\Falafel\Documents\glm-0.9.9-a1
INCLUDEPATH += ../Octal

LIBS += -L.. -lOctal



