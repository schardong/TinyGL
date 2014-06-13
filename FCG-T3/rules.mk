MODULE := fcg-t3

fcg-t3_TARGET := t3_camera-calib
fcg-t3_CXXFLAGS :=
fcg-t3_LIBS := -lglut -lGLEW -lGL -lopencv_core -lopencv_highgui -lopencv_calib3d -lopencv_flann -lopencv_imgproc
fcg-t3_LOCALLIBS := $(tinygl_TARGET)

include common-rules.mk
