/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file list.c
 * @brief lcrun --spec-list and --options.
 * @Created: 2026/11/24 08:18:00 by Moutig
 * @Updated: 2026/11/24 08:24:05 by Moutig
 *
 * Every interface and what the target says of it (requirement,
 * visibility, notes); the option groups.
 */

#include "runner/lcrun.h"

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * @brief Append the notes of one line to @p notes (base, ob, removed, ext).
 *
 * @param e     Line.
 * @param notes NUL-terminated buffer.
 * @param size  Size of @p notes.
 */
static void	lineNotes(const t_entry *e, char *notes, size_t size)
{
	size_t	len = strlen(notes);

	if (e->base && len < size)
		len += (size_t)snprintf(notes + len, size - len, "base in %s; ", e->base);
	if (e->ob && len < size)
		len += (size_t)snprintf(notes + len, size - len, "obsolescent in %s; ", e->ob);
	if (e->removed && len < size)
		len += (size_t)snprintf(notes + len, size - len, "removed in %s; ", e->removed);
	if (e->ext && len < size)
		snprintf(notes + len, size - len, "%s extension; ", e->ext);
}

int	toolList(const t_run *r)
{
	char	buf[512];
	int		count[REQ_EXT + 1] = {0};
	int		obs = 0;

	targetDescribe(&r->o.target, buf, sizeof(buf));
	printf("target: %s\n\n", buf);
	printf("%-12s %-26s %-8s %-13s %-8s %-11s %s\n", "category", "name",
		"kind", "since", "option", "target", "notes");
	for (size_t i = 0; i < r->spec.n; i++)
	{
		const t_entry	*e = &r->spec.v[i];
		int				ob;
		t_req			q = specRequired(e, &r->o.target, &ob);
		char			notes[512] = "";

		count[q]++;
		obs += ob;
		lineNotes(e, notes, sizeof(notes));
		if (e->alt)
		{
			size_t	len = strlen(notes);

			snprintf(notes + len, sizeof(notes) - len, "also %s%s%s; ", e->alt->since,
				e->alt->opt ? " " : "", e->alt->opt ? e->alt->opt : "");
			lineNotes(e->alt, notes, sizeof(notes));
		}
		if (!reqSelected(q))
		{
			const char	*why;
			t_vis		v = specVisibility(&r->spec, e, &r->o.target, &why, NULL);
			size_t		len = strlen(notes);

			snprintf(notes + len, sizeof(notes) - len, "%s%s%s%s", visName(v),
				why ? " (" : "", why ? why : "", why ? ")" : "");
		}
		if (!r->o.verbose && !reqSelected(q))
			continue ;
		printf("%-12s %-26s %-8s %-13s %-8s %-11s %s\n", e->cat, e->name,
			kindName(e->kind), e->since, e->opt ? e->opt : "-", reqName(q), notes);
	}
	printf("\n%d required (%d obsolescent), %d by options, %d extensions; "
		"excluded: %d not yet, %d removed, %d option off%s\n",
		count[REQ_BASE], obs, count[REQ_OPTION], count[REQ_EXT],
		count[REQ_NOT_YET], count[REQ_REMOVED], count[REQ_OPTION_OFF],
		r->o.verbose ? "" : " (-v lists them)");
	return (0);
}

int	toolOptions(void)
{
	const t_optdef	*d;

	printf("%-16s %-6s %-5s %-36s %s\n", "option", "family", "from", "defines",
		"description");
	for (size_t i = 0; (d = optAt(i)) != NULL; i++)
		printf("%-16s %-6s %-5s %-36s %s\n", d->name, d->from ? "ISO C" : "POSIX",
			d->from ? d->from : "-", d->want ? d->want : "-", d->desc);
	printf("\nISO C options are conditional features: required before `from`,\n"
		"optional from it on. POSIX options exist on POSIX targets only.\n"
		"Select with OPTS=A,B (make) or -O A,B (lcrun); in a test, .opt = \"XSI\".\n");
	return (0);
}
