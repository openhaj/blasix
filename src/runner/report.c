/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file report.c
 * @brief lcrun: console report, TSV / JSON export, run comparison.
 * @Created: 2026/11/23 04:42:42 by Moutig
 * @Updated: 2026/11/23 04:42:42 by Moutig
 *
 * Console report, in order:
 *   1. every test (-v only);
 *   2. details of every non-passing test (messages + captured output);
 *   3. units that could not be built (function missing in the libc);
 *   4. per-category table (per-function with -v);
 *   5. conformance per category (spec/<cat>.spec);
 *   6. slowest tests, biggest peak RSS, flaky tests (-r N);
 *   7. totals.
 *
 * -q keeps only the tables and the totals. Colors are only used when
 * stdout is a terminal.
 *
 * The TSV (one row per test, per unavailable unit and per required
 * interface) is the input of --compare, which pairs the rows of two
 * runs by "cat/func/test" and classifies each difference as better
 * (now passes), worse (passed before) or changed.
 */

#include "runner/lcrun.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----- Colors ----- */

/** Use ANSI colors (set from the run, or by compareTsv()). */
static int	gColor;

/* Each C_* expands to an escape sequence, or to "" without colors. */

#define C_RESET	(gColor ? "\033[0m" : "")
#define C_BOLD	(gColor ? "\033[1m" : "")
#define C_DIM	(gColor ? "\033[2m" : "")
#define C_RED	(gColor ? "\033[31m" : "")
#define C_GREEN	(gColor ? "\033[32m" : "")
#define C_YEL	(gColor ? "\033[33m" : "")
#define C_MAG	(gColor ? "\033[35m" : "")
#define C_CYAN	(gColor ? "\033[36m" : "")

/**
 * @brief Color of a status: green pass, yellow skip, magenta crash or
 *        timeout, dim unavailable, red otherwise.
 *
 * @param st Status.
 * @return Escape sequence, or "" without colors.
 */
static const char	*stColor(t_status st)
{
	switch (st)
	{
		case ST_PASS: return (C_GREEN);
		case ST_SKIP: return (C_YEL);
		case ST_UNAVAIL: return (C_DIM);
		case ST_CRASH:
		case ST_TIMEOUT: return (C_MAG);
		default: return (C_RED);
	}
}

/**
 * @brief Label of a status in the console report ("PASS", "N/A", ...).
 *
 * @param st Status.
 * @return Static string.
 */
static const char	*stLabel(t_status st)
{
	static const char	*l[ST_COUNT] = {
		"PASS", "FAIL", "SKIP", "CRASH", "TIMEOUT", "ERROR", "N/A"
	};

	return (l[st]);
}

/* ----- Aggregation ----- */

/** Counters of one row of the results tables (a category or a function). */
typedef struct s_agg
{
	int		n[ST_COUNT];	/* Tests per status. */
	int		tests;			/* Tests. */
	int		funcs;			/* Units. */
	int		unavail;		/* Units that did not build. */
	long	checks;			/* Checks run. */
	double	cpuMs;			/* CPU time of the tests. */
}	t_agg;

/**
 * @brief Count one test in a row.
 *
 * @param a Row.
 * @param t Test.
 */
static void	aggAdd(t_agg *a, const t_test *t)
{
	a->n[t->st]++;
	a->tests++;
	a->checks += t->asserts;
	a->cpuMs += t->cpuMs;
}

/**
 * @brief Percentage, 0 when @p whole is 0.
 *
 * @param part  Part.
 * @param whole Whole.
 * @return 100 * part / whole.
 */
static double	pct(int part, int whole)
{
	return (whole ? 100.0 * part / whole : 0.0);
}

/**
 * @brief Print one row of a results table.
 *
 * pass% is over the tests that ran (skipped ones excluded).
 *
 * @param label     Category, "cat/func" or TOTAL.
 * @param a         Counters.
 * @param showFuncs Also print the units and unavailable columns.
 */
static void	aggRow(const char *label, const t_agg *a, int showFuncs)
{
	int	ran = a->tests - a->n[ST_SKIP];

	printf("  %-22s", label);
	if (showFuncs)
		printf(" %5d %5d", a->funcs, a->unavail);
	printf(" %6d %s%6d%s %s%5d%s %s%5d%s %s%5d%s %s%5d%s %s%5d%s",
		a->tests,
		a->n[ST_PASS] ? C_GREEN : "", a->n[ST_PASS], C_RESET,
		a->n[ST_FAIL] ? C_RED : "", a->n[ST_FAIL], C_RESET,
		a->n[ST_CRASH] ? C_MAG : "", a->n[ST_CRASH], C_RESET,
		a->n[ST_TIMEOUT] ? C_MAG : "", a->n[ST_TIMEOUT], C_RESET,
		a->n[ST_ERROR] ? C_RED : "", a->n[ST_ERROR], C_RESET,
		a->n[ST_SKIP] ? C_YEL : "", a->n[ST_SKIP], C_RESET);
	printf(" %s%6.1f%%%s %8ld %9.1f\n",
		(ran && a->n[ST_PASS] == ran) ? C_GREEN : (ran ? C_RED : ""),
		pct(a->n[ST_PASS], ran), C_RESET, a->checks, a->cpuMs);
}

/**
 * @brief Print the header of a results table.
 *
 * @param first     Title of the first column.
 * @param showFuncs Same as aggRow().
 */
static void	aggHeader(const char *first, int showFuncs)
{
	printf("%s  %-22s", C_BOLD, first);
	if (showFuncs)
		printf(" %5s %5s", "funcs", "n/a");
	printf(" %6s %6s %5s %5s %5s %5s %5s %7s %8s %9s%s\n",
		"tests", "pass", "fail", "crash", "tmo", "err", "skip",
		"pass%", "checks", "cpu ms", C_RESET);
}

/* ----- Sections of the console report ----- */

/**
 * @brief Print a multi-line text indented, at most @p maxLines lines.
 *
 * @param s        Text, or NULL.
 * @param maxLines Lines shown; "[...]" marks the rest.
 */
static void	printIndented(const char *s, int maxLines)
{
	int	lines = 0;

	while (s && *s && lines < maxLines)
	{
		const char	*nl = strchr(s, '\n');
		size_t		len = nl ? (size_t)(nl - s) : strlen(s);

		printf("      %.*s\n", (int)len, s);
		lines++;
		s = nl ? nl + 1 : s + len;
	}
	if (s && *s)
		printf("      %s[...]%s\n", C_DIM, C_RESET);
}

/**
 * @brief -v: every test with its status, time and checks.
 *
 * @param r Run.
 */
static void	sectionTests(const t_run *r)
{
	for (size_t i = 0; i < r->nTests; i++)
	{
		const t_test	*t = &r->tests[i];

		printf("  %s%-7s%s %s/%s/%s %s(%.1f ms, %d checks)%s\n",
			stColor(t->st), stLabel(t->st), C_RESET,
			t->cat, t->func, t->name, C_DIM, t->ms, t->asserts, C_RESET);
		if (t->st == ST_PASS && t->msgs)
			printIndented(t->msgs, 20);
	}
}

/**
 * @brief Every test that did not pass or skip: where, spec reference,
 *        messages and captured output.
 *
 * @param r Run.
 */
static void	sectionFailures(const t_run *r)
{
	int	shown = 0;

	for (size_t i = 0; i < r->nTests; i++)
	{
		const t_test	*t = &r->tests[i];

		if (t->st == ST_PASS || t->st == ST_SKIP)
			continue ;
		if (!shown++)
			printf("\n%s=== Failures ===%s\n", C_BOLD, C_RESET);
		printf("  %s%-7s%s %s/%s/%s  %s%s:%d%s\n",
			stColor(t->st), stLabel(t->st), C_RESET,
			t->cat, t->func, t->name, C_DIM, t->file, t->line, C_RESET);
		if (t->ref || t->since)
			printf("    %sspec: %s%s%s%s\n", C_CYAN, t->ref ? t->ref : "",
				t->ref && t->since ? ", " : "",
				t->since ? "since " : "", t->since ? t->since : "");
		printIndented(t->msgs, 30);
		if (t->out && *t->out)
		{
			printf("    %soutput:%s\n", C_DIM, C_RESET);
			printIndented(t->out, 20);
		}
	}
}

/**
 * @brief Units that did not build, with the start of the compiler output.
 *
 * @param r Run.
 */
static void	sectionUnavailable(const t_run *r)
{
	int	shown = 0;

	for (size_t i = 0; i < r->nUnits; i++)
	{
		const t_unit	*u = &r->units[i];
		const char		*e;

		if (u->bin)
			continue ;
		if (!shown++)
			printf("\n%s=== Unavailable (did not build against this libc) ===%s\n",
				C_BOLD, C_RESET);
		printf("  %s%s/%s%s\n", C_DIM, u->cat, u->func, C_RESET);
		e = u->err;
		if (e == NULL)
			printf("      (no build log)\n");
		else
			printIndented(e, r->o.verbose ? 6 : 2);
	}
}

/**
 * @brief Results per category and total (and per function with -v).
 *
 * @param r Run.
 */
static void	sectionTables(const t_run *r)
{
	t_agg	tot;
	size_t	i;
	size_t	j;

	memset(&tot, 0, sizeof(tot));
	printf("\n%s=== Results by category ===%s\n", C_BOLD, C_RESET);
	aggHeader("category", 1);
	for (i = 0; i < r->nUnits; )
	{
		const char	*cat = r->units[i].cat;
		t_agg		a;

		memset(&a, 0, sizeof(a));
		for (j = i; j < r->nUnits && !strcmp(r->units[j].cat, cat); j++)
		{
			a.funcs++;
			if (r->units[j].bin == NULL)
				a.unavail++;
		}
		for (size_t k = 0; k < r->nTests; k++)
			if (!strcmp(r->tests[k].cat, cat))
				aggAdd(&a, &r->tests[k]);
		aggRow(cat, &a, 1);
		for (int s = 0; s < ST_COUNT; s++)
			tot.n[s] += a.n[s];
		tot.tests += a.tests;
		tot.funcs += a.funcs;
		tot.unavail += a.unavail;
		tot.checks += a.checks;
		tot.cpuMs += a.cpuMs;
		i = j;
	}
	printf("  %s\n", "------------------------------------------------------------"
		"-------------------------------");
	printf("%s", C_BOLD);
	aggRow("TOTAL", &tot, 1);
	printf("%s", C_RESET);
	if (!r->o.verbose)
		return ;
	printf("\n%s=== Results by function ===%s\n", C_BOLD, C_RESET);
	aggHeader("category/function", 0);
	for (i = 0; i < r->nUnits; i++)
	{
		t_agg	a;
		char	label[256];

		if (r->units[i].bin == NULL)
			continue ;
		memset(&a, 0, sizeof(a));
		for (size_t k = 0; k < r->nTests; k++)
			if (r->tests[k].unit == &r->units[i])
				aggAdd(&a, &r->tests[k]);
		snprintf(label, sizeof(label), "%s/%s", r->units[i].cat, r->units[i].func);
		aggRow(label, &a, 0);
	}
}

/* ----- Conformance (spec + probes + tests) ----- */

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
	printf("  %-12s %5d %4d %4d %s%5d%s %s%5d%s %s%5d%s %6d %s%5d%s %s%5d%s",
		label, c->req, c->opt, c->ob,
		C_GREEN, c->ok, C_RESET,
		c->undecl ? C_RED : "", c->undecl, C_RESET,
		c->unlinked ? C_RED : "", c->unlinked, C_RESET,
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

/**
 * @brief Conformance per header for the target.
 *
 * For each required interface: its probe result (present, undeclared,
 * unlinked) and whether its tests ran and passed. Lists the undeclared,
 * unlinked, failing and untested ones.
 *
 * @param r Run.
 */
static void	sectionConformance(const t_run *r)
{
	char	target[256];
	t_conf	tot;
	size_t	i;
	size_t	j;

	memset(&tot, 0, sizeof(tot));
	targetDescribe(&r->o.target, target, sizeof(target));
	printf("\n%s=== Conformance to %s (spec/*.spec) ===%s\n", C_BOLD, target, C_RESET);
	printf("%s  %-12s %5s %4s %4s %5s %5s %5s %6s %5s %5s %8s %8s%s\n", C_BOLD,
		"header", "req", "opt", "ob", "ok", "undec", "unlnk", "tested", "pass",
		"fail", "present", "verified", C_RESET);
	for (i = 0; i < r->spec.n; i = j)
	{
		const char	*cat = r->spec.v[i].cat;
		t_conf		c;
		char		*lUndecl = NULL, *lUnlinked = NULL, *lFail = NULL, *lUntested = NULL;
		int			nUndecl = 0, nUnlinked = 0, nFail = 0, nUntested = 0;

		memset(&c, 0, sizeof(c));
		for (j = i; j < r->spec.n && !strcmp(r->spec.v[j].cat, cat); j++)
		{
			const t_entry	*e = &r->spec.v[j];
			int				ob;
			t_req			q = specRequired(e, &r->o.target, &ob);
			int				nt = 0;
			int				np = 0;

			if (!reqSelected(q) || catFiltered(r, e))
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
		if (c.req == 0)
			continue ;
		confRow(r->spec.v[i].header, &c);
		listPrint("undeclared", lUndecl, nUndecl, r->o.verbose);
		listPrint("not linked", lUnlinked, nUnlinked, r->o.verbose);
		listPrint("failing", lFail, nFail, r->o.verbose);
		listPrint("untested", lUntested, nUntested, r->o.verbose);
		confAdd(&tot, &c);
	}
	printf("  %s\n", "--------------------------------------------------------"
		"---------------------------------");
	printf("%s", C_BOLD);
	confRow("TOTAL", &tot);
	printf("%s", C_RESET);
	printf("  %sreq: required by the target (opt: by an option group, ob: obsolescent)."
		"\n  present: declared by the header and defined by the library (probes)."
		"\n  verified: tested and every test passed.%s\n", C_DIM, C_RESET);
	if (tot.unknown == tot.req)
		printf("  %sno probe results (run through make, or pass -P DIR)%s\n",
			C_DIM, C_RESET);
}

/**
 * @brief qsort() order: slowest test first.
 *
 * @param a Pointer to a t_test pointer.
 * @param b Same.
 * @return Negative, zero or positive.
 */
static int	cmpMs(const void *a, const void *b)
{
	const t_test	*x = *(t_test *const *)a;
	const t_test	*y = *(t_test *const *)b;

	return ((y->ms > x->ms) - (y->ms < x->ms));
}

/**
 * @brief qsort() order: biggest peak RSS first.
 *
 * @param a Pointer to a t_test pointer.
 * @param b Same.
 * @return Negative, zero or positive.
 */
static int	cmpRss(const void *a, const void *b)
{
	const t_test	*x = *(t_test *const *)a;
	const t_test	*y = *(t_test *const *)b;

	return ((y->rssKb > x->rssKb) - (y->rssKb < x->rssKb));
}

/**
 * @brief Slowest tests, biggest peak RSS, and flaky tests (-r N: passed
 *        some of the runs only).
 *
 * @param r Run.
 */
static void	sectionExtremes(const t_run *r)
{
	t_test	**v;
	size_t	n = r->nTests;
	size_t	top = r->o.verbose ? 10 : 5;
	int		flaky = 0;

	if (n == 0)
		return ;
	v = xmalloc(n * sizeof(*v));
	for (size_t i = 0; i < n; i++)
		v[i] = &r->tests[i];
	qsort(v, n, sizeof(*v), cmpMs);
	printf("\n%s=== Slowest tests ===%s\n", C_BOLD, C_RESET);
	for (size_t i = 0; i < n && i < top; i++)
		printf("  %9.1f ms  %s/%s/%s\n", v[i]->ms, v[i]->cat, v[i]->func, v[i]->name);
	qsort(v, n, sizeof(*v), cmpRss);
	printf("\n%s=== Peak memory (max RSS) ===%s\n", C_BOLD, C_RESET);
	for (size_t i = 0; i < n && i < 3; i++)
		printf("  %9ld kB  %s/%s/%s\n", v[i]->rssKb, v[i]->cat, v[i]->func, v[i]->name);
	free(v);
	for (size_t i = 0; i < n; i++)
	{
		const t_test	*t = &r->tests[i];

		if (t->runs > 1 && t->passes > 0 && t->passes < t->runs)
		{
			if (!flaky++)
				printf("\n%s=== Flaky (passed only some of the runs) ===%s\n",
					C_BOLD, C_RESET);
			printf("  %d/%d  %s/%s/%s\n", t->passes, t->runs, t->cat, t->func, t->name);
		}
	}
}

void	reportConsole(const t_run *r)
{
	int		n[ST_COUNT] = {0};
	long	checks = 0;
	int		ran;
	int		units = 0;

	gColor = r->o.color;
	for (size_t i = 0; i < r->nTests; i++)
	{
		n[r->tests[i].st]++;
		checks += r->tests[i].asserts;
	}
	for (size_t i = 0; i < r->nUnits; i++)
		units += (r->units[i].bin != NULL);
	if (r->o.verbose && !r->o.quiet)
	{
		printf("%s=== Tests ===%s\n", C_BOLD, C_RESET);
		sectionTests(r);
	}
	if (!r->o.quiet)
	{
		sectionFailures(r);
		sectionUnavailable(r);
	}
	sectionTables(r);
	if (!r->o.quiet)
	{
		sectionConformance(r);
		sectionExtremes(r);
	}
	ran = (int)r->nTests - n[ST_SKIP];
	{
		char	target[256];

		targetDescribe(&r->o.target, target, sizeof(target));
		printf("\n%slibc %s%s against %s: %d/%zu units built, %zu tests, "
			"%ld checks, %.2f s wall, %d jobs\n", C_BOLD, r->o.libc, C_RESET,
			target, units, r->nUnits, r->nTests, checks, r->wallMs / 1e3, r->o.jobs);
		if (r->nUnitsOutOfTarget || r->nOutOfTarget)
			printf("%s%zu units and %zu tests not in this target%s\n", C_DIM,
				r->nUnitsOutOfTarget, r->nOutOfTarget, C_RESET);
	}
	if (ran && n[ST_PASS] == ran)
		printf("%s%sALL %d TESTS PASSED%s", C_BOLD, C_GREEN, ran, C_RESET);
	else
		printf("%s%s%d/%d PASSED (%.1f%%)%s", C_BOLD,
			ran ? C_RED : C_YEL, n[ST_PASS], ran, pct(n[ST_PASS], ran), C_RESET);
	for (int s = ST_FAIL; s < ST_UNAVAIL; s++)
		if (n[s])
			printf("  %s%d %s%s", stColor((t_status)s), n[s], statusName((t_status)s), C_RESET);
	if (units < (int)r->nUnits)
		printf("  %s%zu unavailable%s", C_DIM, r->nUnits - (size_t)units, C_RESET);
	printf("\n");
}

/* ----- TSV ----- */

/**
 * @brief Name of a probe result in TSV / JSON ("present", ...).
 *
 * @param p Probe result.
 * @return Static string.
 */
static const char	*probeName(t_probe p)
{
	static const char	*n[] = {"unprobed", "present", "undeclared", "unlinked"};

	return (n[p]);
}

int	reportTsv(const t_run *r, const char *path)
{
	FILE	*fp = fopen(path, "w");

	if (fp == NULL)
		return (-1);
	fprintf(fp, "category\tfunction\ttest\tstatus\tms\tcpu_ms\trss_kb"
		"\tchecks\tfailed\truns\tpasses\n");
	for (size_t i = 0; i < r->nTests; i++)
	{
		const t_test	*t = &r->tests[i];

		fprintf(fp, "%s\t%s\t%s\t%s\t%.3f\t%.3f\t%ld\t%d\t%d\t%d\t%d\n",
			t->cat, t->func, t->name, statusName(t->st), t->ms, t->cpuMs,
			t->rssKb, t->asserts, t->failed, t->runs, t->passes);
	}
	for (size_t i = 0; i < r->nUnits; i++)
		if (r->units[i].bin == NULL)
			fprintf(fp, "%s\t%s\t-\tunavailable\t0\t0\t0\t0\t0\t0\t0\n",
				r->units[i].cat, r->units[i].func);
	/* one row per required interface: test "@interface", status = probe */
	for (size_t i = 0; i < r->spec.n; i++)
	{
		int	ob;

		if (!reqSelected(specRequired(&r->spec.v[i], &r->o.target, &ob)))
			continue ;
		fprintf(fp, "%s\t%s\t@interface\t%s\t0\t0\t0\t0\t0\t0\t0\n",
			r->spec.v[i].cat, r->spec.v[i].name, probeName(r->probes[i]));
	}
	return (fclose(fp));
}

/* ----- JSON ----- */

/**
 * @brief Write a JSON string, escaped (NULL is written as "").
 *
 * @param fp Output stream.
 * @param s  String, or NULL.
 */
static void	jsonStr(FILE *fp, const char *s)
{
	fputc('"', fp);
	for (; s && *s; s++)
	{
		unsigned char	c = (unsigned char)*s;

		if (c == '"' || c == '\\')
			fprintf(fp, "\\%c", c);
		else if (c == '\n')
			fputs("\\n", fp);
		else if (c == '\t')
			fputs("\\t", fp);
		else if (c < 0x20)
			fprintf(fp, "\\u%04x", c);
		else
			fputc(c, fp);
	}
	fputc('"', fp);
}

int	reportJson(const t_run *r, const char *path)
{
	FILE	*fp = fopen(path, "w");
	int		n[ST_COUNT] = {0};

	if (fp == NULL)
		return (-1);
	for (size_t i = 0; i < r->nTests; i++)
		n[r->tests[i].st]++;
	char	target[256];

	targetDescribe(&r->o.target, target, sizeof(target));
	fprintf(fp, "{\n  \"libc\": ");
	jsonStr(fp, r->o.libc);
	fprintf(fp, ",\n  \"target\": ");
	jsonStr(fp, target);
	fprintf(fp, ",\n  \"wall_ms\": %.3f,\n  \"jobs\": %d,\n  \"summary\": {",
		r->wallMs, r->o.jobs);
	for (int s = 0; s < ST_COUNT; s++)
		fprintf(fp, "%s\"%s\": %d", s ? ", " : "", statusName((t_status)s), n[s]);
	fprintf(fp, "},\n  \"tests\": [\n");
	for (size_t i = 0; i < r->nTests; i++)
	{
		const t_test	*t = &r->tests[i];

		fprintf(fp, "    {\"category\": ");
		jsonStr(fp, t->cat);
		fprintf(fp, ", \"function\": ");
		jsonStr(fp, t->func);
		fprintf(fp, ", \"test\": ");
		jsonStr(fp, t->name);
		fprintf(fp, ", \"file\": ");
		jsonStr(fp, t->file);
		fprintf(fp, ", \"line\": %d, \"status\": \"%s\", \"ms\": %.3f, "
			"\"cpu_ms\": %.3f, \"rss_kb\": %ld, \"checks\": %d, \"failed\": %d, "
			"\"runs\": %d, \"passes\": %d, \"signal\": %d, \"exit\": %d, "
			"\"messages\": ", t->line, statusName(t->st), t->ms, t->cpuMs,
			t->rssKb, t->asserts, t->failed, t->runs, t->passes, t->sig, t->code);
		jsonStr(fp, t->msgs);
		fprintf(fp, ", \"output\": ");
		jsonStr(fp, t->out);
		fprintf(fp, ", \"spec_ref\": ");
		jsonStr(fp, t->ref);
		fprintf(fp, ", \"since\": ");
		jsonStr(fp, t->since);
		fprintf(fp, "}%s\n", i + 1 < r->nTests ? "," : "");
	}
	fprintf(fp, "  ],\n  \"unavailable\": [");
	for (size_t i = 0, k = 0; i < r->nUnits; i++)
	{
		if (r->units[i].bin)
			continue ;
		fprintf(fp, "%s\n    {\"category\": ", k++ ? "," : "");
		jsonStr(fp, r->units[i].cat);
		fprintf(fp, ", \"function\": ");
		jsonStr(fp, r->units[i].func);
		fprintf(fp, ", \"error\": ");
		jsonStr(fp, r->units[i].err);
		fprintf(fp, "}");
	}
	fprintf(fp, "\n  ],\n  \"interfaces\": [");
	for (size_t i = 0, k = 0; i < r->spec.n; i++)
	{
		const t_entry	*e = &r->spec.v[i];
		int				ob;
		t_req			q = specRequired(e, &r->o.target, &ob);

		if (!reqSelected(q))
			continue ;
		fprintf(fp, "%s\n    {\"header\": ", k++ ? "," : "");
		jsonStr(fp, e->header);
		fprintf(fp, ", \"name\": ");
		jsonStr(fp, e->name);
		fprintf(fp, ", \"since\": ");
		jsonStr(fp, e->since);
		fprintf(fp, ", \"requirement\": \"%s\", \"obsolescent\": %s, "
			"\"probe\": \"%s\"}", reqName(q), ob ? "true" : "false",
			probeName(r->probes[i]));
	}
	fprintf(fp, "\n  ]\n}\n");
	return (fclose(fp));
}

/* ----- Comparison of two TSV results ----- */

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
