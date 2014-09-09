#
# This file and its contents are supplied under the terms of the
# Common Development and Distribution License ("CDDL"), version 1.0.
# You may only use this file in accordance with the terms of version
# 1.0 of the CDDL.
#
# A full copy of the text of the CDDL should have accompanied this
# source.  A copy of the CDDL is also available via the Internet at
# http://www.illumos.org/license/CDDL.
#

#
# Copyright 2014 Garrett D'Amore <garrett@damore.org>
#

PROGS=		less lesskey
XPG4PROG =	more
LINKS =		more


OBJS=		$(less_OBJS) $(lesskey_OBJS)

CLOBBERFILES +=	$(PROGS)

include		$(SRC)/cmd/Makefile.cmd
include		$(SRC)/cmd/less/Makefile.common

ROOTLINKS =	$(LINKS:%=$(ROOTBIN)/%)
ROOTPROGS =	$(PROGS:%=$(ROOTBIN)/%) $(ROOTLINKS) $(ROOTXPG4PROG)
$(ROOTLINKS) 	:= INSLINKTARGET = $(ROOTBIN)/less
$(ROOTXPG4PROG)	:= INSLINKTARGET = ../../bin/less

.KEEP_STATE:

all:		$(PROGS)

less:		$(less_OBJS)
		$(LINK.c) $(less_OBJS) -o $@ $(LDLIBS) $($(@)_LDLIBS)
		$(POST_PROCESS)

lesskey:	$(lesskey_OBJS)
		$(LINK.c) $(lesskey_OBJS) -o $@ $(LDLIBS) $($(@)_LDLIBS)
		$(POST_PROCESS)

clean:
		$(RM) $(OBJS)

install:	all $(ROOTPROGS)

lint:		less_LINT lesskey_LINT

less_LINT:
		$(LINT.c) $(less_OBJS:%.o=%.c) -L/usr/xpg4/lib -lcurses

lesskey_LINT:
		$(LINT.c) $(lesskey_OBJS:%.o=%.c)

$(ROOTLINKS):	$(ROOTBIN)/less
		$(INS.link)

$(ROOTXPG4PROG):
		$(INS.symlink)
		
include		$(SRC)/cmd/Makefile.targ
