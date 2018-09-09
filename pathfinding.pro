QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = pathfinding
TEMPLATE = app
SUBDIRS += ui
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
INCLUDEPATH += src

FORMS += \
    ui/mainwindow.ui \
    ui/helpdialog.ui \
    ui/newmapdialog.ui

HEADERS += \
    src/Graph.h \
    src/TilemapView.h \
    src/MainWindow.h \
    src/Algorithms.hpp \
    src/TilemapScene.h \
    src/HelpDialog.h \
    src/NewMapDialog.h \
    src/Utils.h


SOURCES += \
    src/main.cpp \
    src/Graph.cpp \
    src/TilemapView.cpp \
    src/MainWindow.cpp \
    src/TilemapScene.cpp \
    src/HelpDialog.cpp \
    src/NewMapDialog.cpp \
    src/utils.cpp

RESOURCES += \
    resources.qrc

