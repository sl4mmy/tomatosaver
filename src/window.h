/*
 * Copyright (c) 2011 Kent R. Spillner <kspillner@acm.org>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <X11/Xlib.h>

struct tomatosaver_window {
	Display				*display;
	GC				 graphics_context;
	Window				 root_window, xwindow;
	XFontStruct			*font;
	int				 height, width;
};

void				 close_window(struct tomatosaver_window *);
void				 display_message(struct tomatosaver_window *,
				    const char *);
void				 display_window(struct tomatosaver_window *);
void				 hide_window(struct tomatosaver_window *);
struct tomatosaver_window	*init_window(void);

#endif /* !defined(_WINDOW_H_) */
