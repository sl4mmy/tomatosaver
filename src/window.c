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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "window.h"

void
close_window(struct tomatosaver_window *window)
{
	XCloseDisplay(window->display);

	free(window->message);
	free(window);
}

void
display_message(struct tomatosaver_window *window)
{
	XDrawString(window->display, window->xwindow, window->graphics_context,
	    window->message->x, window->message->y, window->message->contents,
	    strlen(window->message->contents));

	XFlush(window->display);
}

void
display_window(struct tomatosaver_window *window)
{
	XMapWindow(window->display, window->xwindow);
}

void
hide_window(struct tomatosaver_window *window)
{
	XUnmapWindow(window->display, window->xwindow);
}

struct tomatosaver_window *
init_window(void)
{
	struct tomatosaver_window	*window;
	struct tomatosaver_message	*message;
	Display				*display;
	GC				 graphics_context;
	Screen				*screen;
	Window				 root_window, xwindow;
	XEvent				 event;
	XFontStruct			*font;
	XGCValues			 values;
	int				 black, message_x, message_y, message_width, screen_index, state, white, window_height, window_width;
	char				*contents;

	window = NULL;
	message = NULL;
	display = NULL;
	screen = NULL;
	font = NULL;
	contents = NULL;

	window = (struct tomatosaver_window *)malloc(sizeof(struct tomatosaver_window));
	if (window == NULL) {
		log_error("Unable to allocate enough memory to hold "
		    "information about the window: %s\n", strerror(errno));
		goto error_out;
	}
	memset(window, 0, sizeof(struct tomatosaver_window));

	message = (struct tomatosaver_message *)malloc(sizeof(struct tomatosaver_message));
	if (message == NULL) {
		log_error("Unable to allocate enough memory to hold "
		    "message contents: %s\n", strerror(errno));
		goto error_out;
	}
	memset(message, 0, sizeof(struct tomatosaver_message));
	window->message = message;

	display = XOpenDisplay(NULL);
	if (display == NULL) {
		log_error("Cannot open display\n");
		goto error_out;
	}
	window->display = display;

	font = XLoadQueryFont(display, "-*-helvetica-*-18-*");
	if (font == NULL) {
		log_error("Cannot load font\n");
		goto error_out;
	}

	root_window = XDefaultRootWindow(display);
	window->root_window = root_window;

	screen = XDefaultScreenOfDisplay(display);
	screen_index = XScreenNumberOfScreen(screen);

	window_height = XDisplayHeight(display, screen_index);
	window_width = XDisplayWidth(display, screen_index);

	black = BlackPixel(display, screen_index);
	white = WhitePixel(display, screen_index);

	xwindow = XCreateSimpleWindow(display, root_window, 0, 0, window_width, window_height, 0, black, black);
	window->xwindow = xwindow;

	values.font = font->fid;
	values.foreground = white;
	values.line_width = 1;
	values.line_style = LineSolid;
	graphics_context = XCreateGC(display, xwindow, GCFont | GCForeground | GCLineStyle | GCLineWidth, &values);
	window->graphics_context = graphics_context;

	contents = "Get up, walk around, give your eyes a break, and come back in 5 minutes";
	message_width = XTextWidth(font, contents, strlen(contents));
	message_x = (window_width - message_width) / 2;
	message_y = (window_height + font->ascent) / 2;
	message->contents = contents;
	message->x = message_x;
	message->y = message_y;

	XSelectInput(display, xwindow, ButtonPressMask | ExposureMask | StructureNotifyMask);

	XMapWindow(display, xwindow);

	return window;

error_out:
	if (window != NULL) {
		free(window);
	}
	if (message != NULL) {
		free(message);
	}
	if (display != NULL) {
		free(display);
	}
	if (screen != NULL) {
		free(screen);
	}
	if (font != NULL) {
		free(font);
	}
	if (contents != NULL) {
		free(contents);
	}
	close_log();
	exit(EXIT_FAILURE);
}
