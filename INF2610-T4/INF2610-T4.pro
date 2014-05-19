TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = ssao

QMAKE_CXXFLAGS += -std=c++11 -MMD

HEADERS += \
    quad.h

SOURCES += main.cpp \
    quad.cpp

OTHER_FILES += \
    ../Resources/def_fpass.vs \
    ../Resources/def_fpass.fs \
    ../Resources/ssao.vs \
    ../Resources/ssao.fs \

shader.path = $$OUT_PWD/../Resources
shader.files = $$OTHER_FILES

INSTALLS += shader

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -g0 -O2
    LIBS += -L$$PWD/../build/x86/freeglut/lib/Release/ -lfreeglut
    LIBS += -L$$PWD/../glew/lib/ -lglew32
    LIBS += -L$$OUT_PWD/../TinyGL -ltinygl
}

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -g3 -pg -O0
    LIBS += -L$$PWD/../build/x86/freeglut/lib/Debug/ -lfreeglutd
    LIBS += -L$$PWD/../glew/lib/ -lglew32d
    LIBS += -L$$OUT_PWD/../TinyGL -ltinygld
}

INCLUDEPATH += $$PWD/../../freeglut/include
DEPENDPATH += $$PWD/../../freeglut/include

INCLUDEPATH += $$PWD/../../glew/include
DEPENDPATH += $$PWD/../../glew/include

INCLUDEPATH += $$PWD/../../glm
DEPENDPATH += $$PWD/../../glm

INCLUDEPATH += ../include
DEPENDPATH += ../include
