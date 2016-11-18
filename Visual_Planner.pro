#-------------------------------------------------
#
# Project created by QtCreator 2015-02-27T12:34:16
#
#-------------------------------------------------

QT       += core gui\
            sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Visual_Planner
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    kkagentplanclass.cpp \
    playoff.cpp \
    playon.cpp \
    tags.cpp \
    kkplayonplansql.cpp \
    kkplayoffplansql.cpp

HEADERS  += mainwindow.h \
    kkagentplanclass.h \
    base.h \
    playoff.h \
    playon.h \
    tags.h \
    kkplayonplansql.h \
    kkplayoffplansql.h \
    include/ul.h


INCLUDEPATH += $$PWD/include

FORMS    += mainwindow.ui \
            tags.ui


MOC_DIR     = $$PWD/moc_objs
OBJECTS_DIR = $$PWD/objs
UI_DIR      = $$PWD/ui_objs

CONFIG += c++14

RESOURCES += \
    resource.qrc
