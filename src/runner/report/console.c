/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file console.c
 * @brief Console report of a run.
 * @Created: 2026/11/24 08:14:20 by Moutig
 * @Updated: 2026/11/24 11:55:16 by Moutig
 *
 * Failures, units that did not build, results per category (per
 * function with -v), conformance (report/conformance.c), slowest tests,
 * peak memory, flaky tests, totals. Colors only on a terminal.
 */

#include "runner/lcrun.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Use ANSI colors (set from the run, or by compareTsv()). */
int	gColor;

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
 * @brief Color of a status: green pass, yellow skip, cyan not implemented,
 *        magenta crash or timeout, dim unavailable, red otherwise.
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
		case ST_UNIMPL: return (C_CYAN);
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
		"PASS", "FAIL", "SKIP", "N/I", "CRASH", "TIMEOUT", "ERROR", "N/A"
	};

	return (l[st]);
}

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

double	pct(int part, int whole)
{
	return (whole ? 100.0 * part / whole : 0.0);
}

/**
 * @brief Print one row of a results table.
 *
 * pass% is over the tests that ran (skipped and not implemented ones
 * excluded).
 *
 * @param label     Category, "cat/func" or TOTAL.
 * @param a         Counters.
 * @param showFuncs Also print the units and unavailable columns.
 */
static void	aggRow(const char *label, const t_agg *a, int showFuncs)
{
	int	ran = a->tests - a->n[ST_SKIP] - a->n[ST_UNIMPL];

	printf("  %-22s", label);
	if (showFuncs)
		printf(" %5d %5d", a->funcs, a->unavail);
	printf(" %6d %s%6d%s %s%5d%s %s%5d%s %s%5d%s %s%5d%s %s%5d%s %s%5d%s",
		a->tests,
		a->n[ST_PASS] ? C_GREEN : "", a->n[ST_PASS], C_RESET,
		a->n[ST_FAIL] ? C_RED : "", a->n[ST_FAIL], C_RESET,
		a->n[ST_CRASH] ? C_MAG : "", a->n[ST_CRASH], C_RESET,
		a->n[ST_TIMEOUT] ? C_MAG : "", a->n[ST_TIMEOUT], C_RESET,
		a->n[ST_ERROR] ? C_RED : "", a->n[ST_ERROR], C_RESET,
		a->n[ST_SKIP] ? C_YEL : "", a->n[ST_SKIP], C_RESET,
		a->n[ST_UNIMPL] ? C_CYAN : "", a->n[ST_UNIMPL], C_RESET);
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
	printf(" %6s %6s %5s %5s %5s %5s %5s %5s %7s %8s %9s%s\n",
		"tests", "pass", "fail", "crash", "tmo", "err", "skip", "n/i",
		"pass%", "checks", "cpu ms", C_RESET);
}

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
 * @brief Every test that failed (fail, crash, timeout, error): where, spec reference,
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

		if (!statusFailed(t->st))
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
 * @brief Units that did not build, why (the interface is not implemented,
 *        or another build error), and the error lines of the build log.
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
		printf("  %s%s/%s%s  %s(%s)%s\n", C_DIM, u->cat, u->func, C_RESET,
			strcmp(unitReason(r, u), "build") ? C_CYAN : C_RED,
			strcmp(unitReason(r, u), "build") ? "not implemented" : "build error", C_RESET);
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
		"-------------------------------------");
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

/**
 * @brief Results of run/virtual.c named @p prefix... with status @p st.
 *
 * @param r      Run.
 * @param prefix "@hidden", "@reject_", "@not_in_target".
 * @param st     Status.
 * @return Count.
 */
static int	countResults(const t_run *r, const char *prefix, t_status st)
{
	int	n = 0;

	for (size_t i = 0; i < r->nTests; i++)
		n += (r->tests[i].st == st && !strncmp(r->tests[i].name, prefix, strlen(prefix)));
	return (n);
}

/**
 * @brief What the target leaves out, one line: interfaces skipped
 *        (@not_in_target), hidden as required (@hidden), test cases whose
 *        .since / .until / .opt exclude the target.
 *
 * @param r Run.
 */
static void	sectionOutOfTarget(const t_run *r)
{
	int	skipped = countResults(r, "@not_in_target", ST_SKIP);
	int	hidden = countResults(r, "@hidden", ST_PASS);
	int	leaked = countResults(r, "@hidden", ST_FAIL);

	if (skipped == 0 && r->nOutOfTarget == 0)
		return ;
	printf("%snot in this target: %d interface%s skipped (@not_in_target)", C_DIM, skipped,
		skipped == 1 ? "" : "s");
	if (hidden + leaked)
		printf(", %d of them must be hidden: %d hidden, %s%d leaked%s%s", hidden + leaked,
			hidden, leaked ? C_RED : "", leaked, C_RESET, C_DIM);
	if (r->nOutOfTarget)
		printf("; %zu test case%s excluded by .since / .until / .opt", r->nOutOfTarget,
			r->nOutOfTarget == 1 ? "" : "s");
	printf("%s\n", C_RESET);
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
	ran = (int)r->nTests - n[ST_SKIP] - n[ST_UNIMPL];
	{
		char	target[256];

		targetDescribe(&r->o.target, target, sizeof(target));
		printf("\n%slibc %s%s against %s: %d/%zu units built, %zu tests, "
			"%ld checks, %.2f s wall, %d jobs\n", C_BOLD, r->o.libc, C_RESET,
			target, units, r->nUnits, r->nTests, checks, r->wallMs / 1e3, r->o.jobs);
		sectionOutOfTarget(r);
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
	{
		int	leaks = countResults(r, "@hidden", ST_FAIL);
		int	nodiag = countResults(r, "@reject_", ST_FAIL);

		/* what the failures above are made of, when not only tests */
		if (leaks || nodiag)
			printf("  %s(of which %d leaked, %d missing diagnostic%s)%s", C_RED, leaks,
				nodiag, nodiag == 1 ? "" : "s", C_RESET);
		if (r->o.lax)
			printf("  %s(lax: visibility not checked)%s", C_DIM, C_RESET);
	}
	printf("\n");
}
