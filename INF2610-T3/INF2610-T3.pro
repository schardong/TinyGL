TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -MMD

SOURCES += main.cpp

OTHER_FILES += \
    ../Resources/ads_vertex.vs \
    ../Resources/ads_vertex.fs \
    ../Resources/ads_frag.vs \
    ../Resources/ads_frag.fs \

INCLUDEPATH += ../include
DEPENDPATH += ../include

LIBS += -L$$OUT_PWD/../TinyGL
LIBS += -lglut -lGLEW -lGL

shader.path = $$OUT_PWD/../Resources
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
