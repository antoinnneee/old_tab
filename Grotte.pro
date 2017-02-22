#-------------------------------------------------
#
# Project created by QtCreator 2015-08-23T16:47:15
#
#-------------------------------------------------
QT += multimedia
QT       += core gui concurrent
QT       += androidextras bluetooth widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Grotte
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    androidrfcomm.cpp \
    client.cpp \
    server.cpp \
    peermanager.cpp \
    connection.cpp

HEADERS  += mainwindow.h \
    androidrfcomm.h \
    client.h \
    server.h \
    peermanager.h \
    connection.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = 

DISTFILES += \
    Android/AndroidManifest.xml \
    Android/gradle/wrapper/gradle-wrapper.jar \
    Android/gradlew \
    Android/res/values/libs.xml \
    Android/build.gradle \
    Android/gradle/wrapper/gradle-wrapper.properties \
    Android/gradlew.bat


ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android
