TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

OTHER_FILES +=

INCLUDEPATH += ../include
DEPENDPATH += ../include

LIBS += -ltinygl -lfreeglut -lglew
