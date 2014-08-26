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

const char *sym = NULL;

static int good_count = 0;
static int fail_count = 0;
static int full_count = 0;
static int extra_debug = 0;
static char *compilation = "compilation.cfg";

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

/* ====== BEGIN ======== */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#define	MAXENV	64	/* bits */
#define	MAXHDR	10	/* maximum # headers to require to access symbol */
#define	MAXARG	20	/* maximum # of arguments */

#define	WS	" \t"

static int next_env = 0;

struct compile_env {
	char		*name;
	char		*lang;
	char		*defs;
	int		index;
};

static struct compile_env compile_env[MAXENV];

struct env_group {
	char			*name;
	uint64_t		mask;
	struct env_group	*next;
};

typedef enum { SYM_TYPE, SYM_VALUE, SYM_FUNC } sym_type_t;

struct sym_test {
	char			*name;
	sym_type_t		type;
	char			*hdrs[MAXHDR];
	char			*rtype;
	char			*atypes[MAXARG];
	uint64_t		test_mask;
	uint64_t		need_mask;
	char			*prog;
	struct sym_test		*next;
};

struct env_group *env_groups = NULL;

struct sym_test *sym_tests = NULL;
struct sym_test **sym_insert = &sym_tests;

static void
append_sym_test(struct sym_test *st)
{
	*sym_insert = st;
	sym_insert = &st->next;
}

static int
find_env_mask(const char *name, uint64_t *mask)
{
	for (int i = 0; i < 64; i++) {
		if (compile_env[i].name != NULL &&
		    strcmp(compile_env[i].name, name) == 0) {
			*mask |= (1ULL << i);
			return (0);
		}
	}

	for (struct env_group *eg = env_groups; eg != NULL; eg = eg->next) {
		if (strcmp(name, eg->name) == 0) {
			*mask |= eg->mask;
			return (0);
		}
	}
	return (-1);
}


static int
expand_env(char *list, uint64_t *mask, char **erritem)
{
	char *item;
	for (item = strtok(list, WS); item != NULL; item = strtok(NULL, WS)) {
		if (find_env_mask(item, mask) < 0) {
			if (erritem != NULL) {
				*erritem = item;
			}
			return (-1);
		}
	}
	return (0);
}

static int
expand_env_list(char *list, uint64_t *test, uint64_t *need, char **erritem)
{
	uint64_t mask = 0;
	int act;
	char *item;
	for (item = strtok(list, WS); item != NULL; item = strtok(NULL, WS)) {
		switch (item[0]) {
		case '+':
			act = 1;
			item++;
			break;
		case '-':
			act = 0;
			item++;
			break;
		default:
			act = 1;
			break;
		}

		mask = 0;
		if (find_env_mask(item, &mask) < 0) {
			if (erritem != NULL) {
				*erritem = item;
			}
			return (-1);
		}
		*test |= mask;
		if (act) {
			*need |= mask;
		} else {
			*need &= ~(mask);
		}
	}
	return (0);
}

static int
do_env(char **fields, int nfields, char **err)
{
	char *name;
	char *lang;
	char *defs;

	if (nfields != 3) {
		(void) asprintf(err, "number of fields (%d) != 3", nfields);
		return (-1);
	}

	if (next_env >= MAXENV) {
		(void) asprintf(err, "too many environments");
		return (-1);
	}

	name = fields[0];
	lang = fields[1];
	defs = fields[2];

	compile_env[next_env].name = strdup(name);
	compile_env[next_env].lang = strdup(lang);
	compile_env[next_env].defs = strdup(defs);
	compile_env[next_env].index = next_env;
	next_env++;
	return (0);
}

static int
do_env_group(char **fields, int nfields, char **err)
{
	char *name;
	char *list;
	struct env_group *eg;
	uint64_t mask;
	char *item;

	if (nfields != 2) {
		(void) asprintf(err, "number of fields (%d) != 2", nfields);
		return (-1);
	}

	name = fields[0];
	list = fields[1];
	mask = 0;

	if (expand_env(list, &mask, &item) < 0) {
		(void) asprintf(err, "reference to undefined env %s", item);
		return (-1);
	}

	eg = calloc(1, sizeof (*eg));
	eg->name = strdup(name);
	eg->mask = mask;
	eg->next = env_groups;
	env_groups = eg;
	return (0);
}

static void
mkprog(struct sym_test *st)
{
	static char buf[2048];
	char *s;
	char *prog = buf;

	*prog = 0;

#define	ADDSTR(p, str)	(void) strcpy(p, str); p += strlen(p)
#define	ADDFMT(p, ...)	\
	(void) snprintf(p, sizeof (buf) - (p-buf), __VA_ARGS__); \
	p += strlen(p)
#define	ADDCHR(p, c)	*p++ = c; *p = 0

	for (int i = 0; i < MAXHDR && st->hdrs[i] != NULL; i++) {
		ADDFMT(prog, "#include <%s>\n", st->hdrs[i]);
	}

	for (s = st->rtype; *s; s++) {
		ADDCHR(prog, *s);
		if (*s == '(') {
			s++;
			ADDCHR(prog, *s);
			s++;
			break;
		}
	}
	ADDCHR(prog, ' ');

	/* for function pointers, s is closing suffix, otherwise empty */

	switch (st->type) {
	case SYM_TYPE:
		ADDFMT(prog, "test_type;", st->rtype);
		break;

	case SYM_VALUE:
		ADDFMT(prog, "test_value%s;\n", s);	/* s usually empty */
		ADDSTR(prog, "void\ntest_func(void)\n{\n");
		ADDFMT(prog, "\ttest_value = %s;\n}",
		    st->name);
		break;

	case SYM_FUNC:
		ADDSTR(prog, "\ntest_func(");
		for (int i = 0; st->atypes[i] != NULL && i < MAXARG; i++) {
			int didname = 0;
			if (i > 0) {
				ADDSTR(prog, ", ");
			}
			if (strcmp(st->atypes[i], "void") == 0) {
				didname = 1;
			}
			if (strcmp(st->atypes[i], "") == 0) {
				didname = 1;
				ADDSTR(prog, "void");
			}

			/* print the argument list */
			for (char *a = st->atypes[i]; *a; a++) {
				if (*a == '(' && a[1] == '*' && !didname) {
					ADDFMT(prog, "(*a%d", i);
					didname = 1;
					a++;
				} else if (*a == '[' && !didname) {
					ADDFMT(prog, "a%d[", i);
					didname = 1;
				} else {
					ADDCHR(prog, *a);
				}
			}
			if (!didname) {
				ADDFMT(prog, " a%d", i);
			}
		}

		if (st->atypes[0] == NULL) {
			ADDSTR(prog, "void");
		}

		/* close argument list, and closing ")" for func ptrs */
		ADDFMT(prog, ")%s\n{\n\t", s);	/* NB: s is normally empty */

		if (strcmp(st->rtype, "") != 0 &&
		    strcmp(st->rtype, "void") != 0) {
			ADDSTR(prog, "return ");
		}

		/* add the function call */
		ADDFMT(prog, "%s(", st->name);
		for (int i = 0; st->atypes[i] != NULL && i < MAXARG; i++) {
			if (strcmp(st->atypes[i], "") != 0 &&
			    strcmp(st->atypes[i], "void") != 0) {
				ADDFMT(prog, "%sa%d", i > 0 ? ", " : "", i);
			}
		}

		ADDSTR(prog, ");\n}");
		break;
	}

	ADDCHR(prog, '\n');

	st->prog = strdup(buf);
}

static int
add_envs(struct sym_test *st, char *envs, char **err)
{
	char *item;
	if (expand_env_list(envs, &st->test_mask, &st->need_mask, &item) < 0) {
		(void) asprintf(err, "bad env action %s", item);
		return (-1);
	}
	return (0);
}

static int
add_headers(struct sym_test *st, char *hdrs, char **err)
{
	int i = 0;

	for (char *h = strsep(&hdrs, ";"); h != NULL; h = strsep(&hdrs, ";")) {
		if (i >= MAXHDR) {
			(void) asprintf(err, "too many headers");
			return (-1);
		}
		test_trim(&h);
		st->hdrs[i++] = strdup(h);
	}

	return (0);
}

static int
add_arg_types(struct sym_test *st, char *atype, char **err)
{
	int i = 0;
	char *a;
	for (a = strsep(&atype, ";"); a != NULL; a = strsep(&atype, ";")) {
		if (i >= MAXARG) {
			(void) asprintf(err, "too many arguments");
			return (-1);
		}
		test_trim(&a);
		st->atypes[i++] = strdup(a);
	}

	return (0);
}

static int
do_type(char **fields, int nfields, char **err)
{
	char *decl;
	char *hdrs;
	char *envs;
	struct sym_test *st;

	if (nfields != 3) {
		(void) asprintf(err, "number of fields (%d) != 3", nfields);
		return (-1);
	}
	decl = fields[0];
	hdrs = fields[1];
	envs = fields[2];

	st = calloc(1, sizeof (*st));
	st->type = SYM_TYPE;
	st->name = strdup(decl);
	st->rtype = strdup(decl);

	if ((add_envs(st, envs, err) < 0) ||
	    (add_headers(st, hdrs, err) < 0)) {
		return (-1);
	}
	append_sym_test(st);

	return (0);
}

static int
do_value(char **fields, int nfields, char **err)
{
	char *name;
	char *type;
	char *hdrs;
	char *envs;
	struct sym_test *st;

	if (nfields != 4) {
		(void) asprintf(err, "number of fields (%d) != 4", nfields);
		return (-1);
	}
	name = fields[0];
	type = fields[1];
	hdrs = fields[2];
	envs = fields[3];

	st = calloc(1, sizeof (*st));
	st->type = SYM_VALUE;
	st->name = strdup(name);
	st->rtype = strdup(type);

	if ((add_envs(st, envs, err) < 0) ||
	    (add_headers(st, hdrs, err) < 0)) {
		return (-1);
	}
	append_sym_test(st);

	return (0);
}

static int
do_func(char **fields, int nfields, char **err)
{
	char *name;
	char *rtype;
	char *atype;
	char *hdrs;
	char *envs;
	struct sym_test *st;

	if (nfields != 5) {
		(void) asprintf(err, "number of fields (%d) != 5", nfields);
		return (-1);
	}
	name = fields[0];
	rtype = fields[1];
	atype = fields[2];
	hdrs = fields[3];
	envs = fields[4];

	st = calloc(1, sizeof (*st));
	st->type = SYM_FUNC;
	st->name = strdup(name);
	st->rtype = strdup(rtype);

	if ((add_envs(st, envs, err) < 0) ||
	    (add_headers(st, hdrs, err) < 0) ||
	    (add_arg_types(st, atype, err) < 0)) {
		return (-1);
	}
	append_sym_test(st);

	return (0);
}

struct sym_test *
next_sym_test(struct sym_test *st)
{
	return (st == NULL ? sym_tests : st->next);
}

const char *
sym_test_prog(struct sym_test *st)
{
	if (st->prog == NULL) {
		mkprog(st);
	}
	return (st->prog);
}

const char *
sym_test_name(struct sym_test *st)
{
	return (st->name);
}

/*
 * Iterate through tests.  Pass NULL for cenv first time, and previous result
 * the next.  Returns NULL when no more environments.
 */
struct compile_env *
sym_test_env(struct sym_test *st, struct compile_env *cenv, int *need)
{
	int i = cenv ? cenv->index + 1: 0;
	uint64_t b = 1ULL << i;

	while ((i < MAXENV) && (b != 0)) {
		cenv = &compile_env[i];
		if (b & st->test_mask) {
			*need = (st->need_mask & b) ? 1 : 0;
			return (cenv);
		}
		b <<= 1;
		i++;
	}
	return (NULL);
}

const char *
env_name(struct compile_env *cenv)
{
	return (cenv->name);
}

const char *
env_lang(struct compile_env *cenv)
{
	return (cenv->lang);
}

const char *
env_defs(struct compile_env *cenv)
{
	return (cenv->defs);
}

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
mkworkdir(void)
{
	char b[32];
	char *d;

	cleanup();

	(void) strlcpy(b, "/tmp/symbols_testXXXXXX", sizeof (b));
	if ((d = mkdtemp(b)) == NULL) {
		perror("mkdtemp");
		return (-1);
	}
	dname = strdup(d);
	(void) asprintf(&cfile, "%s/compile_test.c", d);
	(void) asprintf(&ofile, "%s/compile_test.o", d);
	(void) asprintf(&lfile, "%s/compile_test.log", d);
	return (0);
}

void
find_compiler(void)
{
	test_t t;
	int i;
	FILE *cf;

	t = test_start("finding compiler");

	if ((cf = fopen(cfile, "w+")) == NULL) {
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
			return;
		default:
			continue;
		}
	}
	test_failed(t, "No compiler found.");
}

int
do_compile(test_t t, struct sym_test *st, struct compile_env *cenv, int need)
{
	char *cmd;
	FILE *logf;
	FILE *dotc;
	const char *prog;

	full_count++;

	if ((dotc = fopen(cfile, "w+")) == NULL) {
		test_failed(t, "fopen(%s): %s", cfile, strerror(errno));
		return (-1);
	}
	prog = sym_test_prog(st);
	if (fwrite(prog, 1, strlen(prog), dotc) < strlen(prog)) {
		test_failed(t, "fwrite: %s", strerror(errno));
		(void) fclose(dotc);
		return (-1);
	}
	if (fclose(dotc) < 0) {
		test_failed(t, "fclose: %s", strerror(errno));
		return (-1);
	}

	(void) unlink(ofile);

	if (asprintf(&cmd, "%s %s %s -c %s -o %s >>%s 2>&1",
	    compiler, strcmp(env_lang(cenv), "c99") == 0 ? c99flags : c89flags,
	    env_defs(cenv), cfile, ofile, lfile) < 0) {
		test_failed(t, "asprintf: %s", strerror(errno));
		return (-1);
	}

	if (extra_debug) {
		test_debugf(t, "command: %s", cmd);
	}


	if ((logf = fopen(lfile, "w+")) == NULL) {
		test_failed(t, "fopen: %s", strerror(errno));
		return (-1);
	}
	(void) fprintf(logf, "===================\n");
	(void) fprintf(logf, "PROGRAM:\n%s\n", sym_test_prog(st));
	(void) fprintf(logf, "COMMAND: %s\n", cmd);
	(void) fprintf(logf, "EXPECT: %s\n", need ? "OK" : "FAIL");
	(void) fclose(logf);

	if (system(cmd) != 0) {
		if (need) {
			fail_count++;
			show_file(t, lfile);
			test_failed(t, "error compiling in %s", env_name(cenv));
			return (-1);
		}
	} else {
		if (!need) {
			fail_count++;
			show_file(t, lfile);
			test_failed(t, "symbol visible in %s", env_name(cenv));
			return (-1);
		}
	}
	good_count++;
	return (0);
}

void
test_compile(void)
{
	struct sym_test *st;
	struct compile_env *cenv;
	test_t t;
	int need;

	for (st = next_sym_test(NULL); st; st = next_sym_test(st)) {
		if ((sym != NULL) && strcmp(sym, sym_test_name(st))) {
			continue;
		}
		/* XXX: we really want a sym_test_desc() */
		for (cenv = sym_test_env(st, NULL, &need);
		    cenv != NULL;
		    cenv = sym_test_env(st, cenv, &need)) {
			t = test_start("%s : %c%s", st->name,
			    need ? '+' : '-', env_name(cenv));
			if (do_compile(t, st, cenv, need) == 0) {
				test_passed(t);
			}
		}
	}

	if (full_count > 0) {
		test_summary();
	}
}

int
main(int argc, char **argv)
{
	int optc;
	int optC = 0;

	while ((optc = getopt(argc, argv, "DdfCs:c:")) != EOF) {
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
		case 'c':
			compilation = optarg;
			break;
		case 'C':
			optC++;
			break;
		case 's':
			sym = optarg;
			break;
		default:
			(void) fprintf(stderr, "Usage: %s [-df]\n", argv[0]);
			exit(1);
		}
	}

	if (test_load_config(NULL, compilation,
	    "env", do_env, "env_group", do_env_group, NULL) < 0) {
		exit(1);
	}

	while (optind < argc) {
		if (test_load_config(NULL, argv[optind++],
		    "type", do_type,
		    "value", do_value,
		    "func", do_func,
		    NULL) < 0) {
			exit(1);
		}
	}

	(void) atexit(cleanup);

	if (mkworkdir() < 0) {
		perror("mkdir");
		exit(1);
	}

	find_compiler();
	if (!optC)
		test_compile();

	exit(0);
}
