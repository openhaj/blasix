/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file check.c
 * @brief lcrun --spec-check: lint of spec/ and src/suites/.
 * @Created: 2026/11/24 08:17:23 by Moutig
 * @Updated: 2026/11/24 08:23:21 by Moutig
 *
 * Spec lines (versions, tags), one file per function, definitions covered
 * by defs.c, order of the bits/ files of a defs.c.
 */

#include "runner/lcrun.h"

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * @brief Is @p since an ISO C edition?
 *
 * @param since Version.
 * @return 1 for C99 ... C23, 0 otherwise.
 */
static int	isCSince(const char *since)
{
	const t_std	*s = stdFind(since);

	return (s && s->posix == 0);
}

/**
 * @brief Lint one line: base= needs an option, no POSIX option on an ISO
 *        C line, same= lists other headers.
 *
 * @param e Line.
 * @return Number of errors (reported on stderr).
 */
static int	checkLine(const t_entry *e)
{
	const t_optdef	*d = e->opt ? optFind(e->opt) : NULL;
	int				errors = 0;

	if (e->base && !e->opt)
	{
		fprintf(stderr, "%s:%d: %s: base= without an option group\n",
			e->file, e->line, e->name);
		errors++;
	}
	if (d && d->from == NULL && isCSince(e->since))
	{
		fprintf(stderr, "%s:%d: %s: POSIX option %s on an ISO C line\n",
			e->file, e->line, e->name, e->opt);
		errors++;
	}
	if (e->same && (strstr(e->same, ".h") == NULL || strstr(e->same, e->header)))
	{
		fprintf(stderr, "%s:%d: %s: same= must list other headers (x.h,y.h)\n",
			e->file, e->line, e->name);
		errors++;
	}
	return (errors);
}

/**
 * @brief Does the source @p path contain @p needle?
 *
 * @param path   File.
 * @param needle Text.
 * @return 1 if found, 0 otherwise (or unreadable).
 */
static int	fileHas(const char *path, const char *needle)
{
	FILE	*fp = fopen(path, "r");
	char	line[1024];
	int		found = 0;

	while (fp && !found && fgets(line, sizeof(line), fp))
		found = strstr(line, needle) != NULL;
	if (fp)
		fclose(fp);
	return (found);
}

/**
 * @brief Check the order of the bits/ files a defs.c includes.
 *
 * A bits/ file that includes another header of the libc (marked
 * BITS_INCLUDES_OTHER_HEADERS) may make visible what the header under
 * test must define itself: such files come after every other one.
 *
 * @param dir  Suites directory.
 * @param path The defs.c file.
 * @return Number of errors (reported on stderr).
 */
static int	checkDefsOrder(const char *dir, const char *path)
{
	FILE	*fp = fopen(path, "r");
	char	line[1024];
	char	impure[512] = "";
	int		errors = 0;

	while (fp && fgets(line, sizeof(line), fp))
	{
		char	inc[512];
		char	bpath[4096];

		if (sscanf(line, " #include \"bits/%500[^\"]\"", inc) != 1)
			continue ;
		snprintf(bpath, sizeof(bpath), "%s/bits/%s", dir, inc);
		if (fileHas(bpath, "BITS_INCLUDES_OTHER_HEADERS"))
		{
			if (!impure[0])
				snprintf(impure, sizeof(impure), "%s", inc);
		}
		else if (impure[0])
		{
			fprintf(stderr, "%s: bits/%s after bits/%s, which includes other headers: "
				"move it before\n", path, inc, impure);
			errors++;
		}
	}
	if (fp)
		fclose(fp);
	return (errors);
}

int	toolCheck(const t_run *r)
{
	t_src		*src;
	long		n = suitesScan(r->o.suitesDir, &src);
	int			errors = r->spec.errors;
	char		path[4096];

	if (n < 0)
	{
		perror(r->o.suitesDir);
		return (2);
	}
	for (long i = 0; i < n; i++)
	{
		snprintf(path, sizeof(path), "%s/%s.c", r->o.suitesDir, src[i].rel);
		if (isDefsUnit(&src[i]))
		{
			errors += checkDefsOrder(r->o.suitesDir, path);
			continue ;
		}
		if (specFind(&r->spec, src[i].cat, src[i].func) == NULL)
		{
			fprintf(stderr, "%s: no entry `%s` in %s/%s.spec\n", path, src[i].func,
				r->o.specDir, src[i].cat);
			errors++;
		}
		/* one file per interface, whatever the group */
		for (long k = 0; k < i; k++)
			if (!strcmp(src[k].cat, src[i].cat) && !strcmp(src[k].func, src[i].func))
			{
				fprintf(stderr, "%s: `%s` already has a unit: %s/%s.c\n", path,
					src[i].func, r->o.suitesDir, src[k].rel);
				errors++;
			}
	}
	for (size_t i = 0; i < r->spec.n; i++)
	{
		const t_entry	*e = &r->spec.v[i];
		int				own = ownUnit(r, e);

		errors += checkLine(e);
		if (e->alt)
			errors += checkLine(e->alt);
		/* types and constants: cases in bits/, run by <cat>/defs.c */
		if (e->kind == KIND_TYPE && own)
		{
			fprintf(stderr, "%s:%d: %s is a type: its cases go in src/suites/bits/, "
				"included by %s/%s/defs.c, not in a unit of its own\n", e->file, e->line,
				e->name, r->o.suitesDir, e->cat);
			errors++;
		}
		else if (specIsDef(e, own) && !defsCovers(r, e))
		{
			fprintf(stderr, "%s:%d: %s: no case in %s/%s/defs.c (LC_REQ_%s guard, "
				"directly or in a bits/ file it includes)\n", e->file, e->line, e->name,
				r->o.suitesDir, e->cat, e->name);
			errors++;
		}
	}
	printf("%zu interfaces, %ld units, %d error%s\n", r->spec.n, n, errors,
		errors == 1 ? "" : "s");
	suitesFree(src, n);
	return (errors ? 1 : 0);
}
