/*
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 */

#ifndef	_SYS_TIMEB_H
#define	_SYS_TIMEB_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/feature_tests.h>

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


/*
 * NOTE: struct timeb and ftime live in sys/timeb.h due to standards
 * requirements.
 *
 * The actual function is not a system call, but a libc function.  Hence
 * this header should never be included into kernel modules.  Presumably
 * the header shouldn't be included unless ftime() is desired, since it
 * defines nothing else, but be pedantic.
 */

#ifdef _KERNEL
#error	"sys/timeb.h not supported in the kernel"
#endif

#if !defined(_STRICT_SYMBOLS) || (defined(_XPG4_2) && !defined(_XPG7))

/* Standard says we get time_t, but lets not bring in all sys/types.h */
#if !defined(_TIME_T)
#define	_TIME_T
typedef long		time_t; /* time of day in seconds */
#endif	/* _TIME_T */

struct timeb {
	time_t	time;			/* seconds since the Epoch */
	unsigned short millitm;		/* + milliseconds since the Epoch */
	short	timezone;		/* minutes west of CUT */
	short	dstflag;		/* DST == non-zero */
};

extern int ftime(struct timeb *);
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_TIMEB_H */
