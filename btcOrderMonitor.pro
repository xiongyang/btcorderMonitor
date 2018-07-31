#-------------------------------------------------
#
# Project created by QtCreator 2018-01-17T16:06:12
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = btcOrderMonitor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
INCLUDEPATH +="E:\boost_1_65_1"
LIBS += -LE:\boost_1_65_1\lib64-msvc-14.1

INCLUDEPATH +="D:\boost_1_61_0"
LIBS += -LD:\boost_1_61_0\stage\2017_x64_lib

INCLUDEPATH +="C:\work\lib\boost_1_66_0"
LIBS += -LC:\work\lib\boost_1_66_0/stage/lib
}
unix {
INCLUDEPATH +=/root/workspace/boost_1_66_0/
LIBS += -L/root/workspace/boost_1_66_0/stage/lib -lboost_log -lboost_filesystem -lboost_system -lboost_thread -lboost_log_setup -lboost_regex -lboost_date_time  -lboost_atomic -lboost_chrono -lboost_iostreams
DEFINES += BOOST_LOG_DYN_LINK linux
LIBS += -lrt
}


SOURCES += \
        main.cpp \
        monitordialog.cpp \
    querydialog.cpp \
    logger.cpp \
    bitmexquerydialog.cpp \
    util.cpp

HEADERS += \
        monitordialog.h \
    querydialog.h \
    logger.h \
    bitmexquerydialog.h \
    util.h
