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

#include <stdlib.h>
#include <stdio.h>
#include "latencytop.h"
#include "ltlist.h"

struct lt_list_node {
	void			*lt_data;
	struct lt_list_node	*lt_next;
	struct lt_list_node	*lt_prev;
	void			(*lt_dtor)(void *);
	struct lt_list		*lt_list;
};

struct lt_list {
	struct lt_list_node	l_nodes;
};

lt_list_t *
lt_list_new(void (*dtor)(void *))
{
	lt_list_t *list;

	list = lt_malloc(sizeof (*list));
	list->l_nodes.lt_data = NULL;
	list->l_nodes.lt_next = &list->l_nodes;
	list->l_nodes.lt_prev = &list->l_nodes;
	list->l_nodes.lt_dtor = dtor;
	list->l_nodes.lt_list = list;
	return (list);
}

void
lt_list_free(lt_list_t *list)
{
	lt_list_node_t *node;

	while ((node = list->l_nodes.lt_next) != &list->l_nodes) {
		lt_list_node_remove(node);
	}
	free(list);
}


void
lt_list_insert_before_full(lt_list_node_t *n, void *item, void (*dtor)(void *))
{
	lt_list_node_t *node;

	node = lt_zalloc(sizeof (*node));
	node->lt_list = n->lt_list;
	node->lt_data = item;
	node->lt_dtor = dtor;
	node->lt_prev = n->lt_prev;
	node->lt_next = n;
	node->lt_prev->lt_next = node;
	node->lt_next->lt_prev = node;
}

void
lt_list_append_full(lt_list_t *l, void *item, void (*dtor)(void *))
{
	lt_list_insert_before_full(&l->l_nodes, item, dtor);
}

void
lt_list_append(lt_list_t *l, void *item)
{
	lt_list_append_full(l, item, l->l_nodes.lt_dtor);
}

void
lt_list_node_remove(lt_list_node_t *n)
{
	n->lt_next = n->lt_prev;
	n->lt_prev = n->lt_next;
	if (n->lt_dtor)
		n->lt_dtor(n->lt_data);
	free(n);
}

void
lt_list_walk(lt_list_t *l, int (*fn)(void *, lt_list_node_t *, void *),
    void *arg)
{
	lt_list_node_t *n = l->l_nodes.lt_next;
	lt_list_node_t *x;
	while ((x = n) != &l->l_nodes) {
		n = n->lt_next;
		if (fn(x->lt_data, x, arg) < 0) {
			break;
		}
	}
}
