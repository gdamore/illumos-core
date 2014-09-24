/*
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 */

/*
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 */

#include "lint.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#define	FDFILE	"/dev/fd/%u"

/*
 * fexecve.c - implements the fexecve function.
 *
 * We implement in terms of the execve() system call, but use the path
 * /dev/fd/<fd>.  This depends on special handling in the execve system
 * call; note that /dev/fd files are not normally directly executable.
 * This does not actually use the fd filesystem mounted on /dev/fd.
 */
int
fexecve(int fd, char *const argv[], char *const envp[])
{
	char path[32];	/* 8 for "/dev/fd/", 10 for %u, + \0 == 19 */

	if (fcntl(fd, F_GETFL) < 0) {
		/* This will have the effect of returning EBADF */
		return (-1);
	}
	assert(snprintf(NULL, 0, FDFILE, fd) < (sizeof (path) - 1));
	(void) snprintf(path, sizeof (path), FDFILE, fd);
	return (execve(path, argv, envp));
}
