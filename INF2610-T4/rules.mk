MODULE := inf2610-t4

inf2610-t4_TARGET := t4_ssao
inf2610-t4_CXXFLAGS := -ITinyGL/src -IHarrisCD/src
inf2610-t4_LIBS := -lglut -lGLEW -lGL
inf2610-t4_LOCALLIBS := $(tinygl_TARGET) $(harriscd_TARGET)

include common-rules.mk
