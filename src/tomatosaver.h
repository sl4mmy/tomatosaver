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

#ifndef _TOMATOSAVER_H_
#define _TOMATOSAVER_H_

#include <time.h>
#include <unistd.h>

enum message_type {
	MSG_CTL_NONE,
	MSG_CTL_STATUS,
	MSG_CTL_QUIT
};

enum tomatosaver_state {
	STATE_NOT_RUNNING,
	STATE_POMODORI,
	STATE_SHORT_BREAK,
	STATE_LONG_BREAK,
	STATE_WAITING,
};

struct tomatosaver {
	pid_t			 process_id;
	time_t			 started_at;
	time_t			 next_transition;
	enum tomatosaver_state	 current_state;
	int			 control_socket;
};

#endif /* !defined(_TOMATOSAVER_H_) */
