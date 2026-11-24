/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file matrix.c
 * @brief lcrun --spec-matrix and --spec-todo: progress of the suites.
 * @Created: 2026/11/24 08:18:35 by Moutig
 * @Updated: 2026/11/24 08:24:24 by Moutig
 *
 * Per header, tested / required in each of the 8 standards; what the
 * target requires and no unit tests yet.
 */

#include "runner/lcrun.h"

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/** Columns of the matrix (the 8 standards). */
#define MAX_STDS	8

/**
 * @brief The run's target moved to standard @p s, keeping the options
 *        that make sense there (no POSIX option on an ISO C standard).
 *
 * @param r   Run.
 * @param s   Standard of the column.
 * @param buf Storage for the option list (64 entries).
 * @return The target.
 */
static t_target	targetFor(const t_run *r, const t_std *s, char **buf)
{
	t_target	t = r->o.target;
	size_t		n = 0;

	t.std = s;
	for (char **o = r->o.target.opts; o && *o && n < 63; o++)
	{
		const t_optdef	*d = optFind(*o);

		if (d == NULL || d->from || s->posix)
			buf[n++] = *o;
	}
	buf[n] = NULL;
	t.opts = buf;
	return (t);
}

/**
 * @brief Print one cell of the matrix: "tested/req", or "-" when nothing
 *        is required.
 *
 * @param tested Required interfaces with a unit.
 * @param req    Required interfaces.
 */
static void	cell(int tested, int req)
{
	char	c[24];

	if (req)
		snprintf(c, sizeof(c), "%d/%d", tested, req);
	else
		snprintf(c, sizeof(c), "-");
	printf(" %12s", c);
}

int	toolMatrix(const t_run *r)
{
	char		*optBuf[MAX_STDS][64];
	t_target	t[MAX_STDS];
	size_t		nStd = 0;
	int			total[MAX_STDS][2] = {{0}};
	char		buf[512];

	while (nStd < MAX_STDS && stdAt(nStd))
	{
		t[nStd] = targetFor(r, stdAt(nStd), optBuf[nStd]);
		nStd++;
	}
	targetDescribe(&r->o.target, buf, sizeof(buf));
	printf("tested / required per header, options of %s (ISO C columns: ISO C "
		"options only)\n\n%-14s", buf, "header");
	for (size_t s = 0; s < nStd; s++)
		printf(" %12s", stdAt(s)->name);
	printf("\n");
	for (size_t i = 0, j; i < r->spec.n; i = j)
	{
		int	row[MAX_STDS][2] = {{0}};

		for (j = i; j < r->spec.n && !strcmp(r->spec.v[j].cat, r->spec.v[i].cat); j++)
		{
			int	tested = hasUnit(r, &r->spec.v[j]);

			for (size_t s = 0; s < nStd; s++)
			{
				int	ob;

				if (!reqSelected(specRequired(&r->spec.v[j], &t[s], &ob)))
					continue ;
				row[s][1]++;
				row[s][0] += tested;
			}
		}
		if (!inList(r->o.cats, r->spec.v[i].cat))
			continue ;
		printf("%-14s", r->spec.v[i].cat);
		for (size_t s = 0; s < nStd; s++)
		{
			total[s][0] += row[s][0];
			total[s][1] += row[s][1];
			cell(row[s][0], row[s][1]);
		}
		printf("\n");
	}
	printf("%-14s", "TOTAL");
	for (size_t s = 0; s < nStd; s++)
		cell(total[s][0], total[s][1]);
	printf("\n");
	return (0);
}

int	toolTodo(const t_run *r)
{
	char	buf[512];
	int		todo = 0;
	int		req = 0;

	targetDescribe(&r->o.target, buf, sizeof(buf));
	printf("required by %s, no unit yet (%s/<cat>/<group>/<name>.c):\n", buf,
		r->o.suitesDir);
	for (size_t i = 0, j; i < r->spec.n; i = j)
	{
		int	shown = 0;

		for (j = i; j < r->spec.n && !strcmp(r->spec.v[j].cat, r->spec.v[i].cat); j++)
		{
			const t_entry	*e = &r->spec.v[j];
			int				ob;
			t_req			q = specRequired(e, &r->o.target, &ob);

			if (!reqSelected(q) || e->kind == KIND_TYPE || !inList(r->o.cats, e->cat))
				continue ;
			req++;
			if (hasUnit(r, e))
				continue ;
			if (!shown++)
				printf("\n  <%s>\n", e->header);
			printf("    %-28s %-8s %-13s %s%s\n", e->name, kindName(e->kind), e->since,
				q == REQ_OPTION ? (e->opt ? e->opt : e->alt->opt) : "",
				ob ? " (obsolescent)" : "");
			todo++;
		}
	}
	printf("\n%d of %d still to test (types are tested from the units that use them)\n",
		todo, req);
	return (0);
}
