/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file coverage.c
 * @brief Is an interface tested: a unit of its own, or defs.c?
 * @Created: 2026/11/24 08:17:42 by Moutig
 * @Updated: 2026/11/24 08:23:48 by Moutig
 *
 * A function has a file of its own in a group of its category; a type or
 * a constant has its LC_REQ_<name> guard in <category>/defs.c or in a
 * bits/ file it includes. Used by --spec-check, --spec-matrix and
 * --spec-todo.
 */

#include "runner/lcrun.h"

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * @brief Append @p s to the heap string @p text.
 *
 * @param text Heap string (reallocated).
 * @param s    String to add.
 * @return The new string.
 */
static char	*appendText(char *text, const char *s)
{
	size_t	a = strlen(text);
	size_t	b = strlen(s);

	text = xrealloc(text, a + b + 1);
	memcpy(text + a, s, b + 1);
	return (text);
}

/**
 * @brief Text of <cat>/defs.c and of every bits/ file it includes.
 *
 * Cached per category (the matrix asks for every interface).
 *
 * @param dir Suites directory.
 * @param cat Category.
 * @return The text (owned by the cache), "" when there is no defs.c.
 */
static const char	*defsText(const char *dir, const char *cat)
{
	static struct { char *cat; char *text; }	cache[64];
	static int									n;
	char										path[4096];
	char										line[1024];
	char										*text = NULL;
	FILE										*fp;

	for (int i = 0; i < n; i++)
		if (!strcmp(cache[i].cat, cat))
			return (cache[i].text);
	snprintf(path, sizeof(path), "%s/%s/defs.c", dir, cat);
	fp = fopen(path, "r");
	text = xstrdup("");
	while (fp && fgets(line, sizeof(line), fp))
	{
		char	inc[512];
		FILE	*bp;
		char	bl[1024];

		text = appendText(text, line);
		if (sscanf(line, " #include \"bits/%500[^\"]\"", inc) != 1)
			continue ;
		snprintf(path, sizeof(path), "%s/bits/%s", dir, inc);
		bp = fopen(path, "r");
		while (bp && fgets(bl, sizeof(bl), bp))
			text = appendText(text, bl);
		if (bp)
			fclose(bp);
	}
	if (fp)
		fclose(fp);
	if (n < 64)
	{
		cache[n].cat = xstrdup(cat);
		cache[n++].text = text;
	}
	return (text);
}

int	defsCovers(const t_run *r, const t_entry *e)
{
	char		tok[300];
	const char	*text = defsText(r->o.suitesDir, e->cat);
	size_t		len;

	snprintf(tok, sizeof(tok), "LC_REQ_%s", e->name);
	len = strlen(tok);
	for (const char *p = strstr(text, tok); p; p = strstr(p + 1, tok))
		if (!isalnum((unsigned char)p[len]) && p[len] != '_')
			return (1);
	return (0);
}

int	ownUnit(const t_run *r, const t_entry *e)
{
	static t_src	*src;
	static long		n = -2;

	if (n == -2)
		n = suitesScan(r->o.suitesDir, &src);
	for (long i = 0; i < n; i++)
		if (!strcmp(src[i].cat, e->cat) && !strcmp(src[i].func, e->name))
			return (1);
	return (0);
}

int	hasUnit(const t_run *r, const t_entry *e)
{
	int	own = ownUnit(r, e);

	return (own || (specIsDef(e, own) && defsCovers(r, e)));
}
