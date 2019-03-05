#-------------------------------------------------
#
# Project created by QtCreator 2018-12-01T22:28:31
#
#-------------------------------------------------

QT       += core gui
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Rutenerkennung
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    Beobachter.cpp \
    Kommunikation.cpp \
    Steckling.cpp \
    Rutenerkennung.cpp \
    Videoquelle.cpp \
    mainwindow.cpp \
    Clickablelabel.cpp \
    Einmessung_koordinatensystem.cpp \
    Einmessung_maske.cpp

HEADERS  += \
    config_manager.h \
    Beobachter.h \
    Kommunikation.h \
    Steckling.h \
    Rutenerkennung.h \
    Videoquelle.h \
    mainwindow.h \
    Clickablelabel.h \
    Einmessung_koordinatensystem.h \
    Einmessung_maske.h

FORMS    += mainwindow.ui \
    einmessung_koordinatensystem.ui \
    einmessung_maske.ui

LIBS     += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lpthread

DISTFILES += \
    Config.xml
