/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file compare.c
 * @brief Comparison of two TSV results (--compare).
 * @Created: 2026/11/24 08:14:07 by Moutig
 * @Updated: 2026/11/24 08:19:17 by Moutig
 *
 * Rows are matched by key (cat/func/test); a status that went from pass to
 * something else is WORSE, the reverse BETTER.
 */

#include "runner/lcrun.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** One row of a TSV result, for --compare. */
typedef struct s_row
{
	char	*key;	/* "cat/func/test" (test "@interface" for a probe row). */
	char	*st;	/* Status (test) or probe result (interface). */
}	t_row;

/**
 * @brief qsort() order of TSV rows: by key.
 *
 * @param a First row.
 * @param b Second row.
 * @return Negative, zero or positive.
 */
static int	cmpRow(const void *a, const void *b)
{
	return (strcmp(((const t_row *)a)->key, ((const t_row *)b)->key));
}

/**
 * @brief Read a TSV result (reportTsv()) into sorted rows.
 *
 * Exits with status 2 if the file cannot be opened.
 *
 * @param path TSV file.
 * @param n    Set to the number of rows.
 * @return New array of rows, NULL if the file is empty.
 */
static t_row	*loadTsv(const char *path, size_t *n)
{
	FILE	*fp = fopen(path, "r");
	char	line[4096];
	t_row	*v = NULL;

	*n = 0;
	if (fp == NULL)
	{
		perror(path);
		exit(2);
	}
	if (fgets(line, sizeof(line), fp) == NULL)
	{
		fclose(fp);
		return (NULL);
	}
	while (fgets(line, sizeof(line), fp))
	{
		char	*f[4];
		char	*save;
		char	key[1024];

		f[0] = strtok_r(line, "\t\n", &save);
		f[1] = strtok_r(NULL, "\t\n", &save);
		f[2] = strtok_r(NULL, "\t\n", &save);
		f[3] = strtok_r(NULL, "\t\n", &save);
		if (!f[3])
			continue ;
		snprintf(key, sizeof(key), "%s/%s/%s", f[0], f[1], f[2]);
		v = xrealloc(v, (*n + 1) * sizeof(*v));
		v[*n].key = xstrdup(key);
		v[*n].st = xstrdup(f[3]);
		(*n)++;
	}
	fclose(fp);
	qsort(v, *n, sizeof(*v), cmpRow);
	return (v);
}

int	compareTsv(const char *pa, const char *pb, int color)
{
	size_t	na;
	size_t	nb;
	t_row	*a;
	t_row	*b;
	size_t	i = 0;
	size_t	j = 0;
	int		worse = 0;
	int		better = 0;
	int		other = 0;
	int		same = 0;

	gColor = color;
	a = loadTsv(pa, &na);
	b = loadTsv(pb, &nb);
	printf("%s=== %s -> %s ===%s\n", C_BOLD, pa, pb, C_RESET);
	while (i < na || j < nb)
	{
		int	c = (i >= na) ? 1 : (j >= nb) ? -1 : strcmp(a[i].key, b[j].key);

		if (c < 0)
		{
			printf("  %sonly in A%s  %-50s %s\n", C_DIM, C_RESET, a[i].key, a[i].st);
			i++;
		}
		else if (c > 0)
		{
			printf("  %sonly in B%s  %-50s %s\n", C_DIM, C_RESET, b[j].key, b[j].st);
			j++;
		}
		else
		{
			if (strcmp(a[i].st, b[j].st))
			{
				int	wasOk = !strcmp(a[i].st, "pass");
				int	isOk = !strcmp(b[j].st, "pass");

				if (wasOk && !isOk)
					worse++;
				else if (!wasOk && isOk)
					better++;
				else
					other++;
				printf("  %s%-9s%s  %-50s %s -> %s\n",
					(wasOk && !isOk) ? C_RED : (isOk ? C_GREEN : C_YEL),
					(wasOk && !isOk) ? "WORSE" : (isOk ? "BETTER" : "CHANGED"),
					C_RESET, a[i].key, a[i].st, b[j].st);
			}
			else
				same++;
			i++;
			j++;
		}
	}
	printf("\n%d same, %s%d better%s, %s%d worse%s, %d changed\n",
		same, C_GREEN, better, C_RESET, C_RED, worse, C_RESET, other);
	return (worse != 0);
}
