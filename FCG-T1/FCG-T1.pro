TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -MMD

HEADERS += color.h

SOURCES += main.cpp \
           color.c

OTHER_FILES += \
    ../Resources/fcgt1.vs \
    ../Resources/fcgt1.fs

INCLUDEPATH += ../include
DEPENDPATH += ../include

LIBS += -L$$OUT_PWD/../TinyGL
LIBS += -lglut -lGLEW -lGL

shader.path = $$OUT_PWD
shader.files = $$OTHER_FILES

INSTALLS += shader

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -g0 -O2
    LIBS += -ltinygl
}

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -g3 -pg -O0
    LIBS += -ltinygld
}
