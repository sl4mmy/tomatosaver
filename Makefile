PROJECT_ROOT ?= ${CURDIR}

include ${PROJECT_ROOT}/common.mk

SUBDIRS = src

all clean compile: ${SUBDIRS}

bootstrap:
	mkdir ${HOME}/.tomatosaver
	chmod go-rwx ${HOME}/.tomatosaver

${SUBDIRS}: ${HOME}/.tomatosaver/
	-@echo "===> ${CURDIR}/$@"
	${MAKE} -C $@ ${MAKECMDGOALS}

${HOME}/.tomatosaver/:
	$(error Run 'make bootstrap' first)

.PHONY: all bootstrap clean compile ${SUBDIRS}

.EXPORT_ALL_VARIABLES:
