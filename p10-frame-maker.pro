
CONFIG += debug # default -O0 -g -gdbg

TEMPLATE = app
TARGET = p10-frame-maker.elf

QT = core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
LIBS += -lsfml-graphics 
LIBS += -lsfml-window 
LIBS += -lsfml-system 
LIBS += -lsfml-audio 

OBJECTS_DIR = build/obj
MOC_DIR = build/moc

SOURCES += main.cpp 
SOURCES += qsfml/qsfml.cpp 
SOURCES += p10sprites/p10sprites.cpp
SOURCES += p10canvas/p10canvas.cpp
SOURCES += p10frame/p10frame.cpp
#SOURCES += MainWindow/MainWindow.cpp

HEADERS += qsfml/qsfml.h
HEADERS += p10sprites/p10sprites.hpp
HEADERS += p10canvas/p10canvas.hpp
HEADERS += p10frame/p10frame.hpp
#HEADERS += MainWindow/MainWindow.hpp