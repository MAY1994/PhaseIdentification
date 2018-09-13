#-------------------------------------------------
#
# Project created by QtCreator 2018-04-10T11:12:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UartAssistant
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp  \
    Forms/comdialog.cpp \
    Forms/aboutdialog.cpp \
    User/combase.cpp \
    User/algorithm.cpp \
    User/database.cpp

HEADERS  += mainwindow.h  \
    Forms/aboutdialog.h \
    Forms/comdialog.h \
    User/combase.h \
    User/algorithm.h \
    User/database.h

FORMS    += \
    MainWindow.ui \
    Forms/aboutdialog.ui \
    Forms/comdialog.ui

RESOURCES += \
    resource.qrc

RC_FILE += PI.rc

QT += serialport
