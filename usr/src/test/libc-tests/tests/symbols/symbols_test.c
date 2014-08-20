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
 * This program tests symbol visibility using the /usr/bin/c89 and
 * /usr/bin/c99 programs.
 *
 * See symbols_defs.c for the actual list of symbols tested.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <note.h>
#include <sys/wait.h>
#include "test_common.h"

char *dname;
char *cfile;
char *ofile;
char *lfile;

static int good_count = 0;
static int fail_count = 0;
static int full_count = 0;
static int extra_debug = 0;

/*
 * This requires Studio compilers to be installed.  We should make this
 * smarter, so that it can support GCC or CLANG.  Sadly, each compiler has
 * a different variation of -Werror -Wall.
 */

#if defined(_LP64)
#define	MFLAG "-m64"
#elif defined(_ILP32)
#define	MFLAG "-m32"
#endif

const char *compilers[] = {
	"cc",
	"gcc",
	"/opt/SUNWspro/bin/cc",
	"/opt/gcc/4.4.4/bin/gcc",
	"/opt/sunstudio12.1/bin/cc",
	"/opt/sfw/bin/gcc",
	"/usr/local/bin/gcc",
	NULL
};

const char *compiler = NULL;
const char *c89flags = NULL;
const char *c99flags = NULL;

struct compile_env {
	const char *desc;
	int cstd;
	char *flags;
} envs[] = {
	{ "XPG3", 89, "-D_XOPEN_SOURCE" },
	{ "XPG4", 89, "-D_XOPEN_SOURCE -D_XOPEN_VERSION=4" },
	{ "SUS/XPG4v2", 89, "-D_XOPEN_SOURCE=4 -D_XOPEN_SOURCE_EXTENDED" },
	{ "SUSv2", 89, "-D_XOPEN_SOURCE=500" },
	{ "SUSv3/POSIX.1-2001", 99, "-D_XOPEN_SOURCE=600" },
	{ "SUSv4/POSIX.1-2008", 99, "-D_XOPEN_SOURCE=700" },
	{ "POSIX.1-1990", 89, "-D_POSIX_SOURCE" },
	{ "POSIX.2-1992", 89, "-D_POSIX_SOURCE -D_POSIX_C_SOURCE=2" },
	{ "POSIX.1b-1993", 89, "-D_POSIX_C_SOURCE=199309L" },
	{ "POSIX.1c-1995", 89, "-D_POSIX_C_SOURCE=199506L" },
	{ "ISO C 90", 89, "" },
	{ "ISO C 99", 99, "" },
	{ NULL }
};

/* These have to match above */
#define	BIT(x)		(1 << (x))
#define	MASK_XPG3	BIT(0)
#define	MASK_XPG4	BIT(1)
#define	MASK_SUS	BIT(2)
#define	MASK_SUSV2	BIT(3)
#define	MASK_SUSV3	BIT(4)
#define	MASK_SUSV4	BIT(5)
#define	MASK_P90	BIT(6)
#define	MASK_P92	BIT(7)
#define	MASK_P93	BIT(8)
#define	MASK_P95	BIT(9)
#define	MASK_C89	BIT(10)
#define	MASK_C99	BIT(11)

#define	MASK_SINCE_SUSV4	(MASK_SUSV4)
#define	MASK_SINCE_SUSV3	(MASK_SINCE_SUSV4|MASK_SUSV3)
#define	MASK_SINCE_SUSV2	(MASK_SINCE_SUSV3|MASK_SUSV2)
#define	MASK_SINCE_SUS		(MASK_SINCE_SUSV2|MASK_SUS)
#define	MASK_SINCE_XPG4		(MASK_SINCE_SUS|MASK_XPG4)
#define	MASK_SINCE_XPG3		(MASK_SINCE_XPG4|MASK_XPG3)
#define	MASK_SINCE_P95		(MASK_SINCE_SUSV3|MASK_P95)
#define	MASK_SINCE_P93		(MASK_SINCE_SUSV3|MASK_SINCE_P95|MASK_P93)
#define	MASK_SINCE_P92		(MASK_SINCE_SUSV3|MASK_SINCE_P93|MASK_P92)
#define	MASK_SINCE_P90		(MASK_SINCE_SUSV3|MASK_SINCE_P92|MASK_P90)
/* SUSv3 and v4 depend upon C99 */
#define	MASK_SINCE_C99		(MASK_SINCE_SUSV3|MASK_C99)
#define	MASK_SINCE_C89	(MASK_SINCE_XPG3|MASK_SINCE_P90|MASK_C89|MASK_C99)
/*
 * UNIX definitions.  C standards allow implementation to include
 * other headers.  We can't reasonably check C89 or C99 conformance
 * when a header file that isn't part of the C standard is concerned.
 */
#define	MASK_UNIX		(MASK_SINCE_XPG3)
#define	MASK_POSIX		(MASK_SINCE_P90)
#define	MASK_IX			(MASK_UNIX|MASK_POSIX)

#define	MASK_ALL		(MASK_SINCE_C89|MASK_IX)

typedef enum { SYM_FUNC, SYM_TYPE, SYM_VALUE } symtype_t;

struct symbol_test {
	const char *symbol;
	symtype_t symtype;
	const char *desc;
	const char *includes[10];
	const char *types[10];
	int envmask;
	int vismask;
};

static const struct symbol_test stests[] = {

/* include the list of symbols here */
#include "symbols_defs.c"

	/* Ensure the list is terminated  */
	{ NULL }
};

static void
show_file(test_t t, const char *path)
{
	FILE *f;
	char *buf = NULL;
	size_t cap = 0;
	int line = 1;

	f = fopen(path, "r");

	test_debugf(t, "----->> begin (%s) <<------", path);
	while (getline(&buf, &cap, f) >= 0) {
		(void) strtok(buf, "\r\n");
		test_debugf(t, "%d: %s", line, buf);
		line++;
	}
	test_debugf(t, "----->> end (%s) <<------", path);
	(void) fclose(f);
}

static void
cleanup(void)
{
	if (ofile != NULL) {
		(void) unlink(ofile);
		free(ofile);
		ofile = NULL;
	}
	if (lfile != NULL) {
		(void) unlink(lfile);
		free(lfile);
		lfile = NULL;
	}
	if (cfile != NULL) {
		(void) unlink(cfile);
		free(cfile);
		cfile = NULL;
	}
	if (dname) {
		(void) rmdir(dname);
		free(dname);
		dname = NULL;
	}
}

static int
mkworkdir(test_t t, const char *symbol)
{
	char *d;

	cleanup();

	d = tmpnam(NULL);

	if (mkdir(d, 0755) != 0) {
		test_failed(t, "mkdir: %s", strerror(errno));
		return (-1);
	}
	dname = strdup(d);
	(void) asprintf(&cfile, "%s/%s_test.c", d, symbol);
	(void) asprintf(&ofile, "%s/%s_test.o", d, symbol);
	(void) asprintf(&lfile, "%s/%s_test.log", d, symbol);
	return (0);
}

void
find_compiler(void)
{
	test_t t;
	int i;
	FILE *cf;

	t = test_start("finding compiler");

	if (mkworkdir(t, "cc") < 0) {
		return;
	}
	if ((cf = fopen(cfile, "w+")) == NULL) {
		cleanup();
		return;
	}
	(void) fprintf(cf, "#include <stdio.h>\n");
	(void) fprintf(cf, "int main(int argc, char **argv) {\n");
	(void) fprintf(cf, "#if defined(__SUNPRO_C)\n");
	(void) fprintf(cf, "exit(51);\n");
	(void) fprintf(cf, "#elif defined(__GNUC__)\n");
	(void) fprintf(cf, "exit(52);\n");
	(void) fprintf(cf, "#else\n");
	(void) fprintf(cf, "exit(99)\n");
	(void) fprintf(cf, "#endif\n}\n");
	(void) fclose(cf);

	for (i = 0; compilers[i] != NULL; i++) {
		char cmd[256];
		int rv;

		test_debugf(t, "trying %s", compilers[i]);
		(void) snprintf(cmd, sizeof (cmd),
		    "%s %s -o %s >/dev/null 2>&1",
		    compilers[i], cfile, ofile);
		rv = system(cmd);

		if (rv != 0) {
			continue;
		}

		rv = system(ofile);
		if (WIFEXITED(rv)) {
			rv = WEXITSTATUS(rv);
		} else {
			rv = -1;
		}

		switch (rv) {
		case 51:	/* STUDIO */
			test_debugf(t, "Found Studio C");
			compiler = compilers[i];
			c89flags = "-Xc -errwarn=%all -v -xc99=%none " MFLAG;
			c99flags = "-Xc -errwarn=%all -v -xc99=%all " MFLAG;
			if (extra_debug) {
				test_debugf(t, "compiler: %s", compiler);
				test_debugf(t, "c89flags: %s", c89flags);
				test_debugf(t, "c99flags: %s", c99flags);
			}
			test_passed(t);
			cleanup();
			return;
		case 52:	/* GCC */
			test_debugf(t, "Found GNU C");
			compiler = compilers[i];
			c89flags = "-Wall -Werror -std=c89 " MFLAG;
			c99flags = "-Wall -Werror -std=c99 " MFLAG;
			if (extra_debug) {
				test_debugf(t, "compiler: %s", compiler);
				test_debugf(t, "c89flags: %s", c89flags);
				test_debugf(t, "c99flags: %s", c99flags);
			}
			test_passed(t);
			cleanup();
			return;
		default:
			continue;
		}
	}
	test_failed(t, "No compiler found.");
	cleanup();
}

/*
 * This function handles creation of a C file.  It understands
 * functions, values (usually macros), and types.  It also understands
 * function pointer types, which adds a lot of complexity.
 */
void
mkcfile(test_t t, const struct symbol_test *st)
{
	FILE *f;
	int i;
	const char *rvsuffix = NULL;

	if (mkworkdir(t, st->symbol) < 0) {
		return;
	}

	if ((f = fopen(cfile, "w+")) == NULL) {
		test_failed(t, "fopen: %s", strerror(errno));
		cleanup();
		return;
	}

	for (i = 0; st->includes[i] != NULL; i++) {
		(void) fprintf(f, "#include <%s>\n", st->includes[i]);
	}

	for (rvsuffix = st->types[0]; *rvsuffix; rvsuffix++) {
		(void) fputc(*rvsuffix, f);
		if (*rvsuffix == '(') {
			rvsuffix++;
			(void) fputc(*rvsuffix, f);
			rvsuffix++;
			break;
		}
	}
	if (*rvsuffix == 0) {
		(void) fputc(' ', f);
	}
	switch (st->symtype) {
	case SYM_TYPE:
		(void) fprintf(f, "test_%s", st->symbol);
		if (*rvsuffix != 0) {
			(void) fprintf(f, "%s", rvsuffix);
		}
		(void) fputc(';', f);
		(void) fputc('\n', f);
		break;
	case SYM_VALUE:
		(void) fprintf(f, "test_%s", st->symbol);
		if (*rvsuffix != 0) {
			(void) fprintf(f, "%s", rvsuffix);
		}
		(void) fputc(';', f);
		(void) fputc('\n', f);
		(void) fprintf(f, "void func_%s(void) { ", st->symbol);
		(void) fprintf(f, "test_%s = %s;\n", st->symbol, st->symbol);
		(void) fprintf(f, "}\n");
		break;
	case SYM_FUNC:
		(void) fprintf(f, "test_%s(", st->symbol);
		for (i = 1; st->types[i] != NULL; i++) {
			const char *s;
			int didarg = 0;
			if (i > 1) {
				(void) fprintf(f, ", ");
			}
			for (s = st->types[i]; *s; s++) {
				(void) fputc(*s, f);
				if (*s == '(' && s[1] == '*') {
					(void) fprintf(f, "*a%d", i);
					didarg = 1;
					s++;
				}
			}
			if (!didarg) {
				(void) fprintf(f, " a%d", i);
			}
		}
		if (i == 1) {
			(void) fprintf(f, "void");
		}
		(void) fprintf(f, ")");
		if (*rvsuffix != 0) {
			(void) fprintf(f, "%s", rvsuffix);
		}
		(void) fprintf(f, " { ");
		if ((strcmp(st->types[0], "") != 0) &&
		    (strcmp(st->types[0], "void") != 0)) {
			(void) fprintf(f, "return ");
		}
		(void) fprintf(f, "%s(", st->symbol);
		for (i = 1; st->types[i] != NULL; i++) {
			(void) fprintf(f, "%sa%d", i > 1 ? ", " : "", i);
		}
		(void) fprintf(f, "); }\n");
		break;
	}
	(void) fclose(f);
	if (extra_debug)
		test_debugf(t, "cfile %s", cfile);
}

int
do_compile(test_t t, int idx, int vis)
{
	struct compile_env *e = &envs[idx];
	char *cmd;
	FILE *logf;

	test_debugf(t, "%s (%s)", e->desc, vis ? "+" : "-");

	if (asprintf(&cmd, "%s %s %s -c %s -o %s >>%s 2>&1",
	    compiler, e->cstd == 99 ? c99flags : c89flags,
	    e->flags, cfile, ofile, lfile) < 0) {
		test_failed(t, "asprintf: %s", strerror(errno));
		return (-1);
	}

	if (extra_debug) {
		test_debugf(t, "command: %s", cmd);
	}

	full_count++;

	if ((logf = fopen(lfile, "w+")) == NULL) {
		test_failed(t, "fopen: %s", strerror(errno));
		cleanup();
		return (-1);
	}
	(void) fprintf(logf, "COMMAND: %s\n", cmd);
	(void) fprintf(logf, "EXPECT: %s\n", vis ? "OK" : "FAIL");
	(void) fclose(logf);

	if (system(cmd) != 0) {
		if (vis) {
			fail_count++;
			show_file(t, lfile);
			show_file(t, cfile);
			cleanup();
			test_failed(t, "error compiling in %s", e->desc);
			return (-1);
		}
	} else {
		if (!vis) {
			fail_count++;
			show_file(t, lfile);
			show_file(t, cfile);
			cleanup();
			test_failed(t, "symbol visible in %s", e->desc);
			return (-1);
		}
	}
	good_count++;
	return (0);
}

void
test_compile(void)
{
	const struct symbol_test *st;
	int i, bit;
	test_t t;
	int rv = 0;

	for (i = 0; stests[i].symbol != NULL; i++) {
		st = &stests[i];
		if (st->desc != NULL) {
			t = test_start("%s (%s)", st->symbol, st->desc);
		} else {
			t = test_start(st->symbol);
		}
		mkcfile(t, st);
		for (bit = 0; bit < 31; bit++) {
			int m = 1U << bit;
			if ((st->envmask & m) == 0) {
				continue;
			}
			if (do_compile(t, bit, st->vismask & m) != 0) {
				rv = -1;
			}
		}
		if (rv == 0) {
			test_passed(t);
		}
		cleanup();
	}

	t = test_start("Summary");
	if (rv != 0) {
		test_failed(t, "Test FAIL/PASS: %d/%d (%d %%)",
		    fail_count, good_count, fail_count * 100 / full_count);
	} else {
		test_debugf(t, "Test PASS/TOTAL: %d/%d (%d %%)",
		    good_count, good_count, good_count * 100 / full_count);
		test_passed(t);
	}
}

int
main(int argc, char **argv)
{
	int optc;
	int optC = 0;

	while ((optc = getopt(argc, argv, "DdfC")) != EOF) {
		switch (optc) {
		case 'd':
			test_set_debug();
			break;
		case 'f':
			test_set_force();
			break;
		case 'D':
			test_set_debug();
			extra_debug++;
			break;
		case 'C':
			optC++;
			break;
		default:
			(void) fprintf(stderr, "Usage: %s [-df]\n", argv[0]);
			exit(1);
		}
	}

	find_compiler();
	if (!optC)
		test_compile();

	exit(0);
}
