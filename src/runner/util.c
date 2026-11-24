/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file util.c
 * @brief Small helpers of lcrun: allocation, strings, lists, time.
 * @Created: 2026/11/24 08:26:01 by Moutig
 * @Updated: 2026/11/24 08:26:20 by Moutig
 *
 * Allocation that exits on failure (status 2), growing strings, comma
 * lists and filters, a monotonic clock.
 */

#include "runner/lcrun.h"

#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void	*xmalloc(size_t n)
{
	void	*p = malloc(n ? n : 1);

	if (p == NULL)
	{
		perror("lcrun");
		exit(2);
	}
	return (p);
}

void	*xrealloc(void *p, size_t n)
{
	p = realloc(p, n ? n : 1);
	if (p == NULL)
	{
		perror("lcrun");
		exit(2);
	}
	return (p);
}

char	*xstrndup(const char *s, size_t n)
{
	char	*d = xmalloc(n + 1);

	memcpy(d, s, n);
	d[n] = '\0';
	return (d);
}

char	*xstrdup(const char *s)
{
	return (xstrndup(s, strlen(s)));
}

void	strAppend(char **dst, const char *s, size_t n)
{
	size_t	len = *dst ? strlen(*dst) : 0;

	*dst = xrealloc(*dst, len + n + 1);
	memcpy(*dst + len, s, n);
	(*dst)[len + n] = '\0';
}

double	nowMs(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((double)ts.tv_sec * 1e3 + (double)ts.tv_nsec / 1e6);
}

char	**splitList(const char *s)
{
	char	**v = NULL;
	size_t	n = 0;
	char	*copy = xstrdup(s);
	char	*save;

	for (char *tok = strtok_r(copy, ",", &save); tok; tok = strtok_r(NULL, ",", &save))
	{
		v = xrealloc(v, (n + 2) * sizeof(*v));
		v[n++] = xstrdup(tok);
	}
	if (v)
		v[n] = NULL;
	free(copy);
	return (v);
}

int	inList(char **list, const char *s)
{
	if (list == NULL)
		return (1);
	for (; *list; list++)
		if (strcmp(*list, s) == 0)
			return (1);
	return (0);
}
