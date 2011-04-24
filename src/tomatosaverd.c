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

#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "daemon.h"
#include "log.h"
#include "paths.h"
#include "socket.h"
#include "timer.h"
#include "tomatosaver.h"
#include "window.h"

#ifndef MAX
#define MAX(left, right)	(((left) >= (right)) ? (left) : (right))
#endif

int	 main_loop(struct tomatosaver *);
char	*working_dir(void);

int
main(int argc, char **argv)
{
	struct tomatosaver	*tomatosaver;
	time_t			 started_at;
	int			 control_socket, result;
	char			*cwd;

	started_at = time(NULL);

	init_log();

	cwd = working_dir();
	switch (daemonize(cwd)) {
	case -1:
		log_error("Unable to daemonize: %s\n", strerror(errno));
		goto error_out;
	case 0:
		/* In the child process. */
		break;
	default:
		/* In the parent process. */
		close_log();
		free(cwd);
		exit(EXIT_SUCCESS);
	}

	if (!access(TOMATOSAVER_CONTROL_SOCKET, F_OK) &&
	    (unlink(TOMATOSAVER_CONTROL_SOCKET) == -1)) {
		log_error("Unable to remove previous control socket: %s\n",
		    strerror(errno));
		goto error_out;
	}

	tomatosaver = (struct tomatosaver *)malloc(sizeof(struct tomatosaver));
	if (tomatosaver == NULL) {
		log_error("Unable to allocate enough memory to hold "
		    "information about this instance of tomatosaver: %s\n",
		    strerror(errno));
		goto error_out;
	}
	memset(tomatosaver, 0, sizeof(struct tomatosaver));
	tomatosaver->process_id = getpid();
	tomatosaver->started_at = started_at;
	tomatosaver->current_state = STATE_NOT_RUNNING;

	control_socket = init_control_socket(TOMATOSAVER_CONTROL_SOCKET, bind);
	if (control_socket == -1) {
		goto free_error_out;
	}
	tomatosaver->control_socket = control_socket;

	result = main_loop(tomatosaver);

	if (unlink(TOMATOSAVER_CONTROL_SOCKET) == -1) {
		log_error("Unable to remove control socket: %s\n",
		    strerror(errno));
		goto close_error_out;
	}

	if (close(tomatosaver->control_socket) == -1) {
		log_error("Unable to close control socket: %s\n",
		    strerror(errno));
		goto free_error_out;
	}

	close_log();
	free(cwd);
	free(tomatosaver);
	exit(result);

close_error_out:
	close(tomatosaver->control_socket);
free_error_out:
	free(tomatosaver);
error_out:
	free(cwd);
	close_log();
	exit(EXIT_FAILURE);
}

int
main_loop(struct tomatosaver *tomatosaver)
{
	struct tomatosaver_window	*window;
	XEvent				 event;
	struct timeval			 timeout;
	fd_set				 fds;
	enum message_type		 msg_type;
	int				 connection, display_fd, highest_fd;

	if (listen(tomatosaver->control_socket, 8) == -1) {
		log_error("Unable to listen on control socket: %s\n",
		    strerror(errno));
		return EXIT_FAILURE;
	}

	window = init_window();

	display_fd = XConnectionNumber(window->display);

	tomatosaver->current_state = STATE_SHORT_BREAK;
	tomatosaver->next_transition = five_minutes_from_now();

	highest_fd = 0;
	for (;;) {
		while (XPending(window->display)) {
			XNextEvent(window->display, &event);
			switch (event.type) {
			case Expose:
			case MapNotify:
				display_message(window,
				    "Get up, walk around, give your eyes a break, and come back in 5 minutes");
				break;
			case UnmapNotify:
				break;
			case ButtonPress:
				tomatosaver->current_state = STATE_POMODORI;
				tomatosaver->next_transition = twenty_five_minutes_from_now();
				hide_window(window);
				break;
			}
		}

		FD_ZERO(&fds);
		FD_SET(tomatosaver->control_socket, &fds);
		FD_SET(display_fd, &fds);
		highest_fd = MAX(highest_fd, tomatosaver->control_socket);
		highest_fd = MAX(highest_fd, display_fd);

		timeout.tv_sec = seconds_until(tomatosaver->next_transition);
		timeout.tv_usec = 0;

		switch(select(highest_fd + 1, &fds, NULL, NULL, &timeout)) {
		case -1:
			log_error("Unable to wait for data: %s\n",
			    strerror(errno));
			close_window(window);
			return EXIT_FAILURE;
		case 0:
			/* Timed out waiting for events. */
			if (tomatosaver->current_state == STATE_POMODORI) {
				tomatosaver->current_state = STATE_SHORT_BREAK;
				tomatosaver->next_transition = five_minutes_from_now();
				display_window(window);
			} else {
				tomatosaver->current_state = STATE_POMODORI;
				tomatosaver->next_transition = twenty_five_minutes_from_now();
				hide_window(window);
			}
			continue;
		default:
			if (FD_ISSET(tomatosaver->control_socket, &fds)) {
				connection = accept(tomatosaver->control_socket,
				    NULL, NULL);
				if (connection == -1) {
					log_error("Unable to accept new "
					    "connection: %s\n",
					    strerror(errno));
					close_window(window);
					return EXIT_FAILURE;
				}

				msg_type = MSG_CTL_NONE;
				if (read(connection, &msg_type,
				    sizeof(msg_type))) {
					switch (msg_type) {
					case MSG_CTL_NONE:
						break;
					case MSG_CTL_STATUS:
						write(connection, tomatosaver,
						    sizeof(struct tomatosaver));
						break;
					case MSG_CTL_QUIT:
						log_info("Shutting down\n");
						tomatosaver->current_state =
						    STATE_NOT_RUNNING;
						close_window(window);
						close(connection);
						return EXIT_SUCCESS;
					}
				}
				close(connection);
			}
		}
	}

	close_window(window);
	return EXIT_SUCCESS;
}

char *
working_dir(void)
{
	struct passwd	*passwd_entry;
	size_t		 home_dir_length, suffix_length, working_dir_length;
	uid_t		 user_id;
	char		*working_dir;

	user_id = getuid();
	passwd_entry = getpwuid(user_id);

	home_dir_length = strlen(passwd_entry->pw_dir);
	suffix_length = strlen(TOMATOSAVER_WORKING_DIR_SUFFIX);
	working_dir_length = home_dir_length + suffix_length;

	working_dir = (char *)malloc(working_dir_length + 1);
	if (working_dir == NULL) {
		log_error("Unable to allocate enough memory to hold "
		    "path to working directory: %s\n", strerror(errno));
		close_log();
		exit(EXIT_FAILURE);
	}

	memset(working_dir, 0, working_dir_length + 1);
	strcpy(working_dir, passwd_entry->pw_dir);
	strcat(working_dir, TOMATOSAVER_WORKING_DIR_SUFFIX);

	return working_dir;
}
