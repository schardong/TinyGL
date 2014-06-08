QT -= opengl core gui

TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

SOURCES += \
    axis.cpp \
    bufferobject.cpp \
    grid.cpp \
    light.cpp \
    logger.cpp \
    mesh.cpp \
    shader.cpp \
    sphere.cpp \
    tinygl.cpp \
    cube.cpp \
    framebufferobject.cpp \
    quad.cpp

HEADERS += \
    axis.h \
    bufferobject.h \
    grid.h \
    light.h \
    logger.h \
    mesh.h \
    shader.h \
    singleton.h \
    sphere.h \
    tglconfig.h \
    tinygl.h \
    cube.h \
    framebufferobject.h \
    quad.h

INCLUDEPATH += ../include

header.path = ../include
header.files = $$HEADERS

INSTALLS += header

win32 {
    CONFIG(release, debug|release) {
        TARGET = tinygl
        LIBS += -L$$PWD/../build/x86/freeglut/lib/Release/ -lfreeglut
        LIBS += -L$$PWD/../glew/lib/ -lglew32
        PRE_TARGETDEPS += $$PWD/../../build/x86/freeglut/lib/Release/freeglut.lib
        PRE_TARGETDEPS += $$PWD/../../glew/lib/glew32.lib
    }

    CONFIG(debug, debug|release) {
        TARGET = tinygld
        LIBS += -L$$PWD/../build/x86/freeglut/lib/Debug/ -lfreeglutd
        LIBS += -L$$PWD/../glew/lib/ -lglew32d
        PRE_TARGETDEPS += $$PWD/../../build/x86/freeglut/lib/Debug/freeglutd.lib
        PRE_TARGETDEPS += $$PWD/../../glew/lib/glew32d.lib
    }
}

unix {
    CONFIG(release, debug|release) {
        QMAKE_CXXFLAGS += -g0 -O2
        TARGET = tinygl
        LIBS += -L$$PWD/../build/x86/freeglut/lib/Release/ -lfreeglut
        LIBS += -L$$PWD/../glew/lib/ -lglew32
    }

    CONFIG(debug, debug|release) {
        QMAKE_CXXFLAGS += -g3 -pg -O0
        TARGET = tinygld
        LIBS += -L$$PWD/../build/x86/freeglut/lib/Debug/ -lfreeglutd
        LIBS += -L$$PWD/../glew/lib/ -lglew32d
    }
}

INCLUDEPATH += $$PWD/../../freeglut/include
DEPENDPATH += $$PWD/../../freeglut/include

INCLUDEPATH += $$PWD/../../glew/include
DEPENDPATH += $$PWD/../../glew/include

INCLUDEPATH += $$PWD/../../glm
DEPENDPATH += $$PWD/../../glm
