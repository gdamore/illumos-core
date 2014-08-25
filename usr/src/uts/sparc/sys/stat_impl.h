/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
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
 *
 * Copyright 2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_STAT_IMPL_H
#define	_SYS_STAT_IMPL_H

#include <sys/feature_tests.h>
#include <sys/types.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * The implementation specific header for <sys/stat.h>
 */

#if !defined(_KERNEL) || defined(_BOOT)

extern int fstat(int, struct stat *);
extern int stat(const char *_RESTRICT_KYWD, struct stat *_RESTRICT_KYWD);
#if !defined(_STRICT_SYMBOLS) || defined(_XPG7) || defined(_ATFILE_SOURCE)
extern int fstatat(int, const char *, struct stat *, int);
#endif

#if !defined(_STRICT_SYMBOLS) || defined(_XPG4_2)
extern int lstat(const char *_RESTRICT_KYWD, struct stat *_RESTRICT_KYWD);
extern int mknod(const char *, mode_t, dev_t);
#endif

#endif /* !defined(_KERNEL) || defined(_BOOT) */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_STAT_IMPL_H */
