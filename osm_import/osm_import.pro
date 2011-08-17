#-------------------------------------------------
#
# Project created by QtCreator 2011-08-18T00:40:19
#
#-------------------------------------------------

QT       -= gui

TARGET = osm_import
CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH += osmium/include
LIBS += -losmpbf -lprotobuf -lz -lexpat

TEMPLATE = app


SOURCES += main.cpp
