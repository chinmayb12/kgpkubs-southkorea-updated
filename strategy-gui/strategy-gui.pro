#-------------------------------------------------
#
# Project created by QtCreator 2013-06-13T02:51:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = strategy-gui
TEMPLATE = app
CONFIG += link_pkgconfig
PKGCONFIG += gtkmm-2.4\
             protobuf

QMAKE_CXXFLAGS += -std=gnu++0x
SOURCES += main.cpp\
        mainwindow.cpp \
    udpbroadcast.cc \
    socket.cc \
    exception.cc \
    logger.cc \
    addrinfolist.cc \
    publisher.cpp \
    command-packet.pb.cc
INCLUDEPATH += ../kgpkubs/Strategy/Core
HEADERS  += mainwindow.h \
    udpbroadcast.h \
    socket.h \
    noncopyable.h \
    exception.h \
    logger.h \
    addrinfolist.h \
    publisher.h \
    command-packet.pb.h

FORMS    += mainwindow.ui
