/*
 * Copyright (c) 2008-2015 Solarflare Communications Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the FreeBSD Project.
 */

#ifndef _SFXGE_COMPAT_H
#define _SFXGE_COMPAT_H

/*
 * Macro available in newer versions of /usr/include/sdt.h
 */
#include <sys/sdt.h>
#include <sys/inline.h>

#ifndef	DTRACE_PROBE5
#define	DTRACE_PROBE5(name, type1, arg1, type2, arg2, type3, arg3,	\
	type4, arg4, type5, arg5)					\
	{								\
		extern void __dtrace_probe_##name(uintptr_t, uintptr_t,	\
		    uintptr_t, uintptr_t, uintptr_t);			\
		__dtrace_probe_##name((uintptr_t)(arg1),		\
		    (uintptr_t)(arg2), (uintptr_t)(arg3),		\
		    (uintptr_t)(arg4), (uintptr_t)(arg5));		\
	}
#endif	/* DTRACE_PROBE5 */

#if 0
#if defined(__i386) || defined(__amd64)
/* declaring as extern causes SunStudio link error "invalid section index"*/
/*LINTED*/
static inline void prefetch_read_many(void *addr)
{
	__asm__(
	    "prefetcht0 (%0)"
	    :
	    : "r" (addr));
}
/* declaring as extern causes SunStudio link error "invalid section index"*/
/*LINTED*/
static inline void prefetch_read_once(void *addr)
{
	__asm__(
	    "prefetchnta (%0)"
	    :
	    : "r" (addr));
}
#endif


#if defined(__sparcv9)
/* declaring as extern causes SunStudio link error "invalid section index"*/
/*LINTED*/
static inline void prefetch_read_many(void *addr)
{
	__asm__ __volatile__(
	    "prefetch	[%0],#n_reads\n\t"
	: "=r" (addr)
	: "0" (addr));
}

/* declaring as extern causes SunStudio link error "invalid section index"*/
/*LINTED*/
static inline void prefetch_read_once(void *addr)
{
	__asm__ __volatile__(
	    "prefetch	[%0],#one_read\n\t"
	: "=r" (addr)
	: "0" (addr));
}
#endif
#endif

#endif
