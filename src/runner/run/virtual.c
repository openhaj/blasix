/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file virtual.c
 * @brief Results that come from the spec and the probes, not from a unit.
 * @Created: 2026/11/24 12:00:00 by Moutig
 * @Updated: 2026/11/24 11:55:23 by Moutig
 *
 * Added to the tests of a run once they ran, so that the report, the
 * totals and the exit status show them like any other result:
 *
 *   <cat>/<name>/@not_in_target  SKIP: the target does not require the
 *                                interface (not yet, removed, option off,
 *                                extension off), with why;
 *   <cat>/<name>/@hidden         strict mode, for an interface that must
 *                                be hidden: PASS when the header hides it,
 *                                FAIL when it is visible anyway (a leak);
 *   <cat>/<want>/@reject_<id>    a translation unit the standard requires
 *                                to be refused: PASS when refused, FAIL
 *                                when it compiled (K.3.1.1p4).
 *
 * Only for the headers of the target (nothing is said of <strings.h> on
 * an ISO C target), filtered by -c, -f and -t like the tests.
 */

#include "runner/lcrun.h"

#include <fnmatch.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Append a result to the run.
 *
 * @param r    Run.
 * @param e    Spec entry it is about (its file and line are shown), or
 *             NULL.
 * @param cat  Category.
 * @param func Interface (or the want macro of a reject probe).
 * @param name Result name ("@hidden", ...).
 * @param st   Status.
 * @param msg  Why, one line (no newline), or NULL.
 * @param ref  Clause, or NULL.
 */
static void	addResult(t_run *r, const t_entry *e, const char *cat, const char *func,
		const char *name, t_status st, const char *msg, const char *ref)
{
	t_test	*t;
	char	key[1024];

	snprintf(key, sizeof(key), "%s/%s/%s", cat, func, name);
	if (r->o.glob && fnmatch(r->o.glob, key, 0) != 0)
		return ;
	r->tests = xrealloc(r->tests, (r->nTests + 1) * sizeof(t_test));
	t = &r->tests[r->nTests++];
	memset(t, 0, sizeof(*t));
	t->cat = xstrdup(cat);
	t->func = xstrdup(func);
	t->name = xstrdup(name);
	t->file = xstrdup(e ? e->file : "spec");
	t->line = e ? e->line : 0;
	t->ref = ref ? xstrdup(ref) : NULL;
	t->st = st;
	t->runs = 1;
	t->passes = (st == ST_PASS);
	t->asserts = 1;
	t->failed = (st == ST_FAIL);
	t->code = 0;
	if (msg)
	{
		strAppend(&t->msgs, msg, strlen(msg));
		strAppend(&t->msgs, "\n", 1);
	}
}

/**
 * @brief Why the target does not require @p e: "removed in POSIX.1-2008",
 *        "not yet: since C23", "option XSI off", "extension (EXT=1)".
 *
 * @param e    Entry (both ladders are looked at).
 * @param q    Its requirement in the target.
 * @param buf  Destination.
 * @param size Size of @p buf.
 */
static void	notInTargetWhy(const t_entry *e, t_req q, char *buf, size_t size)
{
	const t_entry	*l = e;

	/* the line that says so: the POSIX one when both are excluded alike */
	if (e->alt && (q == REQ_REMOVED ? e->alt->removed != NULL
		: q == REQ_OPTION_OFF ? e->alt->opt != NULL : 0))
		l = e->alt;
	if (q == REQ_REMOVED)
		snprintf(buf, size, "removed in %s", l->removed);
	else if (q == REQ_OPTION_OFF)
		snprintf(buf, size, "option %s off (OPTS=)", l->opt);
	else if (!strcmp(l->since, "EXT"))
		snprintf(buf, size, "%s extension (EXT=1)", l->ext ? l->ext : "an");
	else if (e->alt)
		snprintf(buf, size, "not yet: since %s (%s)", e->since, e->alt->since);
	else
		snprintf(buf, size, "not yet: since %s", e->since);
}

void	addVirtualResults(t_run *r)
{
	char	target[256];
	char	why[256];
	char	msg[768];

	targetDescribe(&r->o.target, target, sizeof(target));
	for (size_t i = 0; i < r->spec.n; i++)
	{
		const t_entry	*e = &r->spec.v[i];
		const char		*vwhy;
		int				ob;
		t_req			q = specRequired(e, &r->o.target, &ob);
		t_vis			v;

		if (reqSelected(q) || !inList(r->o.cats, e->cat) || !inList(r->o.funcs, e->name)
			|| !specHeaderInTarget(&r->spec, e->cat, &r->o.target))
			continue ;
		notInTargetWhy(e, q, why, sizeof(why));
		v = specVisibility(&r->spec, e, &r->o.target, &vwhy, NULL);
		if (v == VIS_ALLOWED)
			snprintf(msg, sizeof(msg), "not in %s: %s; may stay visible (%s is reserved for "
				"<%s>)", target, why, vwhy, e->header);
		else if (v == VIS_HIDDEN && !r->o.lax)
			snprintf(msg, sizeof(msg), "not in %s: %s; must be hidden (%s)", target, why, vwhy);
		else
			snprintf(msg, sizeof(msg), "not in %s: %s", target, why);
		addResult(r, e, e->cat, e->name, "@not_in_target", ST_SKIP, msg, NULL);
		if (r->hidden[i] == HID_UNKNOWN)
			continue ;
		if (r->hidden[i] == HID_HIDDEN)
			addResult(r, e, e->cat, e->name, "@hidden", ST_PASS, NULL, vwhy);
		else
		{
			snprintf(msg, sizeof(msg), "<%s> makes %s visible, but it is not in %s (%s) and "
				"its name is not reserved for the header: it must be hidden", e->header,
				e->name, target, why);
			addResult(r, e, e->cat, e->name, "@hidden", ST_FAIL, msg, vwhy);
		}
	}
	for (size_t i = 0; i < r->nRejects; i++)
	{
		const t_reject	*rj = &r->rejects[i];
		char			name[128];

		if (rj->st == REJ_UNKNOWN || !inList(r->o.cats, rj->cat))
			continue ;
		snprintf(name, sizeof(name), "@reject_%d_%d", rj->first, rj->second);
		if (rj->st == REJ_REJECTED)
			addResult(r, NULL, rj->cat, rj->want, name, ST_PASS, NULL, rj->ref);
		else
		{
			snprintf(msg, sizeof(msg), "compiled: %s defined as %d, then %d, for two "
				"inclusions of <%s>; the implementation must refuse it (a diagnostic as "
				"if by #error)", rj->want, rj->first, rj->second, rj->header);
			addResult(r, NULL, rj->cat, rj->want, name, ST_FAIL, msg, rj->ref);
		}
	}
}
