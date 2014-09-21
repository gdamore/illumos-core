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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#define	FDFILE	"/proc/%u/fd/%u"

/*
 * fexecve.c - implements the fexecve function.
 *
 * We implement in terms of the execve() system call, but use the file
 * descriptor file located in /proc/<pid>/fd/<fd>.  This depends on
 * procfs and the exece system call support for exec'ing such files.
 */
int
fexecve(int fd, char *const argv[], char *const envp[])
{
	char path[32];	/* 10 for "/proc//fd/", 10*2 for %u, + \0 == 31 */

	if (fcntl(fd, F_GETFL) < 0) {
		/* This will have the effect of returning EBADF */
		return (-1);
	}
	assert(snprintf(NULL, 0, FDFILE, getpid(), fd) < (sizeof (path) - 1));
	(void) snprintf(path, sizeof (path), FDFILE, getpid(), fd);
	return (execve(path, argv, envp));
}
