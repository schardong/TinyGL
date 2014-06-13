MODULE := fcg-t1

fcg-t1_TARGET := t1_color-gamut
fcg-t1_CXXFLAGS := 
fcg-t1_LIBS := -lglut -lGLEW -lGL
fcg-t1_LOCALLIBS := $(tinygl_TARGET)

include common-rules.mk
