PROJECT_ROOT ?= $(dir $(lastword $(MAKEFILE_LIST)))/..

include $(PROJECT_ROOT)/common.mk

PROGS = tomatosaver_ctl tomatosaverd

all compile: ${PROGS:%.c=%}

clean:
	-rm -f *.o tomatosaver_ctl tomatosaverd

.PHONY: all clean compile ${SUBDIRS}

.EXPORT_ALL_VARIABLES:
