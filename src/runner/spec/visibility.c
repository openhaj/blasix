/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file visibility.c
 * @brief What a header must hide, and what it must refuse.
 * @Created: 2026/11/24 08:16:27 by Moutig
 * @Updated: 2026/11/24 08:23:02 by Moutig
 *
 * Visibility (strict mode): an interface the target does not require
 * must be hidden unless its name is reserved for the header (spec
 * `reserved` lines, leading underscore), or, for Annex K, as K.3.1.1p1
 * says. Reject probes: translation units the standard requires the
 * implementation to refuse (K.3.1.1p4).
 */

#include "runner/lcrun.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

const char	*visName(t_vis v)
{
	static const char	*n[] = {
		[VIS_REQUIRED] = "required", [VIS_ALLOWED] = "may be visible",
		[VIS_HIDDEN] = "must be hidden", [VIS_UNCHECKED] = "header not in target",
	};

	return (n[v]);
}

/**
 * @brief The want macro of an option that gates one line of @p e and is
 *        off in @p t (Annex K: __STDC_WANT_LIB_EXT1__), for an edition
 *        that has the option (C11 on for Annex K).
 *
 * @param e Entry (both ladders are looked at).
 * @param t Target.
 * @return The macro name (without "=1"), or NULL.
 */
static const char	*offWant(const t_entry *e, const t_target *t)
{
	static char	name[64];

	for (const t_entry *l = e; l; l = (l == e) ? e->alt : NULL)
	{
		const t_optdef	*d = l->opt ? optFind(l->opt) : NULL;

		/* the option exists in the target (line included, not removed) */
		if (d && d->want && !targetHasOpt(t, l->opt) && stdIncludes(t, l->since)
			&& !stdIncludesAny(t, l->removed))
		{
			snprintf(name, sizeof(name), "%.*s", (int)strcspn(d->want, "="), d->want);
			return (name);
		}
	}
	return (NULL);
}

/**
 * @brief The reserved pattern of the header that @p name matches in @p t.
 *
 * @param sp   Database.
 * @param e    Entry (its category and name).
 * @param t    Target.
 * @return The pattern, "_*" for a leading underscore, or NULL.
 */
static const char	*reservedBy(const t_spec *sp, const t_entry *e, const t_target *t)
{
	if (e->name[0] == '_')
		return ("_* (ISO C 7.1.3, XSH 2.2.2)");
	for (size_t h = 0; h < sp->nHdrs; h++)
	{
		const t_hdr	*d = &sp->hdrs[h];

		if (strcmp(d->cat, e->cat))
			continue ;
		for (size_t k = 0; k < d->n; k++)
			if (stdIncludes(t, d->since[k]) && fnmatch(d->pat[k], e->name, 0) == 0)
				return (d->pat[k]);
	}
	return (NULL);
}

t_vis	specVisibility(const t_spec *sp, const t_entry *e, const t_target *t,
		const char **why, const char **want)
{
	const char	*w;
	const char	*pat;
	int			ob;

	if (why)
		*why = NULL;
	if (want)
		*want = NULL;
	if (reqSelected(specRequired(e, t, &ob)))
		return (VIS_REQUIRED);
	if (!specHeaderInTarget(sp, e->cat, t))
		return (VIS_UNCHECKED);
	w = offWant(e, t);
	if (w)
	{
		/* K.3.1.1p1: not declared when the want macro is 0, reserved or not */
		if (why)
			*why = "K.3.1.1p1";
		if (want)
			*want = w;
		return (VIS_HIDDEN);
	}
	pat = reservedBy(sp, e, t);
	if (pat)
	{
		if (why)
			*why = pat;
		return (VIS_ALLOWED);
	}
	if (why)
		*why = t->std->posix ? "XSH 2.2.1" : "ISO C 4p6, 7.1.3";
	return (VIS_HIDDEN);
}

size_t	specRejects(const t_spec *sp, const t_target *t, t_reject **out)
{
	size_t	n = 0;

	*out = NULL;
	for (size_t i = 0; i < sp->n; i++)
	{
		const t_entry	*e = &sp->v[i];
		const t_optdef	*d = e->opt ? optFind(e->opt) : NULL;
		int				ob;
		int				seen = 0;

		/* one header per category: first entry of a selected want option */
		if (!d || !d->want || specRequired(e, t, &ob) != REQ_OPTION)
			continue ;
		for (size_t k = 0; k < n && !seen; k++)
			seen = !strcmp((*out)[k].cat, e->cat);
		if (seen)
			continue ;
		for (int order = 0; order < 2; order++)
		{
			t_reject	*r;

			*out = xrealloc(*out, (n + 1) * sizeof(**out));
			r = &(*out)[n++];
			memset(r, 0, sizeof(*r));
			r->cat = e->cat;
			r->header = e->header;
			snprintf(r->want, sizeof(r->want), "%.*s", (int)strcspn(d->want, "="), d->want);
			r->first = !order;
			r->second = order;
			snprintf(r->id, sizeof(r->id), "__want_%s_%d_%d", d->name, r->first, r->second);
			r->ref = "K.3.1.1p4";
		}
	}
	return (n);
}
