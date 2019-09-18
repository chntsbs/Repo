#-------------------------------------------------
#
# Project created by QtCreator 2017-07-06T14:24:51
#
#-------------------------------------------------
QT       += core gui
QT       += core
QT       += network
QT       += widgets serialport
QT       += sql
QTPLUGIN += qsqlite
QT       += multimedia
LIBS +=-L/usr/lib/crypto++ -lcrypto++
INCLUDEPATH +=-l/usr/include/crypto++



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PSM
CONFIG   += console
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    scproutine.cpp \
    serial.cpp \
    clrc.cpp \
    customerscreen.cpp \
    database.cpp \
    global.cpp \
    main_routine.cpp \
    crc_16.cpp \
    mytcpsocket.cpp \
    crypto.cpp \
    keyboard.cpp \
    savetofile.cpp

HEADERS  += mainwindow.h \
    scproutine.h \
    serial.h \
    clrc.h \
    customerscreen.h \
    database.h \
    global.h \
    main_routine.h \
    crc_16.h \
    mytcpsocket.h \
    config.h \
    crypto.h \
    keyboard.h \
    ui_keyboard.h \
    savetofile.h

FORMS    += mainwindow.ui


OTHER_FILES +=

RESOURCES += \
    icons.qrc

DISTFILES +=
