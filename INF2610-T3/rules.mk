MODULE := inf2610-t3

fcg-t1_TARGET := t3_defered_shader
fcg-t1_CXXFLAGS := -ITinyGL/src
fcg-t1_LIBS := -lglut -lGLEW -lGL
fcg-t1_LOCALLIBS := $(tinygl_TARGET)

include common-rules.mk
