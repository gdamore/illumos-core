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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "latencytop.h"
#include "lthash.h"

#define	HASH(h, key)	\
	((h->h_hash ? h->h_hash(key) : (uintptr_t)key) % h->h_nchains)

#define	MAXHASHLOAD	2	/* grow if nnodes * MAXHASHLOAD > nchains */

typedef struct hash_node *hash_node_t;

struct hash_node {
	void			*hn_key;
	void			*hn_val;
	struct hash_node	*hn_next;
};

struct lt_hash {
	hash_node_t		*h_chains;
	int			h_nchains;
	int			h_nnodes;
	int			(*h_compare)(const void *, const void *);
	unsigned		(*h_hash)(const void *);
	void			(*h_key_dtor)(void *);
	void			(*h_val_dtor)(void *);
};

static int nextprime(int);
static void hash_grow(lt_hash_t);

lt_hash_t
lt_hash_new(unsigned (*hash)(const void *),
    int (*compare)(const void *, const void *),
    void (*key_dtor)(void *), void (*val_dtor)(void *))
{
	lt_hash_t h;

	h = lt_malloc(sizeof (*h));
	h->h_nchains = 31;	/* a reasonable starting point */
	h->h_chains = lt_zalloc(sizeof (hash_node_t) * h->h_nchains);
	h->h_hash = hash;
	h->h_compare = compare;
	h->h_key_dtor = key_dtor;
	h->h_val_dtor = val_dtor;
	h->h_nnodes = 0;
	return (h);
}

void
lt_hash_free(lt_hash_t h)
{
	int i;
	for (i = 0; i < h->h_nchains; i++) {
		hash_node_t hn = h->h_chains[i];
		hash_node_t x;
		while ((x = hn) != NULL) {
			hn = hn->hn_next;
			if (h->h_key_dtor != NULL)
				h->h_key_dtor(x->hn_key);
			if (h->h_val_dtor != NULL)
				h->h_val_dtor(x->hn_val);
			free(x);
		}
	}
	free(h->h_chains);
	free(h);
}

void *
lt_hash_lookup(lt_hash_t h, const void *key)
{
	hash_node_t hn = h->h_chains[HASH(h, key)];
	while (hn != NULL) {
		if (h->h_compare(key, hn->hn_key)) {
			return (hn->hn_val);
		}
		hn = hn->hn_next;
	}
	return (NULL);
}

void
lt_hash_insert(lt_hash_t h, void *key, void *val)
{
	unsigned i;
	hash_node_t hn;

	hash_grow(h);

	i = HASH(h, key);
	hn = h->h_chains[i];

	while (hn != NULL) {
		if (!h->h_compare(key, hn->hn_key)) {
			hn = hn->hn_next;
			continue;
		}
		if (h->h_key_dtor)
			h->h_key_dtor(hn->hn_key);
		if (h->h_val_dtor)
			h->h_val_dtor(hn->hn_val);
		hn->hn_key = key;
		hn->hn_val = val;
		return;
	}

	hn = lt_zalloc(sizeof (*hn));
	hn->hn_next = h->h_chains[i];
	hn->hn_key = key;
	hn->hn_val = val;
	h->h_chains[i] = hn;
	h->h_nnodes++;
}

void
lt_hash_walk(lt_hash_t h, int (*walkfn)(void *, void *, void *), void *arg)
{
	int i;
	for (i = 0; i < h->h_nchains; i++) {
		hash_node_t *hnpp = &(h->h_chains[i]);
		hash_node_t hn;
		while ((hn = *hnpp) != NULL) {
			int rv = walkfn(hn->hn_key, hn->hn_val, arg);
			if (rv & LTH_WALK_REMOVE) {
				*hnpp = hn->hn_next;
				if (h->h_key_dtor)
					h->h_key_dtor(hn->hn_key);
				if (h->h_val_dtor)
					h->h_val_dtor(hn->hn_val);
				h->h_nnodes--;
				free(hn);
			} else {
				hnpp = &hn->hn_next;
			}
			if (rv & LTH_WALK_STOP)
				return;
		}
	}
}

int
lt_hash_size(lt_hash_t h)
{
	return (h->h_nnodes);
}

unsigned
lt_strhash(const void *v)
{
	const char *c;
	unsigned hash = 0;
	unsigned g;

	for (c = v; *c != '\0'; c++) {
		hash = (hash << 4) + *c;
		if ((g = (hash & 0xf0000000)) != 0) {
			hash ^= (g >> 24);
			hash ^= g;
		}
	}
	return (hash);
}

int
lt_strcmp(const void *a1, const void *a2)
{
	return (strcmp(a1, a2) == 0 ? 1 : 0);
}

unsigned
lt_defhash(const void *a1)
{
	return ((uintptr_t)a1 & 0xffffffff);
}

int
lt_defcmp(const void *a1, const void *a2)
{
	return (a1 == a2 ? 1 : 0);
}


static int
nextprime(int n)
{
	if (n < 3)
		return (3);

	if ((n % 2) == 0) {
		n++;
	} else {
		n += 2;
	}

	for (;;) {
		int i;
		int composite = 0;
		for (i = 3; i * i < n; i += 2) {
			if ((n % i) == 0) {
				composite = 1;
				break;
			}
		}
		if (!composite) {
			return (n);
		}
		n += 2;
	}
}

static void
hash_grow(lt_hash_t h)
{
	unsigned i;
	int oldnchain;
	hash_node_t *oldchains;

	oldnchain = h->h_nchains;
	oldchains = h->h_chains;

	if (h->h_nchains > (h->h_nnodes * MAXHASHLOAD))
		return;

	/* find the next prime number */
	do {
		h->h_nchains = nextprime(h->h_nchains * 2);
	} while (h->h_nchains < (h->h_nnodes * MAXHASHLOAD));

	h->h_chains = lt_zalloc(sizeof (hash_node_t) * h->h_nchains);

	/* rehash & insert */
	for (i = 0; i < oldnchain; i++) {
		hash_node_t hn;
		while ((hn = oldchains[i]) != NULL) {
			unsigned index = HASH(h, hn->hn_key);
			oldchains[i] = hn->hn_next;
			hn->hn_next = h->h_chains[index];
			h->h_chains[index] = hn;
		}
	}
	free(oldchains);
}
