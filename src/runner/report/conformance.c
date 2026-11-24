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
 * @Updated: 2026/11/24 10:28:32 by Moutig
 *
 * For each header: required interfaces, probe results (declared, linked,
 * leaked, refused), tests run and passed, with the lists of the missing,
 * leaked, failing, unbuilt and untested ones. The verdict of each
 * interface (interfaceResult()) and the counters of each header
 * (confHeader()) are also what the JSON results say.
 */

#include "runner/lcrun.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	to->broken += c->broken;
	to->unbuilt += c->unbuilt;
	to->untested += c->untested;
	to->unknown += c->unknown;
	to->tested += c->tested;
	to->pass += c->pass;
	to->fail += c->fail;
	to->leak += c->leak;
	to->nodiag += c->nodiag;
}

int	interfaceFiltered(const t_run *r, size_t i)
{
	const t_entry	*e = &r->spec.v[i];

	return (!inList(r->o.cats, e->cat) || !inList(r->o.funcs, e->name));
}

const char	*verdictName(t_verdict v)
{
	static const char	*n[VD_COUNT] = {
		"missing", "broken", "failing", "verified", "unbuilt", "untested"
	};

	return (n[v]);
}

/**
 * @brief The unit that tests @p e: its own (<cat>/.../<name>.c), else
 *        <cat>/defs for a type or a constant.
 *
 * @param r Run.
 * @param e Entry.
 * @return The unit, or NULL when there is none in this run.
 */
static const t_unit	*interfaceUnit(const t_run *r, const t_entry *e)
{
	const t_unit	*defs = NULL;

	for (size_t i = 0; i < r->nUnits; i++)
	{
		const t_unit	*u = &r->units[i];

		if (strcmp(u->cat, e->cat))
			continue ;
		if (!strcmp(u->func, e->name))
			return (u);
		if (!strcmp(u->func, "defs"))
			defs = u;
	}
	return (specIsDef(e, 0) ? defs : NULL);
}

/**
 * @brief Does the build log of a unit say that @p name is missing?
 *
 * "'name' undeclared", "implicit declaration of function 'name'",
 * "unknown type name 'name'", "undefined reference to `name'", with the
 * quotes of GCC (ASCII or UTF-8) and clang.
 *
 * @param log  Build log, or NULL.
 * @param name Interface.
 * @return 1 or 0.
 */
static int	logSaysMissing(const char *log, const char *name)
{
	size_t	len = strlen(name);

	for (const char *p = log ? strstr(log, name) : NULL; p; p = strstr(p + 1, name))
	{
		const char	*eol = strchr(p, '\n');
		const char	*bol = p;
		char		line[512];
		size_t		n;

		if ((p > log && (isalnum((unsigned char)p[-1]) || p[-1] == '_'))
			|| isalnum((unsigned char)p[len]) || p[len] == '_')
			continue ;
		while (bol > log && bol[-1] != '\n')
			bol--;
		n = (size_t)((eol ? eol : p + strlen(p)) - bol);
		if (n >= sizeof(line))
			n = sizeof(line) - 1;
		memcpy(line, bol, n);
		line[n] = '\0';
		if (strstr(line, "undeclared") || strstr(line, "implicit declaration")
			|| strstr(line, "unknown type name") || strstr(line, "undefined reference")
			|| strstr(line, "undefined symbol") || strstr(line, "use of undeclared"))
			return (1);
	}
	return (0);
}

const char	*unitReason(const t_run *r, const t_unit *u)
{
	if (u->entry)
	{
		for (size_t i = 0; i < r->spec.n; i++)
		{
			if (&r->spec.v[i] != u->entry && r->spec.v[i].alt != u->entry)
				continue ;
			if (r->probes[i] == PR_UNDECLARED || r->probes[i] == PR_UNLINKED)
				return ("unimplemented");
			if (r->probes[i] != PR_UNKNOWN)
				return ("build");
			break ;
		}
		if (logSaysMissing(u->log, u->func))
			return ("unimplemented");
	}
	return ("build");
}

void	interfaceResult(const t_run *r, size_t i, t_iresult *x)
{
	const t_entry	*e = &r->spec.v[i];
	t_probe			p = r->probes[i];

	memset(x, 0, sizeof(*x));
	x->unit = interfaceUnit(r, e);
	for (size_t k = 0; k < r->nTests; k++)
	{
		const t_test	*t = &r->tests[k];

		if (strcmp(t->cat, e->cat) || strcmp(t->func, e->name))
			continue ;
		x->tests++;
		x->passed += (t->st == ST_PASS);
		x->failed += statusFailed(t->st);
		x->skipped += (t->st == ST_SKIP);
		x->unimpl += (t->st == ST_UNIMPL);
	}
	if (p == PR_UNDECLARED || p == PR_UNLINKED)
		x->verdict = VD_MISSING;
	else if (p == PR_BROKEN)
		x->verdict = VD_BROKEN;
	else if (x->failed)
		x->verdict = VD_FAILING;
	else if (x->passed)
		x->verdict = VD_VERIFIED;
	else if (x->unit && x->unit->bin == NULL)
		x->verdict = !strcmp(unitReason(r, x->unit), "unimplemented") && x->unit->entry
			? VD_MISSING : VD_UNBUILT;
	else
		x->verdict = VD_UNTESTED;
}

/** Names listed under a row of the console table. */
typedef struct s_lists
{
	char	*undecl, *unlinked, *broken, *fail, *unbuilt, *untested, *leak, *nodiag;
	int		nUndecl, nUnlinked, nBroken, nFail, nUnbuilt, nUntested, nLeak, nNodiag;
}	t_lists;

/**
 * @brief Counters (and, for the console, lists) of one header.
 *
 * @param r Run.
 * @param i First entry of the header.
 * @param c Filled in.
 * @param l Lists to fill, or NULL.
 * @return Index of the first entry of the next header.
 */
static size_t	confCompute(const t_run *r, size_t i, t_conf *c, t_lists *l)
{
	const char	*cat = r->spec.v[i].cat;
	int			v = r->o.verbose;
	size_t		j;

	memset(c, 0, sizeof(*c));
	if (l)
		memset(l, 0, sizeof(*l));
	for (j = i; j < r->spec.n && !strcmp(r->spec.v[j].cat, cat); j++)
	{
		const t_entry	*e = &r->spec.v[j];
		int				ob;
		t_req			q = specRequired(e, &r->o.target, &ob);
		t_iresult		x;

		if (interfaceFiltered(r, j))
			continue ;
		if (r->hidden[j] == HID_LEAKED)
		{
			c->leak++;
			if (l)
				listAdd(&l->leak, &l->nLeak, e->name, v);
		}
		if (!reqSelected(q))
			continue ;
		c->req++;
		c->opt += (q == REQ_OPTION);
		c->ob += ob;
		switch (r->probes[j])
		{
			case PR_OK: c->ok++; break ;
			case PR_UNDECLARED: c->undecl++;
				if (l)
					listAdd(&l->undecl, &l->nUndecl, e->name, v);
				break ;
			case PR_UNLINKED: c->unlinked++;
				if (l)
					listAdd(&l->unlinked, &l->nUnlinked, e->name, v);
				break ;
			case PR_BROKEN: c->broken++;
				if (l)
					listAdd(&l->broken, &l->nBroken, e->name, v);
				break ;
			default: c->unknown++; break ;
		}
		interfaceResult(r, j, &x);
		c->tested += (x.passed + x.failed > 0);
		switch (x.verdict)
		{
			case VD_VERIFIED: c->pass++; break ;
			case VD_FAILING: c->fail++;
				if (l)
					listAdd(&l->fail, &l->nFail, e->name, v);
				break ;
			case VD_UNBUILT: c->unbuilt++;
				if (l)
					listAdd(&l->unbuilt, &l->nUnbuilt, e->name, v);
				break ;
			case VD_UNTESTED: c->untested++;
				if (l)
					listAdd(&l->untested, &l->nUntested, e->name, v);
				break ;
			default: break ;
		}
	}
	for (size_t k = 0; k < r->nRejects; k++)
		if (!strcmp(r->rejects[k].cat, cat) && r->rejects[k].st == REJ_ACCEPTED
			&& inList(r->o.cats, cat))
		{
			char	what[160];

			snprintf(what, sizeof(what), "%s %d then %d (%s)", r->rejects[k].want,
				r->rejects[k].first, r->rejects[k].second, r->rejects[k].ref);
			c->nodiag++;
			if (l)
				listAdd(&l->nodiag, &l->nNodiag, what, v);
		}
	return (j);
}

size_t	confHeader(const t_run *r, size_t i, t_conf *c)
{
	return (confCompute(r, i, c, NULL));
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
		t_conf	c;
		t_lists	l;
		int		v = r->o.verbose;

		j = confCompute(r, i, &c, &l);
		if (c.req == 0 && c.leak == 0 && c.nodiag == 0)
			continue ;
		confRow(r->spec.v[i].header, &c);
		listPrint("undeclared", l.undecl, l.nUndecl, v);
		listPrint("leaked", l.leak, l.nLeak, v);
		listPrint("accepted", l.nodiag, l.nNodiag, v);
		listPrint("not linked", l.unlinked, l.nUnlinked, v);
		listPrint("link error", l.broken, l.nBroken, v);
		listPrint("failing", l.fail, l.nFail, v);
		listPrint("not built", l.unbuilt, l.nUnbuilt, v);
		listPrint("untested", l.untested, l.nUntested, v);
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
		"\n  tested: at least one test ran (skipped and not implemented ones did not)."
		"\n  verified: tests ran and every one passed.%s\n", C_DIM, C_RESET);
	if (tot.unknown == tot.req)
		printf("  %sno probe results (run through make, or pass -P DIR)%s\n",
			C_DIM, C_RESET);
}
