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
 * Copyright 2014 Garrett D'Amore <garrett@amore.org>
 */

/*
 * Simple list / sequence support.  The linkage structures are managed
 * by this library, so consumers don't have to alter their data structures.
 * Support for customizable destructors is added.
 */

#ifndef	LTLIST_H
#define	LTLIST_H

typedef struct lt_list_node lt_list_node_t;
typedef struct lt_list lt_list_t;

lt_list_t *lt_list_new(void (*)(void *));
void lt_list_free(lt_list_t *);
void lt_list_insert_before_full(lt_list_node_t *, void *, void (*)(void *));
void lt_list_insert_before(lt_list_node_t *, void *);
void lt_list_append_full(lt_list_t *, void *, void (*)(void *));
void lt_list_append(lt_list_t *, void *);
void lt_list_node_remove(lt_list_node_t *);
void lt_list_walk(lt_list_t *, int (*)(void *, lt_list_node_t *, void *),
	void *);


#endif	/* LTLIST_H */
