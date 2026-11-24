/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file conformance.c
 * @brief Conformance table: per header, what the target requires.
 * @Created: 2026/11/24 08:14:13 by Moutig
 * @Updated: 2026/11/24 08:20:22 by Moutig
 *
 * For each header: required interfaces, probe results (declared, linked,
 * leaked, refused), tests run and passed, with the lists of the missing,
 * leaked, failing and untested ones.
 */

#include "runner/lcrun.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Counters of one row of the conformance table (a header). */
typedef struct s_conf
{
	int	req;		/* required interfaces (base + options + extensions) */
	int	opt;		/* of which in an option group */
	int	ob;			/* of which obsolescent */
	int	ok;			/* declared and linkable */
	int	undecl;		/* header does not declare it */
	int	unlinked;	/* declared, library does not define it */
	int	unknown;	/* not probed */
	int	tested;		/* at least one test ran */
	int	pass;		/* tested, every test passed */
	int	fail;		/* tested, a test did not pass */
	int	leak;		/* must be hidden (strict mode), visible anyway */
	int	nodiag;		/* reject probes that compiled (diagnostic missing) */
}	t_conf;

/**
 * @brief Add a name to a list of the conformance table ("a, b, c").
 *
 * Without -v, only the first 10 names are kept (listPrint() shows how
 * many more).
 *
 * @param list    Heap string, or pointer to NULL.
 * @param n       Names counted so far.
 * @param name    Name to add.
 * @param verbose Keep every name.
 */
static void	listAdd(char **list, int *n, const char *name, int verbose)
{
	int	max = verbose ? 100000 : 10;

	if ((*n)++ < max)
	{
		if (*list)
			strAppend(list, ", ", 2);
		strAppend(list, name, strlen(name));
	}
}

/**
 * @brief Print and free a list of the conformance table.
 *
 * @param label   "undeclared", "failing", ...
 * @param list    The list (freed).
 * @param n       Names in the list, kept or not.
 * @param verbose Same as listAdd().
 */
static void	listPrint(const char *label, char *list, int n, int verbose)
{
	if (n == 0)
		return ;
	printf("      %s%-11s%s %s", C_DIM, label, C_RESET, list);
	if (!verbose && n > 10)
		printf(" %s(+%d, -v)%s", C_DIM, n - 10, C_RESET);
	printf("\n");
	free(list);
}

/**
 * @brief Print one row of the conformance table.
 *
 * present = ok / req ("-" when nothing was probed), verified = pass / req.
 *
 * @param label Header name or TOTAL.
 * @param c     Counters.
 */
static void	confRow(const char *label, const t_conf *c)
{
	printf("  %-12s %5d %4d %4d %s%5d%s %s%5d%s %s%5d%s %s%5d%s %s%6d%s %6d %s%5d%s %s%5d%s",
		label, c->req, c->opt, c->ob,
		C_GREEN, c->ok, C_RESET,
		c->undecl ? C_RED : "", c->undecl, C_RESET,
		c->unlinked ? C_RED : "", c->unlinked, C_RESET,
		c->leak ? C_RED : "", c->leak, C_RESET,
		c->nodiag ? C_RED : "", c->nodiag, C_RESET,
		c->tested,
		c->pass ? C_GREEN : "", c->pass, C_RESET,
		c->fail ? C_RED : "", c->fail, C_RESET);
	if (c->unknown == c->req)
		printf(" %8s", "-");
	else
		printf(" %s%7.1f%%%s", c->ok == c->req ? C_GREEN : C_YEL,
			pct(c->ok, c->req), C_RESET);
	printf(" %s%7.1f%%%s\n", c->pass == c->req ? C_GREEN : C_YEL,
		pct(c->pass, c->req), C_RESET);
}

/**
 * @brief Add the counters of a row to the total.
 *
 * @param to Total.
 * @param c  Row.
 */
static void	confAdd(t_conf *to, const t_conf *c)
{
	to->req += c->req;
	to->opt += c->opt;
	to->ob += c->ob;
	to->ok += c->ok;
	to->undecl += c->undecl;
	to->unlinked += c->unlinked;
	to->unknown += c->unknown;
	to->tested += c->tested;
	to->pass += c->pass;
	to->fail += c->fail;
	to->leak += c->leak;
	to->nodiag += c->nodiag;
}

/**
 * @brief Is @p e left out by -c / -f?
 *
 * @param r Run.
 * @param e Entry.
 * @return 1 if filtered out, 0 if shown.
 */
static int	catFiltered(const t_run *r, const t_entry *e)
{
	if (r->o.cats)
	{
		int	in = 0;

		for (char **c = r->o.cats; *c; c++)
			in |= !strcmp(*c, e->cat);
		if (!in)
			return (1);
	}
	if (r->o.funcs)
	{
		int	in = 0;

		for (char **f = r->o.funcs; *f; f++)
			in |= !strcmp(*f, e->name);
		if (!in)
			return (1);
	}
	return (0);
}

void	sectionConformance(const t_run *r)
{
	char	target[256];
	t_conf	tot;
	size_t	i;
	size_t	j;

	memset(&tot, 0, sizeof(tot));
	targetDescribe(&r->o.target, target, sizeof(target));
	printf("\n%s=== Conformance to %s%s (spec/*.spec) ===%s\n", C_BOLD, target,
		r->o.lax ? ", lax" : "", C_RESET);
	printf("%s  %-12s %5s %4s %4s %5s %5s %5s %5s %6s %6s %5s %5s %8s %8s%s\n", C_BOLD,
		"header", "req", "opt", "ob", "ok", "undec", "unlnk", "leak", "nodiag", "tested",
		"pass", "fail", "present", "verified", C_RESET);
	for (i = 0; i < r->spec.n; i = j)
	{
		const char	*cat = r->spec.v[i].cat;
		t_conf		c;
		char		*lUndecl = NULL, *lUnlinked = NULL, *lFail = NULL, *lUntested = NULL;
		char		*lLeak = NULL, *lNodiag = NULL;
		int			nUndecl = 0, nUnlinked = 0, nFail = 0, nUntested = 0, nLeak = 0;
		int			nNodiag = 0;

		memset(&c, 0, sizeof(c));
		for (j = i; j < r->spec.n && !strcmp(r->spec.v[j].cat, cat); j++)
		{
			const t_entry	*e = &r->spec.v[j];
			int				ob;
			t_req			q = specRequired(e, &r->o.target, &ob);
			int				nt = 0;
			int				np = 0;

			if (catFiltered(r, e))
				continue ;
			if (r->hidden[j] == HID_LEAKED)
			{
				c.leak++;
				listAdd(&lLeak, &nLeak, e->name, r->o.verbose);
			}
			if (!reqSelected(q))
				continue ;
			c.req++;
			c.opt += (q == REQ_OPTION);
			c.ob += ob;
			switch (r->probes[j])
			{
				case PR_OK: c.ok++; break ;
				case PR_UNDECLARED: c.undecl++;
					listAdd(&lUndecl, &nUndecl, e->name, r->o.verbose); break ;
				case PR_UNLINKED: c.unlinked++;
					listAdd(&lUnlinked, &nUnlinked, e->name, r->o.verbose); break ;
				default: c.unknown++; break ;
			}
			for (size_t k = 0; k < r->nTests; k++)
			{
				if (strcmp(r->tests[k].cat, cat) || strcmp(r->tests[k].func, e->name))
					continue ;
				nt++;
				np += (r->tests[k].st == ST_PASS || r->tests[k].st == ST_SKIP);
			}
			if (nt)
			{
				c.tested++;
				if (np == nt)
					c.pass++;
				else
				{
					c.fail++;
					listAdd(&lFail, &nFail, e->name, r->o.verbose);
				}
			}
			else if (r->probes[j] != PR_UNDECLARED && r->probes[j] != PR_UNLINKED)
				listAdd(&lUntested, &nUntested, e->name, r->o.verbose);
		}
		for (size_t k = 0; k < r->nRejects; k++)
			if (!strcmp(r->rejects[k].cat, cat) && r->rejects[k].st == REJ_ACCEPTED
				&& inList(r->o.cats, cat))
			{
				char	what[160];

				snprintf(what, sizeof(what), "%s %d then %d (%s)", r->rejects[k].want,
					r->rejects[k].first, r->rejects[k].second, r->rejects[k].ref);
				c.nodiag++;
				listAdd(&lNodiag, &nNodiag, what, r->o.verbose);
			}
		if (c.req == 0 && c.leak == 0 && c.nodiag == 0)
			continue ;
		confRow(r->spec.v[i].header, &c);
		listPrint("undeclared", lUndecl, nUndecl, r->o.verbose);
		listPrint("leaked", lLeak, nLeak, r->o.verbose);
		listPrint("accepted", lNodiag, nNodiag, r->o.verbose);
		listPrint("not linked", lUnlinked, nUnlinked, r->o.verbose);
		listPrint("failing", lFail, nFail, r->o.verbose);
		listPrint("untested", lUntested, nUntested, r->o.verbose);
		confAdd(&tot, &c);
	}
	printf("  %s\n", "--------------------------------------------------------"
		"----------------------------------------------");
	printf("%s", C_BOLD);
	confRow("TOTAL", &tot);
	printf("%s", C_RESET);
	printf("  %sreq: required by the target (opt: by an option group, ob: obsolescent)."
		"\n  present: declared by the header and defined by the library (probes)."
		"\n  leak: not required, not reserved for the header, visible anyway"
		" (strict mode; LAX=1 skips it)."
		"\n  nodiag: a translation unit the standard requires to be refused compiled"
		" (reject probes)."
		"\n  verified: tested and every test passed.%s\n", C_DIM, C_RESET);
	if (tot.unknown == tot.req)
		printf("  %sno probe results (run through make, or pass -P DIR)%s\n",
			C_DIM, C_RESET);
}
