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

#include "timer.h"

#ifndef MAX
#define MAX(left, right)	(((left) >= (right)) ? (left) : (right))
#endif

time_t	 minutes_from_now(int);

time_t
minutes_from_now(int minutes)
{
	return time(NULL) + (time_t)(minutes * 60);
}

time_t
five_minutes_from_now(void)
{
	return minutes_from_now(5);
}

time_t
twenty_minutes_from_now(void)
{
	return minutes_from_now(20);
}

time_t
twenty_five_minutes_from_now(void)
{
	return minutes_from_now(25);
}

time_t
seconds_until(time_t instant)
{
	return MAX((time_t)0, instant - time(NULL));
}
