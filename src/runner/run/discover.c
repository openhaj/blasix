/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file discover.c
 * @brief Discovery of a run: sources, units, tests, probe results.
 * @Created: 2026/11/24 08:14:52 by Moutig
 * @Updated: 2026/11/24 10:29:36 by Moutig
 *
 * Finds the sources of the suites (any group folder), the units the
 * target selects (and the <category>/defs.c of each of its headers), asks
 * each binary for its tests (--list), and reads the probe results.
 */

#include "runner/lcrun.h"

#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/** Bytes of a build log kept for the report (the rest is in the .err). */
#define LC_LOG_MAX	65536

/**
 * @brief The start of a file (build log of an unavailable unit).
 *
 * @param path File.
 * @return New string (at most LC_LOG_MAX bytes), or NULL if the file
 *         cannot be read.
 */
static char	*readLog(const char *path)
{
	FILE	*fp = fopen(path, "r");
	char	buf[4096];
	char	*s = NULL;
	size_t	total = 0;
	size_t	n;

	if (fp == NULL)
		return (NULL);
	while (total < LC_LOG_MAX && (n = fread(buf, 1, sizeof(buf), fp)) > 0)
	{
		if (n > LC_LOG_MAX - total)
			n = LC_LOG_MAX - total;
		strAppend(&s, buf, n);
		total += n;
	}
	fclose(fp);
	return (s);
}

/**
 * @brief What went wrong in a build log: the lines that say "error"
 *        (compiler, linker, LTO), at most @p maxLines; the first lines of
 *        the log when none does.
 *
 * @param log      Build log, or NULL.
 * @param maxLines Lines to keep.
 * @return New string, or NULL for a NULL or empty log.
 */
static char	*logErrors(const char *log, int maxLines)
{
	char	*s = NULL;
	int		n = 0;

	for (int pass = 0; pass < 2 && s == NULL; pass++)
		for (const char *p = log; p && *p && n < maxLines; )
		{
			const char	*nl = strchr(p, '\n');
			size_t		len = nl ? (size_t)(nl - p) + 1 : strlen(p);
			char		line[1024];

			snprintf(line, sizeof(line), "%.*s", (int)len, p);
			if (pass == 1 || strstr(line, "error:") || strstr(line, "undefined reference"))
			{
				strAppend(&s, p, len);
				n++;
			}
			p += len;
		}
	return (s);
}

/**
 * @brief Append every .c below @p abs (relative path @p rel) to @p v.
 *
 * @param abs Directory on disk.
 * @param cat Category the sources belong to.
 * @param rel Its path relative to the suites.
 * @param v   Array (grown).
 * @param n   Number of sources in @p v (updated).
 */
static void	scanDir(const char *abs, const char *cat, const char *rel, t_src **v, long *n)
{
	DIR				*d = opendir(abs);
	struct dirent	*f;

	if (d == NULL)
		return ;
	while ((f = readdir(d)) != NULL)
	{
		char		sub[4096];
		char		subRel[4096];
		size_t		len = strlen(f->d_name);
		struct stat	sb;

		if (f->d_name[0] == '.')
			continue ;
		snprintf(sub, sizeof(sub), "%s/%s", abs, f->d_name);
		if (stat(sub, &sb) == 0 && S_ISDIR(sb.st_mode))
		{
			snprintf(subRel, sizeof(subRel), "%s/%s", rel, f->d_name);
			scanDir(sub, cat, subRel, v, n);
			continue ;
		}
		if (len < 3 || strcmp(f->d_name + len - 2, ".c") != 0)
			continue ;
		*v = xrealloc(*v, (size_t)(*n + 1) * sizeof(**v));
		(*v)[*n].cat = xstrdup(cat);
		(*v)[*n].func = xstrndup(f->d_name, len - 2);
		snprintf(subRel, sizeof(subRel), "%s/%.*s", rel, (int)(len - 2), f->d_name);
		(*v)[*n].rel = xstrdup(subRel);
		(*n)++;
	}
	closedir(d);
}

/**
 * @brief qsort() order of sources: by path.
 *
 * @param a First source.
 * @param b Second source.
 * @return Negative, zero or positive.
 */
static int	cmpSrc(const void *a, const void *b)
{
	return (strcmp(((const t_src *)a)->rel, ((const t_src *)b)->rel));
}

long	suitesScan(const char *dir, t_src **out)
{
	DIR				*top = opendir(dir);
	struct dirent	*c;
	long			n = 0;

	*out = NULL;
	if (top == NULL)
		return (-1);
	while ((c = readdir(top)) != NULL)
	{
		char	abs[4096];

		if (c->d_name[0] == '.' || !strcmp(c->d_name, "bits"))
			continue ;
		snprintf(abs, sizeof(abs), "%s/%s", dir, c->d_name);
		scanDir(abs, c->d_name, c->d_name, out, &n);
	}
	closedir(top);
	if (n)
		qsort(*out, (size_t)n, sizeof(**out), cmpSrc);
	return (n);
}

void	suitesFree(t_src *v, long n)
{
	for (long i = 0; i < n; i++)
	{
		free(v[i].cat);
		free(v[i].func);
		free(v[i].rel);
	}
	free(v);
}

/**
 * @brief qsort() order of units: by category, then by function.
 *
 * @param a First unit.
 * @param b Second unit.
 * @return Negative, zero or positive.
 */
static int	cmpUnit(const void *a, const void *b)
{
	const t_unit	*x = a;
	const t_unit	*y = b;
	int				c = strcmp(x->cat, y->cat);

	return (c ? c : strcmp(x->func, y->func));
}

int	isDefsUnit(const t_src *s)
{
	char	rel[256];

	snprintf(rel, sizeof(rel), "%s/defs", s->cat);
	return (!strcmp(s->rel, rel));
}

/**
 * @brief With -f, does the filter name a definition of @p cat (a type or
 *        a constant, tested by <cat>/defs)?
 *
 * @param r   Run.
 * @param cat Category.
 * @return 1 without -f, or when a filtered name is a type or a macro of
 *         the category; 0 otherwise.
 */
static int	defsSelected(const t_run *r, const char *cat)
{
	if (r->o.funcs == NULL)
		return (1);
	for (char **f = r->o.funcs; *f; f++)
	{
		const t_entry	*e = specFind(&r->spec, cat, *f);

		if (e && (e->kind == KIND_TYPE || e->kind == KIND_MACRO))
			return (1);
	}
	return (0);
}

void	discoverUnits(t_run *r, int filters)
{
	t_src	*src;
	long	n = suitesScan(r->o.suitesDir, &src);

	if (n < 0)
	{
		fprintf(stderr, "lcrun: %s: %s\n", r->o.suitesDir, strerror(errno));
		exit(2);
	}
	for (long i = 0; i < n; i++)
	{
		const t_entry	*e;
		int				ob;

		if (isDefsUnit(&src[i]))
		{
			/* <cat>/defs: the types and constants of the header */
			if ((filters && (!inList(r->o.cats, src[i].cat) || !defsSelected(r, src[i].cat)))
				|| !specHeaderInTarget(&r->spec, src[i].cat, &r->o.target))
			{
				r->nUnitsOutOfTarget++;
				continue ;
			}
			r->units = xrealloc(r->units, (r->nUnits + 1) * sizeof(t_unit));
			r->units[r->nUnits++] = (t_unit){.cat = xstrdup(src[i].cat),
				.func = xstrdup(src[i].func), .rel = xstrdup(src[i].rel), .entry = NULL};
			continue ;
		}
		if (filters && (!inList(r->o.cats, src[i].cat) || !inList(r->o.funcs, src[i].func)))
			continue ;
		e = specFind(&r->spec, src[i].cat, src[i].func);
		if (e == NULL || !reqSelected(specRequired(e, &r->o.target, &ob)))
		{
			if (e == NULL)
				fprintf(stderr, "lcrun: %s/%s.c: no entry in %s/%s.spec, ignored\n",
					r->o.suitesDir, src[i].rel, r->o.specDir, src[i].cat);
			r->nUnitsOutOfTarget++;
			continue ;
		}
		r->units = xrealloc(r->units, (r->nUnits + 1) * sizeof(t_unit));
		r->units[r->nUnits++] = (t_unit){.cat = xstrdup(src[i].cat),
			.func = xstrdup(src[i].func), .rel = xstrdup(src[i].rel), .entry = e};
	}
	suitesFree(src, n);
	qsort(r->units, r->nUnits, sizeof(t_unit), cmpUnit);
}

/**
 * @brief A protocol field, "-" meaning absent.
 *
 * @param s Field.
 * @return New copy, or NULL for "-".
 */
static char	*optField(const char *s)
{
	return (strcmp(s, "-") ? xstrdup(s) : NULL);
}

/**
 * @brief Add one test from a "@test" line, if the run selects it.
 *
 * Dropped: another function than -f, a name that does not match -t,
 * since / until / opt that exclude the target (counted in nOutOfTarget).
 * An unknown version or option is reported and counted in nBadVersion
 * (the run then fails).
 *
 * @param r   Run.
 * @param u   Unit the line comes from.
 * @param fld The 11 fields after "@test" (see lc.c).
 */
static void	addTest(t_run *r, t_unit *u, char **fld)
{
	t_test	*t;
	char	key[1024];
	char	*since = optField(fld[7]);
	char	*until = optField(fld[8]);
	char	*opt = optField(fld[9]);

	if (!inList(r->o.funcs, fld[1]))
		return ;
	if ((since && !stdValid(since)) || (until && !stdValid(until))
		|| (opt && !optFind(opt)))
	{
		/* A typo would silently drop the test from every target. */
		fprintf(stderr, "lcrun: %s:%s: %s/%s: unknown .since / .until / .opt "
			"(\"%s\" \"%s\" \"%s\"; make standards, make options)\n", fld[3], fld[4],
			fld[1], fld[2], since ? since : "-", until ? until : "-", opt ? opt : "-");
		r->nBadVersion++;
		free(since);
		free(until);
		free(opt);
		return ;
	}
	if (opt && !targetHasOpt(&r->o.target, opt)
		&& !(optFind(opt)->from && !stdIncludes(&r->o.target, optFind(opt)->from)))
	{
		r->nOutOfTarget++;
		free(since);
		free(until);
		free(opt);
		return ;
	}
	free(opt);
	snprintf(key, sizeof(key), "%s/%s/%s", fld[0], fld[1], fld[2]);
	if (r->o.glob && fnmatch(r->o.glob, key, 0) != 0)
		return ;
	if (!stdIncludes(&r->o.target, since) || (until && stdIncludes(&r->o.target, until)))
	{
		r->nOutOfTarget++;
		free(since);
		free(until);
		return ;
	}
	r->tests = xrealloc(r->tests, (r->nTests + 1) * sizeof(t_test));
	t = &r->tests[r->nTests++];
	memset(t, 0, sizeof(*t));
	t->unit = u;
	t->cat = xstrdup(fld[0]);
	t->func = xstrdup(fld[1]);
	t->name = xstrdup(fld[2]);
	t->file = xstrdup(fld[3]);
	t->line = atoi(fld[4]);
	t->timeout = atoi(fld[5]);
	t->expect = atoi(fld[6]);
	t->since = since;
	t->until = until;
	t->ref = optField(fld[10]);
	t->code = -1;
}

/**
 * @brief Ask one binary for its tests (bin --list) and add them.
 *
 * fd 3 is closed for the child, so the framework writes for lcrun on
 * fd 1 instead of using the event protocol.
 *
 * @param r Run.
 * @param u Unit with a binary.
 */
static void	listUnit(t_run *r, t_unit *u)
{
	char	cmd[4200];
	char	line[4096];
	FILE	*p;

	snprintf(cmd, sizeof(cmd), "'%s' --list 3>&-", u->bin);
	p = popen(cmd, "r");
	if (p == NULL)
		return ;
	while (fgets(line, sizeof(line), p))
	{
		char	*fld[11];
		char	*s = line + 6;
		int		n = 0;

		if (strncmp(line, "@test\t", 6) != 0)
			continue ;
		line[strcspn(line, "\n")] = '\0';
		while (n < 11 && s)
		{
			fld[n++] = s;
			s = strchr(s, '\t');
			if (s)
				*s++ = '\0';
		}
		if (n == 11)
			addTest(r, u, fld);
	}
	if (pclose(p) != 0 && !r->o.quiet)
		fprintf(stderr, "lcrun: %s --list failed\n", u->bin);
}

/**
 * @brief qsort() order of tests: by unit, then by line in the source.
 *
 * Section order in the binary is not source order, hence the sort.
 *
 * @param a First test.
 * @param b Second test.
 * @return Negative, zero or positive.
 */
static int	cmpTest(const void *a, const void *b)
{
	const t_test	*x = a;
	const t_test	*y = b;

	if (x->unit != y->unit)
		return ((x->unit > y->unit) - (x->unit < y->unit));
	return ((x->line > y->line) - (x->line < y->line));
}

/**
 * @brief Read <probedir>/<cat>/<name>.status (declaration probe) and
 *        <name>.hidden (hidden probe: "leaked" or "hidden") for every
 *        spec entry, and <id>.reject ("rejected" or "accepted") for every
 *        reject probe of the target.
 *
 * Left PR_UNKNOWN / HID_UNKNOWN without -P or for an interface that was
 * not probed; hidden probes are ignored in lax mode.
 *
 * @param r Run, r->probes and r->hidden allocated (one per entry).
 */
static void	readProbes(t_run *r)
{
	char	path[4096];
	char	word[32];

	r->probes = xmalloc(r->spec.n * sizeof(*r->probes));
	r->hidden = xmalloc(r->spec.n * sizeof(*r->hidden));
	for (size_t i = 0; i < r->spec.n; i++)
	{
		FILE	*fp;

		r->probes[i] = PR_UNKNOWN;
		r->hidden[i] = HID_UNKNOWN;
		if (r->o.probeDir == NULL)
			continue ;
		snprintf(path, sizeof(path), "%s/%s/%s.hidden", r->o.probeDir,
			r->spec.v[i].cat, r->spec.v[i].name);
		fp = r->o.lax ? NULL : fopen(path, "r");
		if (fp && fscanf(fp, "%31s", word) == 1)
			r->hidden[i] = !strcmp(word, "leaked") ? HID_LEAKED : HID_HIDDEN;
		if (fp)
			fclose(fp);
		snprintf(path, sizeof(path), "%s/%s/%s.status", r->o.probeDir,
			r->spec.v[i].cat, r->spec.v[i].name);
		fp = fopen(path, "r");
		if (fp == NULL)
			continue ;
		if (fscanf(fp, "%31s", word) == 1)
			r->probes[i] = !strcmp(word, "ok") ? PR_OK
				: !strcmp(word, "unlinked") ? PR_UNLINKED
				: !strcmp(word, "broken") ? PR_BROKEN : PR_UNDECLARED;
		fclose(fp);
	}
	r->nRejects = specRejects(&r->spec, &r->o.target, &r->rejects);
	for (size_t i = 0; i < r->nRejects && r->o.probeDir; i++)
	{
		FILE	*fp;

		snprintf(path, sizeof(path), "%s/%s/%s.reject", r->o.probeDir,
			r->rejects[i].cat, r->rejects[i].id);
		fp = fopen(path, "r");
		if (fp && fscanf(fp, "%31s", word) == 1)
			r->rejects[i].st = !strcmp(word, "accepted") ? REJ_ACCEPTED : REJ_REJECTED;
		if (fp)
			fclose(fp);
	}
}

void	discover(t_run *r)
{
	char		path[4096];
	struct stat	sb;

	discoverUnits(r, 1);
	for (size_t i = 0; i < r->nUnits; i++)
	{
		t_unit	*u = &r->units[i];

		snprintf(path, sizeof(path), "%s/%s", r->o.binDir, u->rel);
		if (stat(path, &sb) == 0 && (sb.st_mode & S_IXUSR))
		{
			u->bin = xstrdup(path);
			listUnit(r, u);
		}
		else
		{
			snprintf(path, sizeof(path), "%s/%s.err", r->o.binDir, u->rel);
			u->log = readLog(path);
			u->err = logErrors(u->log, 6);
		}
	}
	qsort(r->tests, r->nTests, sizeof(t_test), cmpTest);
	readProbes(r);
}
