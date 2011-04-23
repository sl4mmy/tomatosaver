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

#include <sys/un.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "socket.h"

int
init_control_socket(char *path,
    int (*initialize)(int, const struct sockaddr *, socklen_t))
{
	struct sockaddr_un	 socket_address;
	int			 control_socket;

	control_socket = socket(PF_UNIX, SOCK_STREAM, 0);
	if (control_socket == -1) {
		log_error("Unable to create control socket: %s\n",
		    strerror(errno));
		goto error_out;
	}

	memset(&socket_address, 0, sizeof(socket_address));
	socket_address.sun_family = AF_UNIX;
	strncpy(socket_address.sun_path, path,
	    sizeof(socket_address.sun_path) - 1);

	if (initialize(control_socket, (struct sockaddr *)&socket_address,
	    sizeof(socket_address)) == -1) {
		log_error("Unable to initialize control socket: %s\n",
		    strerror(errno));
		goto close_error_out;
	}

	return control_socket;

close_error_out:
	close(control_socket);
error_out:
	return -1;
}
