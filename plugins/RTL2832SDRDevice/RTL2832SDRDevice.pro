#-------------------------------------------------
#
# Project created by QtCreator 2014-01-03T13:01:57
#
#-------------------------------------------------
#RTL2832 original source code: git clone git://git.osmocom.org/rtl-sdr.git
# autoreconf -i
# ./configure
# make
# sudo make install

#Project common
include(../../application/pebbleqt.pri)
#DESTDIR is set in pebbleqt.pri
DESTDIR = $${DESTDIR}/plugins

#Common library dependency code for all Pebble plugins
include (../DigitalModemExample/fix_plugin_libraries.pri)
#This anchors @rpath references in plugins to our lib directory, always at the same level os plugin directory
QMAKE_LFLAGS += -rpath $${DESTDIR}/../lib

#Custom rtl libraries that have to be relocated
rtl1.path += $${DESTDIR}
rtl1.commands += install_name_tool -change /usr/local/lib/librtlsdr.0.dylib  @rpath/librtlsdr.0.dylib $${DESTDIR}/lib$${TARGET}.dylib
INSTALLS += rtl1

#Copy rtlsdr dylib to Pebble Frameworks dir
#We don't want to overwrite and exists() seems to always return true
#Try removing directory first

macx {
	rtl2.files += /usr/local/lib/librtlsdr.0.dylib
	rtl2.files += /opt/local/lib/libusb-1.0.0.dylib
	rtl2.path = $${DESTDIR}/../lib
	INSTALLS += rtl2
}

#We have to fixup librtlsdr dependencies
rtl3.path += $${DESTDIR}
rtl3.commands += install_name_tool -change /opt/local/lib/libusb-1.0.0.dylib  @rpath/libusb-1.0.0.dylib $${DESTDIR}/lib$${TARGET}.dylib
INSTALLS += rtl3

QT += widgets
#For TCP
QT += network

TEMPLATE = lib
CONFIG += plugin

TARGET = rtl2832sdrDevice
VERSION = 1.0.0

SOURCES += rtl2832sdrdevice.cpp

HEADERS += rtl2832sdrdevice.h

#Help plugin not worry about include paths
INCLUDEPATH += ../../application
DEPENDPATH += ../../application
INCLUDEPATH += ../../pebblelib
DEPENDPATH += ../../pebblelib
#Just for rtl-sdr
INCLUDEPATH += ../../rtl-sdr/include
DEPENDPATH += ../../rtl-sdr/include

LIBS += -L$${PWD}/../../pebblelib/$${LIB_DIR} -lpebblelib
LIBS += -L$${PWD}/../../rtl-sdr/src/.libs -lrtlsdr

OTHER_FILES += \
	fix_plugin_libraries.pri

FORMS += \
    rtl2832sdrdevice.ui