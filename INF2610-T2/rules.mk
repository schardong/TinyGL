MODULE := inf2610-t2

inf2610-t2_TARGET := t2_shaded_spheres
inf2610-t2_CXXFLAGS := -ITinyGL/src
inf2610-t2_LIBS := -lglut -lGLEW -lGL
inf2610-t2_LOCALLIBS := $(tinygl_TARGET)

include common-rules.mk
