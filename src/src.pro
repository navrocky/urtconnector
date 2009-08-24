# -------------------------------------------------
# Project created by QtCreator 2009-07-11T13:11:32
# -------------------------------------------------
TARGET = ../bin/urtconnector
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
 serverid.cpp \
 options.cpp \
 appoptions.cpp \
 optionsdialog.cpp \
 launcher.cpp \
 exception.cpp \
 application.cpp \
 serveroptions.cpp \
 servoptsdialog.cpp \
 pushbuttonactionlink.cpp \
 serverlistcustom.cpp \
 serverinfo.cpp \
 playerinfo.cpp \
 serverlistqstat.cpp
HEADERS += mainwindow.h \
 serverid.h \
 options.h \
 appoptions.h \
 optionsdialog.h \
 launcher.h \
 exception.h \
 application.h \
 serveroptions.h \
 servoptsdialog.h \
 pushbuttonactionlink.h \
 serverlistcustom.h \
 serverinfo.h \
 playerinfo.h \
 serverlistqstat.h
FORMS += mainwindow.ui \
 optionsdialog.ui \
 servoptsdialog.ui
CONFIG -= release

CONFIG += debug

RESOURCES += images.qrc

bin.path = /usr/bin

INSTALLS += target

target.path = /usr/bin

