TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -MMD

HEADERS += \
    sphere.h \
    grid.h

SOURCES += main.cpp \
    sphere.cpp \
    grid.cpp

OTHER_FILES += \
    simple.gs \
    simple.fs \
    simple.vs \
    ../Resources/simple.vs \
    ../Resources/simple.gs \
    ../Resources/simple.fs

INCLUDEPATH += ../include
DEPENDPATH += ../include

LIBS += -L$$OUT_PWD/../TinyGL -ltinygl
LIBS += -lglut -lGLEW -lGL

shader.path = $$OUT_PWD
shader.files = $$OTHER_FILES

INSTALLS += shader

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -g0 -O2
}

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -g3 -pg -O0
}
