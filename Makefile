ifneq ($(MAKECMDGOALS), release)
    CONFIG := debug
    CXXFLAGS += -g3 -O0 -D_DEBUG
else
    CONFIG := release
    CXXFLAGS += -g0 -O2 -DNDEBUG
endif

JUNK_DIR := obj
CXXFLAGS += -std=c++11 -MMD

MODULES := tinygl harriscd fcg-t1 fcg-t2 fcg-t3

tinygl_PATH := TinyGL
harriscd_PATH := HarrisCD
fcg-t1_PATH := FCG-T1
fcg-t2_PATH := FCG-T2
fcg-t3_PATH := FCG-T3

include $(foreach i,$(MODULES),$($(i)_PATH)/rules.mk)

.DEFAULT_GOAL :=
.PHONY: all
all: $(foreach i,$(MODULES),$($(i)_TARGET))

.PHONY: release
release: all

.PHONY: clean
clean:
	rm -Rf debug release

