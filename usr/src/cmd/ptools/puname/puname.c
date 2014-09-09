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
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 *
 * Portions Copyright 2008 Chad Mynhier
 *
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <libproc.h>
#include <wait.h>

static	char	*command;
static	char	*pidarg;

static	int	Fflag;
static	int	errflg;
static	int	Sflag;
static	int	Uflag;
static	int	vflag;

int
main(int argc, char **argv)
{
	int opt;
	pid_t pid;
	int gret;
	int isalt;
	pid_t parent = 0;
	struct ps_prochandle *Pr;

	if ((command = strrchr(argv[0], '/')) != NULL)
		command++;
	else
		command = argv[0];

	while ((opt = getopt(argc, argv, "Fp:SUv")) != EOF) {
		switch (opt) {
		case 'F':		/* force grabbing (no O_EXCL) */
			Fflag = PGRAB_FORCE;
			break;
		case 'S':
			Sflag = 1;
			break;
		case 'U':
			Uflag = 1;
			break;
		case 'p':
			pidarg = optarg;
			break;
		case 'v':
			vflag = 1;
			break;
		default:
			errflg = 1;
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if (Sflag && Uflag) {
		errflg++;
	}

	if (((pidarg != NULL) ^ (argc < 1)) || errflg) {
		(void) fprintf(stderr,
		    "usage:\t%s [-S|-U] [-p pid | command [ args ... ]]\n",
		    command);
		(void) fprintf(stderr,
		    "  (set or unset alternate uname for process)\n");
		return (1);
	}

	if (pidarg == NULL) {
		/*
		 * We perform the action in a child.  This allows us
		 * to avoid masquerading our status, so that when run
		 * in a shell the parent gets to see our status directly.
		 */
		pid = fork();
		switch (pid) {
		case 0:
			parent = getppid();
			(void) asprintf(&pidarg, "%d", (int)parent);
			break;
		case -1:
			(void) fprintf(stderr, "%s: fork1: %s\n",
			    command, strerror(errno));
			return (1);
		default:
			if (waitpid(pid, &gret, 0) < 0) {
				(void) fprintf(stderr, "%s: waitpid: %s\n",
				    command, strerror(errno));
				return (1);
			}
			if ((!WIFEXITED(gret)) || (WEXITSTATUS(gret) != 0)) {
				return (1);
			}
			(void) execvp(argv[0], argv);
			(void) fprintf(stderr, "%s: cannot exec %s: %s\n",
			    command, argv[0], strerror(errno));
			return (1);
			break;
		}
	}
	if ((Pr = proc_arg_grab(pidarg, PR_ARG_PIDS, Fflag, &gret)) == NULL) {
		(void) fprintf(stderr, "%s: cannot examine %s: %s\n",
		    command, pidarg, Pgrab_error(gret));
		return (1);
	}
	pid = Pstatus(Pr)->pr_pid;
	if (Sflag) {
		if (Psetflags(Pr, PR_AUNAME) != 0) {
			(void) fprintf(stderr,
			    "%s: failed to set flag %s: %s\n",
			    command, pidarg, strerror(errno));
			return (1);
		}
	} else if (Uflag) {
		if (Punsetflags(Pr, PR_AUNAME) != 0) {
			(void) fprintf(stderr,
			    "%s: failed to unset flag %s: %s\n",
			    command, pidarg, strerror(errno));
			return (1);
		}
	}
	isalt = Pstatus(Pr)->pr_flags & PR_AUNAME;

	if (vflag || !(Sflag || Uflag)) {
		(void) fprintf(stderr, "%d: %s\n", (int)pid,
		    isalt ? "alternate uname" : "standard uname");
	}

	Prelease(Pr, 0);
	return (0);
}
