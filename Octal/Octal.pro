
QT		+= core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Octal
TEMPLATE = lib
DESTDIR = ..
#CONFIG = staticlib


SOURCES += \
    Perlin.cpp \
    octal.cpp \
    octalnode.cpp \
    cubeintersect.cpp \
    octaltex.cpp \
    simpletexture.cpp

HEADERS += \
    Perlin.h \
    shaderstorage.h \
    commonfunctions.h \
    octal.h \
    octalnode.h \
    samplerfunctions.h \
    cubeintersect.h \
    octaltex.h \
    simpletexture.h \
    samplerfuncdefs.h

