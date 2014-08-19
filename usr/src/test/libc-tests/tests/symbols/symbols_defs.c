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
},

/*
 * Functions
 */
{
	"bsd_signal", SYM_FUNC, NULL,
	{ "signal.h" },
	{ "void (*)(int)", "int", "void (*)(int)" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"ecvt", SYM_FUNC, NULL,
	{ "stdlib.h" },
	{ "char *", "double", "int", "int *", "int *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"fcvt", SYM_FUNC, NULL,
	{ "stdlib.h" },
	{ "char *", "double", "int", "int *", "int *" },
	MASK_ALL, MASK_SINCE_SUS & ~MASK_SINCE_SUSV4
}, {
	"gcvt", SYM_FUNC, NULL,
	{ "stdlib.h" },
	{ "char *", "double", "int", "char *" },
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
},
