#-------------------------------------------------
#
# Project created by QtCreator 2018-04-17T12:40:45
#
#-------------------------------------------------

QT += core gui
CONFIG += qt c++17
QMAKE_LFLAGS += -Wl,-rpath,"'$$ORIGIN'"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GYROTRON
TEMPLATE = app
LIBS += -lpthread

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



SOURCES += \
    device.cpp \
    gyrotron.cpp \
    lib/qcustomplot.cpp \
    lib/spine/device/device.cpp \
    lib/spine/logger/logger.cpp \
    lib/spine/spine.cpp \
        main.cpp \
        mainwindow.cpp \
    pid.cpp \
    qcustomplot.cpp

HEADERS += \
    device.h \
    face.h \
    gyrotron.h \
    lib/face/face.h \
    lib/pid.h \
    lib/qcustomplot.h \
    lib/spine/ctpl_stl.h \
    lib/spine/device/device.h \
    lib/spine/logger/logger.h \
    lib/spine/spine.h \
        mainwindow.h \
    pid.h \
    qcustomplot.h

FORMS += \
        lib/face/b12dialog.ui \
        mainwindow.ui

DISTFILES +=

RESOURCES += \
    resources.qrc
