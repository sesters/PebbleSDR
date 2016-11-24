#-------------------------------------------------
#
# Project created by QtCreator 2014-02-13T18:40:05
#
#-------------------------------------------------

#Project common
include(../../application/pebbleqt.pri)
#DESTDIR is set in pebbleqt.pri
INSTALL_DIR = $${DESTDIR}
DESTDIR = $${DESTDIR}/plugins

#Common library dependency code for all Pebble plugins
include (../DigitalModemExample/fix_plugin_libraries.pri)

#Help plugin not worry about include paths
INCLUDEPATH += ../../application
DEPENDPATH += ../../application
INCLUDEPATH += ../../pebblelib
DEPENDPATH += ../../pebblelib

#Required for options UI
QT += widgets

TARGET = SoftRockSDRDevice
VERSION = 1.0.0
TEMPLATE = lib
CONFIG += plugin

SOURCES += softrocksdrdevice.cpp

HEADERS += softrocksdrdevice.h

LIBS += -L$${PWD}/../../pebblelib/$${LIB_DIR} -lpebblelib

OTHER_FILES += \
        fix_plugin_libraries.pri

FORMS += \
    softrockoptions.ui
