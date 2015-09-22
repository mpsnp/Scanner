#-------------------------------------------------
#
# Project created by QtCreator 2015-09-21T23:45:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Scanner
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += C:\Users\kir_g\Downloads\opencv\build\include
LIBS += -L"C:\Development\OpenCV\bin"
LIBS += -lopencv_core300 -lopencv_imgproc300 -lopencv_imgcodecs300

SOURCES += main.cpp\
        mainwindow.cpp \
    GraphSolver.cpp \
    imagelabel.cpp

HEADERS  += mainwindow.h \
    GraphSolver.h \
    imagelabel.h

FORMS    += mainwindow.ui
