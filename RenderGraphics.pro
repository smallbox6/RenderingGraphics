#-------------------------------------------------
#
# Project created by QtCreator 2016-12-06T20:22:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RenderGraphics
TEMPLATE = app


SOURCES += main.cpp\
        renderwindow.cpp \
    Matrix.cpp \
    controller.cpp \
    pipeline.cpp

HEADERS  += renderwindow.h \
    Matrix.h \
    basicelement.h \
    controller.h \
    pipeline.h

RESOURCES += \
    qrc.qrc
