/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 * Copyright (c) 2010, Oracle and/or its affiliates. All rights reserved.
 */

#include "lint.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>

/*
 * A bit of history.  The link() function used to behave differently
 * for XPGv4.  Specifically, it was modified so that under XPG4v2, it
 * would follow symbolic links (which is also the BSD and SunOS 4 behavior),
 * on the basis that the standard (XPG4v2) required it.
 *
 * However, after reading standards going back to SUSv2, I can find no such
 * language in either the old standard or the new.  In fact XPG7 clears this
 * up by specifically stating that whether link() follows symbolic links is
 * implementation defined.
 *
 * Linux seems to have the same legacy behavior by default, and glibc
 * documents the same historical thinking.
 *
 * Nonetheless, the behavior of following symbolic links is probably what
 * the user expects.
 *
 * This is an area where the BSDs and Linux differ.  The caller should
 * use linkat() if they need precise control.
 *
 * The ln(1) program also offers more precise control with -L and -P
 * arguments.
 */
int
linkat(int fd1, const char *path1, int fd2, const char *path2, int flag)
{
	return (syscall(SYS_linkat, fd1, path1, fd2, path2, flag));
}

#pragma	weak _link = link
int
link(const char *path1, const char *path2)
{
	return (linkat(AT_FDCWD, path1, AT_FDCWD, path2, AT_SYMLINK_FOLLOW));
}
