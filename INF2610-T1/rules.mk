MODULE := inf2610-t1

inf2610-t1_TARGET := t1_basic_spheres
inf2610-t1_CXXFLAGS := -ITinyGL/src
inf2610-t1_LIBS := -lglut -lGLEW -lGL
inf2610-t1_LOCALLIBS := $(tinygl_TARGET)

include common-rules.mk
