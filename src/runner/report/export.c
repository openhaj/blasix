/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file export.c
 * @brief Results of a run as TSV and JSON.
 * @Created: 2026/11/24 08:14:32 by Moutig
 * @Updated: 2026/11/24 11:55:19 by Moutig
 *
 * TSV: one row per test (the @not_in_target, @hidden and @reject_* results
 * included), plus one @interface row per required interface
 * (read back by --compare). JSON: every field, the target, the mode, every
 * unit that did not build (with why and its log), every required
 * interface with its probe, tests and verdict, the conformance counters of
 * each header (the console table), every hidden and reject probe.
 */

#include "runner/lcrun.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Name of a probe result in TSV / JSON ("present", ...).
 *
 * @param p Probe result.
 * @return Static string.
 */
static const char	*probeName(t_probe p)
{
	static const char	*n[] = {"unprobed", "present", "undeclared", "unlinked", "broken"};

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
		fprintf(fp, ", \"until\": ");
		jsonStr(fp, t->until);
		fprintf(fp, ", \"unit\": ");
		jsonStr(fp, t->unit ? t->unit->rel : NULL);
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
		fprintf(fp, ", \"unit\": ");
		jsonStr(fp, r->units[i].rel);
		fprintf(fp, ", \"reason\": \"%s\", \"error\": ", unitReason(r, &r->units[i]));
		jsonStr(fp, r->units[i].err);
		fprintf(fp, ", \"log\": ");
		jsonStr(fp, r->units[i].log);
		fprintf(fp, "}");
	}
	fprintf(fp, "\n  ],\n  \"interfaces\": [");
	for (size_t i = 0, k = 0; i < r->spec.n; i++)
	{
		const t_entry	*e = &r->spec.v[i];
		int				ob;
		t_req			q = specRequired(e, &r->o.target, &ob);
		t_iresult		x;

		if (!reqSelected(q) || interfaceFiltered(r, i))
			continue ;
		interfaceResult(r, i, &x);
		fprintf(fp, "%s\n    {\"header\": ", k++ ? "," : "");
		jsonStr(fp, e->header);
		fprintf(fp, ", \"category\": ");
		jsonStr(fp, e->cat);
		fprintf(fp, ", \"name\": ");
		jsonStr(fp, e->name);
		fprintf(fp, ", \"kind\": \"%s\", \"since\": ", kindName(e->kind));
		jsonStr(fp, e->since);
		fprintf(fp, ", \"option\": ");
		jsonStr(fp, q == REQ_OPTION ? e->opt : NULL);
		fprintf(fp, ", \"requirement\": \"%s\", \"obsolescent\": %s, "
			"\"probe\": \"%s\", \"unit\": ", reqName(q), ob ? "true" : "false",
			probeName(r->probes[i]));
		jsonStr(fp, x.unit ? x.unit->rel : NULL);
		fprintf(fp, ", \"built\": %s, \"tests\": %d, \"passed\": %d, \"failed\": %d, "
			"\"skipped\": %d, \"unimplemented\": %d, \"verdict\": \"%s\"}",
			x.unit == NULL ? "null" : x.unit->bin ? "true" : "false",
			x.tests, x.passed, x.failed, x.skipped, x.unimpl, verdictName(x.verdict));
	}
	/* the conformance table: one row per header, as the console prints it */
	fprintf(fp, "\n  ],\n  \"conformance\": [");
	for (size_t i = 0, k = 0, next; i < r->spec.n; i = next)
	{
		t_conf	c;

		next = confHeader(r, i, &c);
		if (c.req == 0 && c.leak == 0 && c.nodiag == 0)
			continue ;
		fprintf(fp, "%s\n    {\"header\": ", k++ ? "," : "");
		jsonStr(fp, r->spec.v[i].header);
		fprintf(fp, ", \"category\": ");
		jsonStr(fp, r->spec.v[i].cat);
		fprintf(fp, ", \"required\": %d, \"option\": %d, \"obsolescent\": %d, "
			"\"present\": %d, \"undeclared\": %d, \"unlinked\": %d, \"broken\": %d, "
			"\"unprobed\": %d, \"tested\": %d, \"verified\": %d, \"failing\": %d, "
			"\"unbuilt\": %d, \"untested\": %d, \"leaked\": %d, \"nodiag\": %d}",
			c.req, c.opt, c.ob, c.ok, c.undecl, c.unlinked, c.broken, c.unknown,
			c.tested, c.pass, c.fail, c.unbuilt, c.untested, c.leak, c.nodiag);
	}
	/* strict mode: the interfaces that must be hidden, and whether they are */
	fprintf(fp, "\n  ],\n  \"mode\": \"%s\",\n  \"hidden\": [", r->o.lax ? "lax" : "strict");
	for (size_t i = 0, k = 0; i < r->spec.n; i++)
	{
		if (r->hidden[i] == HID_UNKNOWN)
			continue ;
		fprintf(fp, "%s\n    {\"header\": ", k++ ? "," : "");
		jsonStr(fp, r->spec.v[i].header);
		fprintf(fp, ", \"name\": ");
		jsonStr(fp, r->spec.v[i].name);
		fprintf(fp, ", \"leaked\": %s}", r->hidden[i] == HID_LEAKED ? "true" : "false");
	}
	fprintf(fp, "\n  ],\n  \"rejects\": [");
	for (size_t i = 0, k = 0; i < r->nRejects; i++)
	{
		if (r->rejects[i].st == REJ_UNKNOWN)
			continue ;
		fprintf(fp, "%s\n    {\"header\": ", k++ ? "," : "");
		jsonStr(fp, r->rejects[i].header);
		fprintf(fp, ", \"probe\": ");
		jsonStr(fp, r->rejects[i].id);
		fprintf(fp, ", \"clause\": ");
		jsonStr(fp, r->rejects[i].ref);
		fprintf(fp, ", \"refused\": %s}", r->rejects[i].st == REJ_REJECTED ? "true" : "false");
	}
	fprintf(fp, "\n  ]\n}\n");
	return (fclose(fp));
}
