# -------------------------------------------------
# Project created by QtCreator 2009-06-14T19:17:49
# -------------------------------------------------
TARGET = EARComm
TEMPLATE = app
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
SOURCES += main.cpp \
    earcomm.cpp \
    data.cpp
HEADERS += earcomm.h \
    data.h
FORMS += earcomm.ui
LIBS += -lserialport
INCLUDEPATH += /usr/local/include
LIBPATH += /usr/local/lib

DISTFILES += \
    events.dat \
    fips.dat
APP_DAT_FILES.files = events.dat fips.dat
APP_DAT_FILES.path = Contents/MacOS
QMAKE_BUNDLE_DATA += APP_DAT_FILES
