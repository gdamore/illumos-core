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


/*
 * This is the beginning of test cases.  Ints in a separate file for
 * ease of use.  This file will be #include'd into the actual C file.
 *
 * Note that no attempt is made to verify volatile or restrict qualifiers
 * in parameters.  We do validate const keywords along with variable types.
 */

/*
 * Types
 */
{
	"locale_t", SYM_TYPE, "in locale.h",
	{ "locale.h" }, { "locale_t" },
	MASK_ALL, MASK_SINCE_SUSV4,
}, {
	"locale_t", SYM_TYPE, "in wchar.h",
	{ "wchar.h" }, { "locale_t" },
	MASK_ALL, MASK_SINCE_SUSV4,
}, {
	"pid_t", SYM_TYPE, NULL,
	{ "unistd.h" }, { "pid_t" },
	MASK_IX, MASK_IX,
}, {
	"sigset_t", SYM_TYPE, "in ucontext.h",
	{ "ucontext.h" }, { "ucontext_t" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4,
}, {
	"stack_t", SYM_TYPE, "in ucontext.h",
	{ "ucontext.h" }, { "ucontext_t" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4,
}, {
	"time_t", SYM_TYPE, "in sys/timeb.h",
	{ "sys/timeb.h" }, { "time_t" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4,
}, {
	"timeb", SYM_TYPE, "in sys/timeb.h",
	{ "sys/timeb.h" }, { "struct timeb" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4,
}, {
	"ucontext_t", SYM_TYPE, NULL,
	{ "ucontext.h" }, { "ucontext_t" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4,
}, {
	"wctype_t", SYM_TYPE, "in wchar.h (UNIX)",
	{ "wchar.h" }, { "wctype_t" },
	MASK_ALL, MASK_UNIX,
}, {
	"wctype_t", SYM_TYPE, "in wctype.h (ISO C)",
	{ "wctype.h" }, { "wctype_t" },
	MASK_ALL, MASK_SINCE_C89,
},

/*
 * Values.
 */
{
	"NULL", SYM_VALUE, "in wchar.h",
	{ "wchar.h" }, { "void *" },
	MASK_ALL, MASK_SINCE_C89,
}, {
	"LC_GLOBAL_LOCALE", SYM_VALUE, NULL,
	{ "locale.h" }, { "locale_t" },
	MASK_ALL, MASK_SINCE_SUSV4,
}, {
	"h_errno", SYM_VALUE, NULL,
	{ "netdb.h" }, { "int" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4,
}, {
	"HOST_NOT_FOUND", SYM_VALUE, NULL,
	{ "netdb.h" }, { "int" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4,
}, {
	"NO_DATA", SYM_VALUE, NULL,
	{ "netdb.h" }, { "int" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4,
}, {
	"NO_RECOVERY", SYM_VALUE, NULL,
	{ "netdb.h" }, { "int" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4,
}, {
	"TRY_AGAIN", SYM_VALUE, NULL,
	{ "netdb.h" }, { "int" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4,
},

/*
 * Functions
 */
{
	"alphasort", SYM_FUNC, NULL,
	{ "sys/types.h", "dirent.h" },
	{ "int", "const struct dirent **", "const struct dirent **" },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"bcmp", SYM_FUNC, NULL,
	{ "strings.h" },
	{ "int", "const void *", "const void *", "size_t" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"bcopy", SYM_FUNC, NULL,
	{ "strings.h" },
	{ "void", "const void *", "void *", "size_t" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"bsd_signal", SYM_FUNC, NULL,
	{ "signal.h" },
	{ "void (*)(int)", "int", "void (*)(int)" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"bzero", SYM_FUNC, NULL,
	{ "strings.h" },
	{ "void", "void *", "size_t" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"ecvt", SYM_FUNC, NULL,
	{ "stdlib.h" },
	{ "char *", "double", "int", "int *", "int *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"endhostent", SYM_FUNC, NULL,
	{ "netdb.h" },
	{ "void" },
	MASK_ALL, MASK_SINCE_SUS
}, {
	"fcvt", SYM_FUNC, NULL,
	{ "stdlib.h" },
	{ "char *", "double", "int", "int *", "int *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"ftime", SYM_FUNC, NULL,
	{ "sys/timeb.h" },
	{ "int", "struct timeb *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"gcvt", SYM_FUNC, NULL,
	{ "stdlib.h" },
	{ "char *", "double", "int", "char *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"getcontext", SYM_FUNC, NULL,
	{ "ucontext.h" },
	{ "int", "ucontext_t *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"getdelim", SYM_FUNC, NULL,
	{ "stdio.h" },
	{ "ssize_t", "char **", "size_t *", "int", "FILE *" },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"gethostbyaddr", SYM_FUNC, NULL,
	{ "netdb.h" },
	{ "struct hostent *", "const char *", "socklen_t", "int" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"gethostbyname", SYM_FUNC, NULL,
	{ "netdb.h" },
	{ "struct hostent *", "const char *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"gethostent", SYM_FUNC, NULL,
	{ "netdb.h" },
	{ "struct hostent *" },
	MASK_ALL, MASK_SINCE_SUS
}, {
	"getline", SYM_FUNC, NULL,
	{ "stdio.h" },
	{ "ssize_t", "char **", "size_t *", "FILE *" },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"getwd", SYM_FUNC, NULL,
	{ "unistd.h" },
	{ "char *", "char *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"index", SYM_FUNC, NULL,
	{ "strings.h" },
	{ "char *", "const char *", "int" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"makecontext", SYM_FUNC, NULL,
	{ "ucontext.h" },
	{ "void", "ucontext_t *", "void (*)()", "int" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"mktemp", SYM_FUNC, NULL,
	{ "stdlib.h" },
	{ "char *", "char *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"pthread_attr_getstackaddr", SYM_FUNC, NULL,
	{ "pthread.h" },
	{ "int", "const pthread_attr_t *", "void **" },
	MASK_ALL, (MASK_SINCE_P95|MASK_SINCE_SUSV2) & ~MASK_SINCE_SUSV4
}, {
	"pthread_attr_setstackaddr", SYM_FUNC, NULL,
	{ "pthread.h" },
	{ "int", "pthread_attr_t *", "void *" },
	MASK_ALL, (MASK_SINCE_P95|MASK_SINCE_SUSV2) & ~MASK_SINCE_SUSV4
}, {
	"rindex", SYM_FUNC, NULL,
	{ "strings.h" },
	{ "char *", "const char *", "int" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"scandir", SYM_FUNC, NULL,
	{ "sys/types.h", "dirent.h" },
	{ "int", "const char *", "struct dirent ***",
	    "int (*)(const struct dirent *)",
	    "int (*)(const struct dirent **, const struct dirent **)"  },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"setcontext", SYM_FUNC, NULL,
	{ "ucontext.h" },
	{ "int", "const ucontext_t *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"sethostent", SYM_FUNC, NULL,
	{ "netdb.h" },
	{ "void", "int" },
	MASK_ALL, MASK_SINCE_SUS
}, {
	"swapcontext", SYM_FUNC, NULL,
	{ "ucontext.h" },
	{ "int", "ucontext_t *", "const ucontext_t *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"ualarm", SYM_FUNC, NULL,
	{ "unistd.h" },
	{ "int", "useconds_t", "useconds_t" },
	MASK_IX, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"usleep", SYM_FUNC, NULL,
	{ "unistd.h" },
	{ "int", "useconds_t" },
	MASK_IX, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"vfork", SYM_FUNC, NULL,
	{ "unistd.h" },
	{ "pid_t" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"vforkx", SYM_FUNC, NULL,
	{ "sys/fork.h" },
	{ "pid_t", "int" },
	MASK_ALL, 0,	/* vforkx not in any standards */
}, {
	"wcpcpy", SYM_FUNC, NULL,
	{ "wchar.h" },
	{ "wchar_t *", "wchar_t *", "const wchar_t *" },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"wcpncpy", SYM_FUNC, NULL,
	{ "wchar.h" },
	{ "wchar_t *", "wchar_t *", "const wchar_t *", "size_t" },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"wcsdup", SYM_FUNC, NULL,
	{ "wchar.h" },
	{ "wchar_t *", "const wchar_t *" },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"wcscasecmp", SYM_FUNC, NULL,
	{ "wchar.h" },
	{ "int", "const wchar_t *", "const wchar_t *" },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"wcscasecmp_l", SYM_FUNC, NULL,
	{ "wchar.h" },
	{ "int", "const wchar_t *", "const wchar_t *", "locale_t" },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"wcslen", SYM_FUNC, NULL,	/* introduced in C90-AMD1 */
	{ "wchar.h" },
	{ "size_t", "const wchar_t *" },
	MASK_ALL, MASK_ALL
}, {
	"wcsncasecmp", SYM_FUNC, NULL,
	{ "wchar.h" },
	{ "int", "const wchar_t *", "const wchar_t *", "size_t" },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"wcsncasecmp_l", SYM_FUNC, NULL,
	{ "wchar.h" },
	{ "int", "const wchar_t *", "const wchar_t *", "size_t", "locale_t" },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"wcsnlen", SYM_FUNC, NULL,
	{ "wchar.h" },
	{ "size_t", "const wchar_t *", "size_t" },
	MASK_ALL, MASK_SINCE_SUSV4
}, {
	"wcswcs", SYM_FUNC, NULL,
	{ "wchar.h" },
	{ "wchar_t *", "const wchar_t *", "const wchar_t *" },
	MASK_ALL, MASK_SINCE_XPG4 & ~MASK_SINCE_SUSV4
},
