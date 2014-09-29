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
 * Copyright (c) 2008-2009, Intel Corporation.
 * All Rights Reserved.
 *
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 */

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <note.h>

#include "latencytop.h"
#include "ltlist.h"
#include "lthash.h"

/*
 * Structure that holds detail of a cause.
 */
typedef struct {
	int lt_c_cause_id;
	int lt_c_flags;
	char *lt_c_name;
} lt_cause_t;

/*
 * Structure that represents a matched cause.
 */
typedef struct  {
	int lt_mt_priority;
	int lt_mt_cause_id;
} lt_match_t;

/* All lt_cause_t that are created. */
static lt_hash_t cause_lookup = NULL;
static lt_hash_t causes_array = NULL;
static int causes_array_len = 0;

/*
 * This hash table maps a symbol to a cause.
 * key is of type "char *" and value is of type "lt_match_t *".
 */
static lt_hash_t symbol_lookup_table = NULL;

/*
 * The dtrace translation rules we get from the script
 */
char *dtrans = NULL;

/*
 * These structures are only used inside .trans parser.
 */
typedef struct {
	int lt_dm_priority;
	char *lt_dm_macro;
} lt_dmacro_t;

typedef struct {
	lt_list_t *lt_pr_cmd_disable;
	lt_hash_t lt_pr_dmacro;
} lt_parser_t;

static void
free_cause(void *v)
{
	lt_cause_t *cause = v;
	assert(cause != NULL && cause->lt_c_name != NULL);

	free(cause->lt_c_name);
	free(cause);
}

static void
free_dmacro(void *a)
{
	lt_dmacro_t *d = a;
	assert(d->lt_dm_macro != NULL);
	free(d->lt_dm_macro);
	free(d);
}

/*
 * Add a cause.
 */
static lt_cause_t *
new_cause(char *name, int flags)
{
	lt_cause_t *entry;

	assert(name != NULL);

	entry = (lt_cause_t *)lt_malloc(sizeof (lt_cause_t));
	entry->lt_c_flags = flags;
	entry->lt_c_name = name;
	entry->lt_c_cause_id = causes_array_len;

	lt_hash_insert(causes_array, LT_INT_TO_POINTER(causes_array_len),
	    entry);
	causes_array_len++;

	return (entry);
}

/*
 * Set a cause to "disabled" state.
 */
static int
disable_cause(void *data, lt_list_node_t *node, void *arg)
{
	char *cause_str = data;
	lt_hash_t cause_table = arg;
	lt_cause_t *cause;
	NOTE(ARGUNUSED(node));

	if ((cause = lt_hash_lookup(cause_table, cause_str)) != NULL) {
		cause->lt_c_flags |= CAUSE_FLAG_DISABLED;
	}
	return (0);
}

/*
 * Helper functions that reads a line from a character array.
 */
static int
read_line_from_mem(const char *mem, int mem_len, char *line, int line_len,
    int *index)
{
	assert(mem != NULL && line != NULL && index != NULL);

	if (line_len <= 0 || mem_len <= 0) {
		return (0);
	}

	if (*index >= mem_len) {
		return (0);
	}

	while (line_len > 1 && *index < mem_len) {
		*line = mem[(*index)++];
		--line_len;
		++line;

		if (*(line-1) == '\r' || *(line-1) == '\n') {
			break;
		}
	}
	*line = '\0';

	return (1);
}

/*
 * Parse special command from configuration file. Special command
 * has the following format :
 *
 *	disable_cause <cause name>
 */
static int
parse_config_cmd(char *begin, lt_parser_t *parser)
{
	char *tmp;
	char old_chr = 0;

	/*
	 * disable_cause  FSFlush Daemon
	 * ^
	 */
	if (*begin == '\0') {
		return (0);
	}

	for (tmp = begin;
	    *tmp != '\0' && !isspace(*tmp);
	    ++tmp) {
	}
	old_chr = *tmp;
	*tmp = '\0';

	if (strcmp("disable_cause", begin) == 0) {
		if (old_chr == '\0') {
			/* Must have an argument */
			lt_display_error(
			    "Invalid command format: %s\n",
			    begin);
			return (-1);
		}

		begin = tmp+1;
		while (isspace(*begin)) {
			++begin;
		}

		lt_list_append(parser->lt_pr_cmd_disable, lt_strdup(begin));
	} else   {
		*tmp = old_chr;
		lt_display_error(
		    "Unknown command: %s\n", begin);
		return (-1);
	}

	return (0);
}

/*
 * Parse symbol translation from configuration file. Symbol translation
 * has the following format :
 *
 *	<priority> <symbol name> <cause>
 *
 * Finally check if that cause has already been mapped.
 */
static int
parse_sym_trans(char *begin)
{
	int priority = 0;
	char *match;
	char *match_dup;
	char *cause_str;
	lt_cause_t *cause;
	lt_match_t *match_entry;
	char *tmp;

	/*
	 * 10	genunix`pread			Syscall pread
	 * ^
	 */
	priority = strtol(begin, &tmp, 10);

	if (tmp == begin || priority == 0) {
		return (-1);
	}

	begin = tmp;

	/*
	 * 10	genunix`pread			Syscall pread
	 * --^
	 */

	if (!isspace(*begin)) {
		/* At least one space char after <priority> */
		return (-1);
	}

	while (isspace(*begin)) {
		++begin;
	}

	if (*begin == 0) {
		return (-1);
	}

	/*
	 * 10	genunix`pread			Syscall pread
	 * -----^
	 */
	for (tmp = begin;
	    *tmp != '\0' && !isspace(*tmp);
	    ++tmp) {
	}

	if (*tmp == '\0') {
		return (-1);
	}

	*tmp = '\0';
	match = begin;

	/* Check if we have mapped this function before. */
	match_entry = lt_hash_lookup(symbol_lookup_table, match);
	if ((match_entry != NULL) &&
	    HIGHER_PRIORITY(match_entry->lt_mt_priority, priority)) {
		/* We already have a higher entry. Ignore this. */
		return (0);
	}

	begin = tmp + 1;

	/*
	 * 10	genunix`pread			Syscall pread
	 * -------------------------------------^
	 */
	while (isspace(*begin)) {
		++begin;
	}

	if (*begin == 0) {
		return (-1);
	}

	cause_str = begin;

	/* Check if we have mapped this cause before. */
	if ((cause = lt_hash_lookup(cause_lookup, cause_str)) == NULL) {
		char *cause_dup = lt_strdup(cause_str);
		cause = new_cause(cause_dup, 0);
		lt_hash_insert(cause_lookup, cause_dup, cause);
	}

	match_entry = (lt_match_t *)lt_malloc(sizeof (lt_match_t));
	match_entry->lt_mt_priority = priority;
	match_entry->lt_mt_cause_id = cause->lt_c_cause_id;
	match_dup = lt_strdup(match);

	lt_hash_insert(symbol_lookup_table, match_dup, match_entry);

	return (0);
}

/*
 * Parse D macro. D macros have the following format :
 *
 *	<priority> <entry probe> <return probe> <cause>
 *
 * Finally check if that cause has already been mapped.
 */
static int
parse_dmacro(char *begin, lt_parser_t *parser)
{
	int priority = 0;
	char *entryprobe;
	char *returnprobe;
	char *cause_str;
	char buf[512];
	char probepair[512];
	char *tmp = NULL;
	lt_cause_t *cause;
	lt_dmacro_t *dmacro;

	/*
	 * 10	syscall::pread:entry	syscall::pread:return	Syscall pread
	 * ^
	 */
	priority = strtol(begin, &tmp, 10);

	if (tmp == begin || priority == 0) {
		return (-1);
	}

	begin = tmp;

	/*
	 * 10	syscall::pread:entry	syscall::pread:return	Syscall pread
	 * --^
	 */
	while (isspace(*begin)) {
		++begin;
	}

	if (*begin == 0) {
		return (-1);
	}

	/*
	 * 10	syscall::pread:entry	syscall::pread:return	Syscall pread
	 * -----^
	 */
	for (tmp = begin;
	    *tmp != '\0' && !isspace(*tmp);
	    ++tmp) {
	}

	if (*tmp == '\0') {
		return (-1);
	}

	*tmp = '\0';
	entryprobe = begin;
	begin = tmp + 1;

	while (isspace(*begin)) {
		++begin;
	}

	/*
	 * 10	syscall::pread:entry	syscall::pread:return	Syscall pread
	 * -----------------------------^
	 */
	for (tmp = begin;
	    *tmp != '\0' && !isspace(*tmp);
	    ++tmp) {
	}

	if (*tmp == '\0') {
		return (-1);
	}

	*tmp = '\0';
	returnprobe = begin;
	begin = tmp + 1;

	while (isspace(*begin)) {
		++begin;
	}

	/*
	 * 10	syscall::pread:entry	syscall::pread:return	Syscall pread
	 * -----------------------------------------------------^
	 */
	if (*begin == 0) {
		return (-1);
	}

	cause_str = begin;

	dmacro = NULL;

	/* Check if we have mapped this cause before. */
	if ((cause = lt_hash_lookup(cause_lookup, cause_str)) == NULL) {
		char *cause_dup = lt_strdup(cause_str);
		cause = new_cause(cause_dup, 0);
		lt_hash_insert(cause_lookup, cause_dup, cause);
	}

	(void) snprintf(buf, sizeof (buf), "\nTRANSLATE(%s, %s, \"%s\", %d)\n",
	    entryprobe, returnprobe, cause_str, priority);

	(void) snprintf(probepair, sizeof (probepair), "%s %s", entryprobe,
	    returnprobe);

	assert(cause != NULL);
	assert(parser->lt_pr_dmacro != NULL);

	dmacro = lt_hash_lookup(parser->lt_pr_dmacro, probepair);
	if (dmacro == NULL) {
		dmacro = (lt_dmacro_t *)lt_malloc(sizeof (lt_dmacro_t));
		dmacro->lt_dm_priority = priority;
		dmacro->lt_dm_macro = lt_strdup(buf);
		lt_hash_insert(parser->lt_pr_dmacro,
		    lt_strdup(probepair), dmacro);
	} else if (dmacro->lt_dm_priority < priority) {
		free(dmacro->lt_dm_macro);
		dmacro->lt_dm_priority = priority;
		dmacro->lt_dm_macro = lt_strdup(buf);
	}

	return (0);
}

/*
 * Helper function to collect TRANSLATE() macros.
 */
/* ARGSUSED */
static int
genscript(void *key, void *val, void *arg)
{
	lt_dmacro_t *dmacro = val;
	char *str;
	char **sp = arg;

	(void) asprintf(&str, "%s%s", *sp, dmacro->lt_dm_macro);
	lt_check_null(str);
	free(*sp);
	*sp = str;
	return (LTH_WALK_CONTINUE);
}

/*
 * Main logic that parses translation rules one line at a time,
 * and creates a lookup table from it. The syntax for the translation
 * is as follows :
 *
 *	#				<--- comment
 *	D <D macro rule>		<--- D macro
 *	S <Symbol translation>		<--- Symbols
 *	disable_cause <cause>		<--- special command
 */
static int
parse_config(const char *work, int work_len)
{
	char line[256];
	int len;
	char *begin, *end;
	int current = 0;
	lt_parser_t parser;
	int ret = 0;
	char flag;

	cause_lookup = lt_hash_new(lt_strhash, lt_strcmp, free, free);
	lt_check_null(cause_lookup);

	parser.lt_pr_cmd_disable = lt_list_new(free);
	lt_check_null(parser.lt_pr_cmd_disable);

	parser.lt_pr_dmacro = lt_hash_new(lt_strhash, lt_strcmp,
	    free, free_dmacro);
	lt_check_null(parser.lt_pr_dmacro);

	while (read_line_from_mem(work, work_len, line, sizeof (line),
	    &current)) {
		len = strlen(line);

		if (line[len-1] != '\n' && line[len-1] != '\r' &&
		    current < work_len) {
			lt_display_error("Configuration line too long.\n");
			goto err;
		}

		begin = line;

		while (isspace(*begin)) {
			++begin;
		}

		if (*begin == '\0') {
			/* Ignore empty line */
			continue;
		}

		/* Delete trailing spaces. */
		end = begin + strlen(begin) - 1;

		while (isspace(*end)) {
			--end;
		}

		end[1] = '\0';

		flag = *begin;
		++begin;

		switch (flag) {
		case '#':
			ret = 0;
			break;
		case ';':
			ret = parse_config_cmd(begin, &parser);
			break;
		case 'D':
		case 'd':
			if (!isspace(*begin)) {
				lt_display_error(
				    "No space after flag char: %s\n", line);
			}
			while (isspace(*begin)) {
				++begin;
			}
			ret = parse_dmacro(begin, &parser);
			break;
		case 'S':
		case 's':
			if (!isspace(*begin)) {
				lt_display_error(
				    "No space after flag char: %s\n", line);
			}
			while (isspace(*begin)) {
				++begin;
			}
			ret = parse_sym_trans(begin);
			break;
		default:
			ret = -1;
			break;
		}

		if (ret != 0) {
			lt_display_error(
			    "Invalid configuration line: %s\n", line);
			goto err;
		}
	}

	dtrans = lt_strdup("");
	lt_hash_walk(parser.lt_pr_dmacro, genscript, &dtrans);

	if (dtrans != NULL && strlen(dtrans) == 0) {
		free(dtrans);
		dtrans = NULL;
	}

	lt_list_walk(parser.lt_pr_cmd_disable, disable_cause, cause_lookup);
	lt_list_free(parser.lt_pr_cmd_disable);

	return (0);

err:
	lt_list_free(parser.lt_pr_cmd_disable);
	lt_hash_free(parser.lt_pr_dmacro);
	return (-1);

}

/*
 * Init function, called when latencytop starts.
 * It loads translation rules from the configuration file. The configuration
 * file defines some causes and symbols that match those causes.
 */
int
lt_table_init(void)
{
	char *config_loaded = NULL;
	int config_loaded_len = 0;
	const char *work = NULL;
	int work_len = 0;
	lt_cause_t *cause;

#ifdef EMBED_CONFIGS
	work = &latencytop_trans_start;
	work_len = (int)(&latencytop_trans_end - &latencytop_trans_start);
#endif

	if (g_config.lt_cfg_config_name != NULL) {
		FILE *fp;
		fp = fopen(g_config.lt_cfg_config_name, "r");

		if (NULL == fp) {
			lt_display_error(
			    "Unable to open configuration file.\n");
			return (-1);
		}

		(void) fseek(fp, 0, SEEK_END);
		config_loaded_len = (int)ftell(fp);
		config_loaded = (char *)lt_malloc(config_loaded_len);
		(void) fseek(fp, 0, SEEK_SET);

		/* A zero-byte translation is valid */
		if (config_loaded_len != 0 &&
		    fread(config_loaded, config_loaded_len, 1, fp) == 0) {
			lt_display_error(
			    "Unable to read configuration file.\n");
			(void) fclose(fp);
			free(config_loaded);
			return (-1);
		}

		(void) fclose(fp);
		(void) printf("Loaded configuration from %s\n",
		    g_config.lt_cfg_config_name);

		work = config_loaded;
		work_len = config_loaded_len;
	}

	lt_table_deinit();
	causes_array = lt_hash_new(lt_defhash, lt_defcmp, NULL, free_cause);
	lt_check_null(causes_array);

	/* 0 is not used, but it is kept as a place for bugs etc. */
	cause = new_cause(lt_strdup("Nothing"), CAUSE_FLAG_DISABLED);
	assert(cause->lt_c_cause_id == INVALID_CAUSE);

	symbol_lookup_table = lt_hash_new(lt_strhash, lt_strcmp, free, free);
	lt_check_null(symbol_lookup_table);

	if (work_len != 0 && parse_config(work, work_len) != 0) {
		return (-1);
	}

	if (config_loaded != NULL) {
		free(config_loaded);
	}

	return (0);
}

/*
 * Some causes, such as "lock spinning", do not have stack trace. Names
 * of such causes are explicitly specified in the D script.
 * This function resolves such causes and dynamically adds them
 * to the global tables when they are found first. If auto_create is set
 * to TRUE, the entry will be created if it is not found.
 * Return cause_id of the cause.
 */
int
lt_table_cause_from_name(char *name, int auto_create, int flags)
{
	lt_cause_t *cause = NULL;

	if (cause_lookup == NULL) {
		cause_lookup = lt_hash_new(lt_strhash, lt_strcmp, free, free);
		lt_check_null(cause_lookup);
	} else   {
		cause = lt_hash_lookup(cause_lookup, name);
	}

	if (cause == NULL && auto_create) {
		char *cause_dup;

		if (name[0] == '#') {
			flags |= CAUSE_FLAG_HIDE_IN_SUMMARY;
		}

		cause_dup = lt_strdup(name);
		cause = new_cause(cause_dup, flags);
		lt_hash_insert(cause_lookup, cause_dup, cause);
	}

	return (cause == NULL ? INVALID_CAUSE : cause->lt_c_cause_id);
}

/*
 * Try to map a symbol on stack to a known cause.
 * module_func has the format "module_name`function_name".
 * cause_id and priority will be set if a cause is found.
 * If cause is found return 1, otherwise return 0.
 */
int
lt_table_cause_from_stack(const char *module_func, int *cause_id, int *priority)
{
	lt_match_t *match = NULL;

	assert(module_func != NULL && cause_id != NULL && priority != NULL);

	if (symbol_lookup_table == NULL) {
		return (0);
	}

	match = lt_hash_lookup(symbol_lookup_table, module_func);
	if (match == NULL) {

		char *func = strchr(module_func, '`');

		if (func != NULL) {
			match = lt_hash_lookup(symbol_lookup_table, func);
		}
	}

	if (match == NULL) {
		return (0);
	} else   {
		*cause_id = match->lt_mt_cause_id;
		*priority = match->lt_mt_priority;
		return (1);
	}
}

/*
 * Get the display name of a cause. cause_id must be valid,
 * it is usually returned from lt_table_cause_from_stack() or
 * lt_table_cause_from_name().
 */
const char *
lt_table_get_cause_name(int cause_id)
{
	lt_cause_t *cause = NULL;

	cause = lt_hash_lookup(causes_array, LT_INT_TO_POINTER(cause_id));
	if (cause == NULL) {
		return (NULL);
	} else {
		return (cause->lt_c_name);
	}
}

/*
 * Check cause flag.
 * If CAUSE_ALL_FLAGS is passed in, all flags are returned.
 */
int
lt_table_get_cause_flag(int cause_id, int flag)
{
	lt_cause_t *cause;

	cause = lt_hash_lookup(causes_array, LT_INT_TO_POINTER(cause_id));
	if (cause == NULL) {
		return (0);
	} else {
		return (cause->lt_c_flags & flag);
	}
}

/*
 * Append macros to D script, if any.
 */
int
lt_table_append_trans(FILE *fp)
{
	if (dtrans != NULL) {
		if (fwrite(dtrans, strlen(dtrans), 1, fp) != 1) {
			return (-1);
		}
	}

	return (0);
}

/*
 * Clean up function.
 * Free the resources used for symbol table (symbols, causes etc.).
 */
void
lt_table_deinit(void)
{
	if (symbol_lookup_table != NULL) {
		lt_hash_free(symbol_lookup_table);
		symbol_lookup_table = NULL;
	}

	if (cause_lookup != NULL) {
		lt_hash_free(cause_lookup);
		cause_lookup = NULL;
	}

	if (causes_array != NULL) {
		lt_hash_free(causes_array);
		causes_array = NULL;
	}

	if (dtrans != NULL) {
		free(dtrans);
		dtrans = NULL;
	}
}
