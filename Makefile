ifneq ($(MAKECMDGOALS), release)
    CONFIG := debug
    CXXFLAGS += -g3 -O0 -D_DEBUG
else
    CONFIG := release
    CXXFLAGS += -g0 -O2 -DNDEBUG
endif

JUNK_DIR := bin/$(CONFIG)
CXXFLAGS += -std=c++0x -MMD -Iinclude


MODULES := tinygl harriscd fcg-t1 fcg-t2 fcg-t3 #inf2610-t1 inf2610-t2 inf2610-t3 inf2610-t4

tinygl_PATH := TinyGL
harriscd_PATH := HarrisCD
fcg-t1_PATH := FCG-T1
fcg-t2_PATH := FCG-T2
fcg-t3_PATH := FCG-T3
inf2610-t1_PATH := INF2610-T1
inf2610-t2_PATH := INF2610-T2
inf2610-t3_PATH := INF2610-T3
inf2610-t4_PATH := INF2610-T4

include $(foreach i,$(MODULES),$($(i)_PATH)/rules.mk)

.DEFAULT_GOAL :=
.PHONY : all
all : $(foreach i,$(MODULES),$($(i)_TARGET))

.PHONY : release
release : all

.PHONY : clean
clean :
	rm -rf bin
