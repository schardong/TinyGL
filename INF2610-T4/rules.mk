MODULE := inf2610-t4

fcg-t1_TARGET := t4_ssao
fcg-t1_CXXFLAGS := -ITinyGL/src
fcg-t1_LIBS := -lglut -lGLEW -lGL
fcg-t1_LOCALLIBS := $(tinygl_TARGET)

include common-rules.mk
