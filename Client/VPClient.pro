QT += core
QT -= gui
QT += network

CONFIG += c++11

TARGET = VPClient
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    client.cpp \
    ../ssl-playbook.pb.cc

HEADERS += \
    client.h \
    ../ssl-playbook.pb.h


LIBS += -L/usr/local/lib \
     -lprotobuf \
     -lprotobuf-lite
