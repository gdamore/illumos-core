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
 * Simple key/value store hash table.
 */

#ifndef LTHTABLE_H
#define	LTHTABLE_H

/* Return codes for lt_hash_walk().  Note that these may be combined. */
#define	LTH_WALK_CONTINUE	0
#define	LTH_WALK_STOP		1
#define	LTH_WALK_REMOVE		2

typedef struct lt_hash *lt_hash_t;

lt_hash_t lt_hash_new(unsigned (*)(const void *),
    int (*)(const void *, const void *),
    void (*)(void *),
    void (*)(void *));
void lt_hash_free(lt_hash_t);
void *lt_hash_lookup(lt_hash_t, const void *);
int lt_hash_remove(lt_hash_t, const void *);
void lt_hash_insert(lt_hash_t, void *, void *);
void lt_hash_walk(lt_hash_t, int (*)(void *, void *, void *), void *);
int lt_hash_size(lt_hash_t);
unsigned lt_strhash(const void *);
int lt_strcmp(const void *, const void *);
unsigned lt_defhash(const void *);
int lt_defcmp(const void *, const void *);

#endif	/* LTHTABLE_H */
