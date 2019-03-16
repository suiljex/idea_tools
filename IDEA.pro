QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = IDEA
INCLUDEPATH += . include

DEFINES += QT_DEPRECATED_WARNINGS

# Input
HEADERS +=  include/idea.hpp \
            include/defines.hpp \
            include/crypto.hpp \
            include/mainwindow.h

SOURCES +=  src/idea.cpp \
            src/main.cpp \
            src/mainwindow.cpp
            

FORMS +=   forms/mainwindow.ui
