MODULE := inf2610-t3

inf2610-t3_TARGET := t3_defered_shader
inf2610-t3_CXXFLAGS := -ITinyGL/src
inf2610-t3_LIBS := -lglut -lGLEW -lGL
inf2610-t3_LOCALLIBS := $(tinygl_TARGET)

include common-rules.mk
