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
            include/mainwindow.h \
            include/idea_demo.hpp \
            include/demo.h \
            include/about.h \
            include/help.h

SOURCES +=  src/idea.cpp \
            src/main.cpp \
            src/mainwindow.cpp \
            src/idea_demo.cpp \
            src/demo.cpp \
            src/about.cpp \
            src/help.cpp
            

FORMS +=    forms/mainwindow.ui \
            forms/demo.ui \
            forms/about.ui \
            forms/help.ui
