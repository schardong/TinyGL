TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = CameraCalib

CONFIG += c++11

HEADERS += fcgt3config.h \
    calibration.h

SOURCES += main.cpp \ 
    calibration.cpp

OTHER_FILES += \
    ../Resources/shaders/fcgt2.vs \
    ../Resources/shaders/fcgt2.fs

shader.path = $$OUT_PWD/../Resources
shader.files = $$OTHER_FILES

INSTALLS += shader

win32 {
    CONFIG(release, debug|release) {
        DESTDIR = $$PWD/../build/release/FCG-T3
        OBJECTS_DIR = $$PWD/../build/release/INF2610-T4/obj

        LIBS += -L$$PWD/../../build/x86/freeglut/lib/Release/ -lfreeglut
        LIBS += -L$$PWD/../../glew/lib/ -lglew32
        LIBS += -L$$PWD/../build/release/TinyGL -ltinygl
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_core249
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_imgproc249
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_features2d249
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_calib3d249
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_flann249
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_highgui249

        PRE_TARGETDEPS += $$PWD/../../build/x86/freeglut/lib/Release/freeglut.lib
        PRE_TARGETDEPS += $$PWD/../../glew/lib/glew32.lib
        PRE_TARGETDEPS += $$PWD/../build/release/TinyGL/tinygl.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_core249.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_imgproc249.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_features2d249.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_calib3d249.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_flann249.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_highgui249.lib
    }

    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/../build/debug/FCG-T3
        OBJECTS_DIR = $$PWD/../build/debug/FCG-T3/obj

        LIBS += -L$$PWD/../../build/x86/freeglut/lib/Debug/ -lfreeglutd
        LIBS += -L$$PWD/../../glew/lib/ -lglew32d
        LIBS += -L$$PWD/../build/debug/TinyGL -ltinygld
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_core249d
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_imgproc249d
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_features2d249d
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_calib3d249d
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_flann249d
        LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_highgui249d

        PRE_TARGETDEPS += $$PWD/../../build/x86/freeglut/lib/Debug/freeglutd.lib
        PRE_TARGETDEPS += $$PWD/../../glew/lib/glew32d.lib
        PRE_TARGETDEPS += $$PWD/../build/debug/TinyGL/tinygld.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_core249d.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_imgproc249d.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_features2d249d.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_calib3d249d.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_flann249d.lib
        PRE_TARGETDEPS += $$PWD/../../opencv/build/x86/vc11/lib/opencv_highgui249d.lib
    }
    message($$LIBS)
}

INCLUDEPATH += $$PWD/../../opencv/build/include
DEPENDPATH += $$PWD/../../opencv/build/include

INCLUDEPATH += $$PWD/../../freeglut/include
DEPENDPATH += $$PWD/../../freeglut/include

INCLUDEPATH += $$PWD/../../glew/include
DEPENDPATH += $$PWD/../../glew/include

INCLUDEPATH += $$PWD/../../glm
DEPENDPATH += $$PWD/../../glm

INCLUDEPATH += ../include
DEPENDPATH += ../include
