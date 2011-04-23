# Copyright (c) 2011 Kent R. Spillner <kspillner@acm.org>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

PROJECT_ROOT ?= $(dir $(lastword $(MAKEFILE_LIST)))/..

include $(PROJECT_ROOT)/common.mk

PROGS = tomatosaver_ctl tomatosaverd

all compile: ${PROGS:%.c=%}

clean:
	-rm -f *.o tomatosaver_ctl tomatosaverd

daemon.o: daemon.h log.h

log.o: log.h

socket.o: log.h socket.h

tomatosaver_ctl: log.h paths.h socket.o tomatosaver.h

tomatosaverd: daemon.o log.o paths.h socket.o tomatosaver.h

.PHONY: all clean compile ${SUBDIRS}

.EXPORT_ALL_VARIABLES:
