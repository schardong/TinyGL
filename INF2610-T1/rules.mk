MODULE := inf2610-t1

fcg-t1_TARGET := t1_basic_spheres
fcg-t1_CXXFLAGS := -ITinyGL/src
fcg-t1_LIBS := -lglut -lGLEW -lGL
fcg-t1_LOCALLIBS := $(tinygl_TARGET)

include common-rules.mk
