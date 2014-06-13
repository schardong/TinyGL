MODULE := fcg-t2

fcg-t2_TARGET := t2_corner-detector
fcg-t2_CXXFLAGS := -ITinyGL/src -IHarrisCD/src
fcg-t2_LIBS := -lglut -lGLEW -lGL
fcg-t2_LOCALLIBS := $(tinygl_TARGET) $(harriscd_TARGET)

include common-rules.mk
