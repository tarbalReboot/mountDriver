#-------------------------------------------------
#
# Project created by QtCreator 2019-11-18T22:39:43
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mountdriver
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    encodercount.cpp \
    globalvars.cpp \
    pid.cpp \
    camera.cpp

HEADERS += \
        mainwindow.h \
    encodercount.h \
    globalvars.h \
    pid.h \
    camera.h

FORMS += \
        mainwindow.ui

LIBS += -lsofa_c -lASICamera2 -lopencv_core -lopencv_highgui -lopencv_imgproc -lboost_date_time -lpigpiod_if2 -lrt -g

target.path = /tmp/astroGoto # path on device
INSTALLS += target

