QT -= opengl core gui

QMAKE_CXXFLAGS += -std=c++11 -MMD

TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    shader.cpp \
    mesh.cpp \
    logger.cpp \
    bufferobject.cpp \
    tinygl.cpp \
    sphere.cpp \
    grid.cpp

HEADERS += \
    singleton.h \
    shader.h \
    mesh.h \
    logger.h \
    config.h \
    bufferobject.h \
    tinygl.h \
    sphere.h \
    grid.h

INCLUDEPATH += ../include

header.path = ../include
header.files = $$HEADERS

INSTALLS += header

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -g0 -O2
    TARGET = tinygld
}

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -g3 -pg -O0
    TARGET = tinygl
}
