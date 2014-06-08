TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = alchemy

CONFIG += c++11

HEADERS += \
    image.h

SOURCES += main.cpp \
    image.c

OTHER_FILES += \
    ../Resources/ssao_fpass.vs \
    ../Resources/ssao_fpass.fs \
    ../Resources/def_spass.vs \
    ../Resources/ssao.fs \
    ../Resources/def_spass.vs \
    ../Resources/blur.fs \

shader.path = $$OUT_PWD/../Resources
shader.files = $$OTHER_FILES

INSTALLS += shader

win32 {
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../../build/x86/freeglut/lib/Release/freeglut.lib
        LIBS += -L$$PWD/../glew/lib/ -lglew32
        LIBS += -L$$OUT_PWD/release/ -ltinygl
        PRE_TARGETDEPS += $$PWD/../../build/x86/freeglut/lib/Release/freeglut.lib
        PRE_TARGETDEPS += $$PWD/../../glew/lib/glew32.lib
        PRE_TARGETDEPS += $$OUT_PWD/release/tinygl.lib
    }

    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../build/x86/freeglut/lib/Debug/ -lfreeglutd
        LIBS += -L$$PWD/../../glew/lib/ -lglew32d
        LIBS += -L$$OUT_PWD/debug/ -ltinygld
        PRE_TARGETDEPS += $$PWD/../../build/x86/freeglut/lib/Debug/freeglutd.lib
        PRE_TARGETDEPS += $$PWD/../../glew/lib/glew32d.lib
        PRE_TARGETDEPS += $$OUT_PWD/debug/tinygld.lib
    }
    message($$LIBS)
}

unix {
    QMAKE_CXXFLAGS += -MMD
    CONFIG(release, debug|release) {
        QMAKE_CXXFLAGS += -g0 -O2
        LIBS += -L$$PWD/../../build/x86/freeglut/lib/Release/ -lfreeglut
        LIBS += -L$$PWD/../glew/lib/ -lglew32
        LIBS += -L$$OUT_PWD/../TinyGL -ltinygl
    }

    CONFIG(debug, debug|release) {
        QMAKE_CXXFLAGS += -g3 -pg -O0
        LIBS += -L$$PWD/../../build/x86/freeglut/lib/Debug/ -lfreeglutd
        LIBS += -L$$PWD/../glew/lib/ -lglew32d
        LIBS += -L$$OUT_PWD/../TinyGL -ltinygld
    }
    message($$LIBS)
}

INCLUDEPATH += $$PWD/../..//freeglut/include
DEPENDPATH += $$PWD/../../freeglut/include

INCLUDEPATH += $$PWD/../../glew/include
DEPENDPATH += $$PWD/../../glew/include

INCLUDEPATH += $$PWD/../../glm
DEPENDPATH += $$PWD/../../glm

INCLUDEPATH += ../include
DEPENDPATH += ../include
