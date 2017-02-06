#-------------------------------------------------
#
# Project created by QtCreator 2015-02-27T12:34:16
#
#-------------------------------------------------

QT       += core gui\
            sql
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Visual_Planner
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    playoff.cpp \
    playon.cpp \
    tags.cpp \
    agentplanclass.cpp \
    playoffplansql.cpp \
    playonplansql.cpp \
    ssl-playbook.pb.cc \
    statuswidget.cpp \
    logger.cpp \
    client.cpp \
    server.cpp

HEADERS  += mainwindow.h \
    base.h \
    playoff.h \
    playon.h \
    tags.h \
    include/ul.h \
    agentplanclass.h \
    playoffplansql.h \
    playonplansql.h \
    ssl-playbook.pb.h \
    statuswidget.h \
    logger.h \
    client.h \
    server.h


INCLUDEPATH += $$PWD/include

INCLUDEPATH += /usr/local/include

FORMS    += mainwindow.ui \
            tags.ui

LIBS += -L/usr/local/lib \
     -lprotobuf \
     -lprotobuf-lite


MOC_DIR     = $$PWD/moc_objs
OBJECTS_DIR = $$PWD/objs
UI_DIR      = $$PWD/ui_objs

CONFIG += c++14

RESOURCES += \
    resource.qrc
