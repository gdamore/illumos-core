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
 * Copyright 2014 Garrett D'Amore
 */

/*
 * This implements the BSD style flock() function.  This is
 * implemented in terms of fcntl(), and the lock is compatible
 * with both fcntl() and lockf() based locks.
 */

#include "lint.h"
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>

int
flock(int fd, int operation)
{
	struct flock64	fl;
	int		cmd;

	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if (operation & LOCK_NB) {
		cmd = F_SETLK64;
	} else {
		cmd = F_SETLKW64;
	}
	switch (operation) {
	case LOCK_SH|LOCK_NB:
	case LOCK_SH:
		fl.l_type = F_RDLCK;
		break;

	case LOCK_EX|LOCK_NB:
	case LOCK_EX:
		fl.l_type = F_WRLCK;
		break;

	case LOCK_UN|LOCK_NB:
	case LOCK_UN:
		fl.l_type = F_UNLCK;
		break;
	default:
		errno = EINVAL;
		return (-1);
	}

	/*
	 * NB: It is posssible to return more errnos than just
	 * EWOULDBLOCK, EBADF, EINVAL.  For example, ENOLCK.
	 */

	return (fcntl(fd, cmd, &fl));
}
