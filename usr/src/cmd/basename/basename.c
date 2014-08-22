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
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char **argv)
{
	char	*p;
	char	*string;
	char	*suffix;

	/*
	 * For better performance, defer the setlocale()/textdomain()
	 * calls until they get really required.
	 */
#if !defined(TEXT_DOMAIN)
#define	TEXT_DOMAIN "SYS_TEST"
#endif
	if (argc == 1) {
		(void) puts(".");
		return (0);
	}

	if (strcmp(argv[1], "--") == 0) {
		argv++;
		argc--;
		if (argc == 1) {
			(void) puts(".");
			return (0);
		}
	}

	if (argc > 3) {
		(void) setlocale(LC_ALL, "");
		(void) textdomain(TEXT_DOMAIN);
		(void) fputs(gettext("Usage: basename string [ suffix ]\n"),
		    stderr);
		return (1);
	}

	string = argv[1];
	suffix = (argc == 2) ? NULL : argv[2];

	if (*string == '\0') {
		(void) puts(".");
		return (0);
	}

	/* remove trailing slashes */
	p = string + strlen(string) - 1;
	while (p >= string && *p == '/')
		*p-- = '\0';

	if (*string == '\0') {
		(void) puts("/");
		return (0);
	}

	/* skip to one past last slash */
	if ((p = strrchr(string, '/')) != NULL)
		string = p + 1;

	if (suffix == NULL) {
		(void) puts(string);
		return (0);
	}

	/*
	 * if a suffix is present and is not the same as the remaining
	 * string and is identical to the last characters in the remaining
	 * string, remove those characters from the string.
	 */
	if (strcmp(string, suffix) != 0) {
		p = string + strlen(string) - strlen(suffix);
		if (strcmp(p, suffix) == 0)
			*p = '\0';
	}
	(void) puts(string);
	return (0);
}
