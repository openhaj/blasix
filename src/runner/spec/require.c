/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file require.c
 * @brief What a target requires of an interface.
 * @Created: 2026/11/24 08:15:41 by Moutig
 * @Updated: 2026/11/24 08:21:59 by Moutig
 *
 * Each line of an entry is decided for the target (since, removed=, its
 * option group, base=); the strongest of the two ladders wins. Also: is a
 * header part of the target, is an entry a definition (a type or a
 * constant, tested by <category>/defs.c).
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

/**
 * @brief Requirement of one line (one ladder) of an entry.
 *
 * In order: an extension follows EXT=; then the line must be included
 * by the target (since), not removed (any version of removed=). An
 * option group makes it optional, except an ISO C feature before its
 * `from` edition, or a POSIX option from its base= edition on; an
 * optional line is required only when its option is selected.
 *
 * @param e           Line.
 * @param t           Target.
 * @param obsolescent Set to 1 if obsolescent in the target (ob=).
 * @return The requirement of this line.
 */
static t_req	lineRequired(const t_entry *e, const t_target *t, int *obsolescent)
{
	const t_optdef	*d;
	int				optional;

	*obsolescent = 0;
	if (!strcmp(e->since, "EXT"))
		return (t->ext ? REQ_EXT : REQ_NOT_YET);
	if (!stdIncludes(t, e->since))
		return (REQ_NOT_YET);
	if (stdIncludesAny(t, e->removed))
		return (REQ_REMOVED);
	optional = e->opt != NULL;
	d = e->opt ? optFind(e->opt) : NULL;
	if (optional && d && d->from && !stdIncludes(t, d->from))
		optional = 0;
	else if (optional && e->base && stdIncludes(t, e->base))
		optional = 0;
	if (optional && !targetHasOpt(t, e->opt))
		return (REQ_OPTION_OFF);
	*obsolescent = stdIncludesAny(t, e->ob);
	return (optional ? REQ_OPTION : REQ_BASE);
}

/**
 * @brief How strongly a requirement holds, to pick between two ladders.
 *
 * @param r Requirement.
 * @return 0 (not yet) ... 5 (base).
 */
static int	reqRank(t_req r)
{
	static const int	rank[] = {
		[REQ_NOT_YET] = 0, [REQ_REMOVED] = 1, [REQ_OPTION_OFF] = 2,
		[REQ_EXT] = 3, [REQ_OPTION] = 4, [REQ_BASE] = 5,
	};

	return (rank[r]);
}

t_req	specRequired(const t_entry *e, const t_target *t, int *obsolescent)
{
	t_req	q = lineRequired(e, t, obsolescent);
	int		ob2;
	t_req	q2;

	if (e->alt == NULL)
		return (q);
	q2 = lineRequired(e->alt, t, &ob2);
	if (reqRank(q2) > reqRank(q))
	{
		*obsolescent = ob2;
		return (q2);
	}
	return (q);
}

int	reqSelected(t_req r)
{
	return (r == REQ_BASE || r == REQ_OPTION || r == REQ_EXT);
}

const char	*reqName(t_req r)
{
	static const char	*n[] = {
		"not yet", "removed", "option off", "base", "option", "extension"
	};

	return (n[r]);
}

int	specHeaderInTarget(const t_spec *sp, const char *cat, const t_target *t)
{
	for (size_t i = 0; i < sp->n; i++)
	{
		int	ob;

		if (!strcmp(sp->v[i].cat, cat) && reqSelected(specRequired(&sp->v[i], t, &ob)))
			return (1);
	}
	return (0);
}

int	specIsDef(const t_entry *e, int ownUnit)
{
	return (e->kind == KIND_TYPE || (e->kind == KIND_MACRO && !ownUnit));
}
