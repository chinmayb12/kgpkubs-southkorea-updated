#-------------------------------------------------
#
# Project created by QtCreator 2011-07-01T12:58:44
#
#-------------------------------------------------

QT       += core gui

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../proto/netraw.cpp \
    ../proto/grSim_Replacement.pb.cc \
    ../proto/grSim_Commands.pb.cc \
    ../proto/grSim_Packet.pb.cc \
    ../proto/sslDebug_Data.pb.cc

HEADERS  += mainwindow.h
INCLUDEPATH += ../include
LIBS += -L../libs/ -lprotobuf
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -O3
