# -------------------------------------------------
# Project created by QtCreator 2011-08-18T00:40:19
# -------------------------------------------------
QT -= gui
TARGET = osm_import
CONFIG += console
CONFIG -= app_bundle
INCLUDEPATH += osmium/include
LIBS += -losmpbf \
	-lprotobuf \
	-lz \
	-lexpat \
	-lqjson \
	-lgeos
TEMPLATE = app
SOURCES += main.cpp \
	CategoryMapper.cpp
HEADERS += TapOsmHandler.h \
	CouchOutput.h \
	Entity.h \
	CategoryMapper.h
DEFINES += OSMIUM_WITH_GEOS
