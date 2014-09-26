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
 * This program tests that open works properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "test_common.h"

static void
test_open_1(void)
{
	test_t t;
        char	fname[32];
	int	fd;
	int	err;
	char	*dname;

        (void) strcpy(fname, "/tmp/testXXXXXX");
        t = test_start("open of directory");

        dname = mkdtemp(fname);
        if (dname == NULL) {
                test_failed(t, "mkdtemp failed: %s", strerror(errno));
                return;
        }
	fd = open(fname, O_DIRECTORY);
	err = errno;

        /* don't leave it in the filesystem */
       	(void) rmdir(fname);
	if (fd >= 0)
		(void) close(fd);

	if (fd < 0) {
		test_failed(t, "open(): %s", strerror(err));
		return;
	}
	
	test_passed(t);
}

static void
test_open_2(void)
{
	test_t	t;
        char	fname[32];
	int	fd;
	int	ffd;
	int	err;

        (void) strcpy(fname, "/tmp/testXXXXXX");
        t = test_start("O_DIRECTORY enforcement (file)");

        ffd = mkstemp(fname);
        if (ffd < 0) {
                test_failed(t, "mkstemp failed: %s", strerror(errno));
                return;
        }
	fd = open(fname, O_DIRECTORY);
	err = errno;
	if (fd >= 0)
		(void) close(fd);
	(void) close(ffd);
	(void) unlink(fname);

	if (fd >= 0) {
		test_failed(t, "O_DIRECTORY open of file did not fail");
		return;

	}

	if (errno != ENOTDIR) {
		test_failed(t, "errno is not ENOTDIR (%d, %s)", err,
		    strerror(err));
		return;
	}

	test_passed(t);
}

static void
test_open_3(void)
{
	test_t	t;
	int	fd;
	int	err;

        t = test_start("O_DIRECTORY enforcement (snode)");
	fd = open("/dev/null", O_DIRECTORY);
	err = errno;
	if (fd >= 0)
		(void) close(fd);

	if (fd >= 0) {
		test_failed(t, "O_DIRECTORY open of snode did not fail");
		return;

	}

	if (errno != ENOTDIR) {
		test_failed(t, "errno is not ENOTDIR (%d, %s)", err,
		    strerror(err));
		return;
	}

	test_passed(t);
}
	
static void
test_open(void)
{
	test_open_1();
	test_open_2();
	test_open_3();
}

int
main(int argc, char **argv)
{
	int optc;

	while ((optc = getopt(argc, argv, "df")) != EOF) {
		switch (optc) {
		case 'd':
			test_set_debug();
			break;
		case 'f':
			test_set_force();
			break;
		default:
			(void) fprintf(stderr, "Usage: %s [-df]\n", argv[0]);
			exit(1);
		}
	}

	test_open();

	exit(0);
}
