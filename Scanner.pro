#-------------------------------------------------
#
# Project created by QtCreator 2015-09-21T23:45:39
#
#-------------------------------------------------

QT       += core gui
QMAKE_MAC_SDK = macosx10.11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Scanner
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

win32 {
    INCLUDEPATH += C:\Users\kir_g\Downloads\opencv\build\include
    LIBS += -L"C:\Development\OpenCV\bin"
    LIBS += -lopencv_core300 -lopencv_imgproc300 -lopencv_imgcodecs300
}

unix {
    QMAKE_CXXFLAGS += -stdlib=libc++
    LIBS += -stdlib=libc++
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib
    LIBS += -lopencv_core -lopencv_imgproc -lopencv_imgcodecs
}


SOURCES += main.cpp\
        mainwindow.cpp \
    GraphSolver.cpp \
    imagelabel.cpp

HEADERS  += mainwindow.h \
    GraphSolver.h \
    imagelabel.h

FORMS    += mainwindow.ui
