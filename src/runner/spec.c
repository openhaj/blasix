/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file spec.c
 * @brief Standards, option groups, the spec database and targets.
 * @Created: 2026/11/23 04:42:37 by Moutig
 * @Updated: 2026/11/24 10:00:00 by Moutig
 *
 * Standards. The 4 ISO C editions and the 4 POSIX.1 editions form two
 * ladders; a POSIX edition is built on an ISO C edition (its "C base"):
 *
 *   id          name           C base   _POSIX_C_SOURCE  _XOPEN_SOURCE
 *   c99         C99            C99      -                -
 *   c11         C11            C11      -                -
 *   c17         C17            C17      -                -
 *   c23         C23            C23      -                -
 *   posix-2001  POSIX.1-2001   C99      200112L          600
 *   posix-2008  POSIX.1-2008   C99      200809L          700
 *   posix-2017  POSIX.1-2017   C99      200809L          700
 *   posix-2024  POSIX.1-2024   C17      202405L          800
 *
 * A target includes version V when V is an ISO C edition not newer than
 * its C base, or a POSIX edition not newer than itself.
 *
 * Option groups (gOpts below):
 *   - POSIX options: the margin codes of XBD 1.7.1 (XSI, TSF, ADV, ...).
 *     Only on a POSIX target, required only when selected (OPTS=);
 *   - ISO C conditional features (THREADS, COMPLEX, LIB_EXT1, ...):
 *     required before their `from` edition, optional from it on, then
 *     required only when selected.
 *
 * Spec file format (spec/<cat>.spec, one per header; see CONTRIBUTING.md):
 *
 *   header string.h
 *   # name     since          tags
 *   memcpy     C99
 *   memccpy    C23
 *   memccpy    POSIX.1-2001   XSI                (other ladder: see alt)
 *   strtok_r   POSIX.1-2001   TSF base=POSIX.1-2008
 *   gets       C99            ob=POSIX.1-2008 removed=C11
 *   asctime    C99            ob=POSIX.1-2008,C23
 *   thrd_create C11           THREADS
 *   setjmp     C99            either
 *   optarg     POSIX.1-2001   var
 *   stdin      C99            macro
 *   FILE       C99            type
 *   memrchr    EXT            ext=GNU
 *
 * Tags: a kind (`macro`, `type`, `either`: macro or function, `var`:
 * object with linkage; default function), an option group, base=V
 * (option moved to the base in V), ob=V,... (obsolescent from any of
 * them), removed=V,..., ext=NAME (extension, since EXT).
 */

#include "runner/lcrun.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* ----- Standards ----- */

/**
 * The 8 standards, ISO C ladder first: id, name, ISO C level (of the C
 * base for POSIX), POSIX level, _POSIX_C_SOURCE, _XOPEN_SOURCE, -std=.
 * C23 compiles with -std=c2x, accepted by GCC and clang before C23 had
 * its final name.
 */
static const t_std	gStds[] = {
	{"c99", "C99", 1, 0, 0, 0, "c99"},
	{"c11", "C11", 2, 0, 0, 0, "c11"},
	{"c17", "C17", 3, 0, 0, 0, "c17"},
	{"c23", "C23", 4, 0, 0, 0, "c2x"},
	{"posix-2001", "POSIX.1-2001", 1, 1, 200112L, 600, "c99"},
	{"posix-2008", "POSIX.1-2008", 1, 2, 200809L, 700, "c99"},
	{"posix-2017", "POSIX.1-2017", 1, 3, 200809L, 700, "c99"},
	{"posix-2024", "POSIX.1-2024", 3, 4, 202405L, 800, "c17"},
};

/** Number of standards. */
#define N_STDS	(sizeof(gStds) / sizeof(gStds[0]))

/* ----- Option groups ----- */

/**
 * Every option group a spec line or a test may name: name, `from`
 * (ISO C edition where the feature becomes conditional; NULL for a
 * POSIX option), macro to define when selected, description.
 * CX (extension to ISO C) and OB (obsolescent) are margin codes too,
 * but not options: CX is base POSIX, OB is the ob= tag.
 */
static const t_optdef	gOpts[] = {
	/* ISO C conditional features. */
	{"COMPLEX", "C11", NULL, "Complex arithmetic <complex.h> (__STDC_NO_COMPLEX__)"},
	{"THREADS", "C11", NULL, "Threads <threads.h> (__STDC_NO_THREADS__)"},
	{"ATOMICS", "C11", NULL, "Atomics <stdatomic.h> (__STDC_NO_ATOMICS__)"},
	{"LIB_EXT1", "C11", "__STDC_WANT_LIB_EXT1__=1",
		"Bounds-checking interfaces, Annex K (__STDC_LIB_EXT1__)"},
	{"IEC_60559", "C99", NULL, "IEC 60559 floating point, Annex F (__STDC_IEC_559__)"},
	{"DFP", "C23", NULL, "Decimal floating point (__STDC_IEC_60559_DFP__)"},
	{"IEC_60559_TYPES", "C23", "__STDC_WANT_IEC_60559_TYPES_EXT__=1",
		"Interchange and extended types, Annex H (__STDC_IEC_60559_TYPES__)"},
	/* POSIX margin codes (XBD 1.7.1) that mark library interfaces. */
	{"ADV", NULL, NULL, "Advisory Information"},
	{"AIO", NULL, NULL, "Asynchronous Input and Output"},
	{"BAR", NULL, NULL, "Barriers"},
	{"CPT", NULL, NULL, "Process CPU-Time Clocks"},
	{"CS", NULL, NULL, "Clock Selection"},
	{"DC", NULL, NULL, "Device Control"},
	{"FSC", NULL, NULL, "File Synchronization"},
	{"IP6", NULL, NULL, "IPV6"},
	{"MC1", NULL, NULL, "Advisory Information and Memory Mapped Files or Shared Memory Objects"},
	{"MC2", NULL, NULL, "Memory Mapped Files, Shared Memory Objects, or Memory Protection"},
	{"MC3", NULL, NULL, "Memory Mapped Files, Shared Memory Objects, or Typed Memory Objects"},
	{"MF", NULL, NULL, "Memory Mapped Files"},
	{"ML", NULL, NULL, "Process Memory Locking"},
	{"MLR", NULL, NULL, "Range Memory Locking"},
	{"MON", NULL, NULL, "Monotonic Clock"},
	{"MPR", NULL, NULL, "Memory Protection"},
	{"MSG", NULL, NULL, "Message Passing"},
	{"MX", NULL, NULL, "IEC 60559 Floating-Point"},
	{"MXC", NULL, NULL, "IEC 60559 Complex Floating-Point"},
	{"MXX", NULL, NULL, "IEC 60559 Floating-Point Extension"},
	{"PIO", NULL, NULL, "Prioritized Input and Output"},
	{"PS", NULL, NULL, "Process Scheduling"},
	{"RPI", NULL, NULL, "Robust Mutex Priority Inheritance"},
	{"RPP", NULL, NULL, "Robust Mutex Priority Protection"},
	{"RS", NULL, NULL, "Raw Sockets"},
	{"RTS", NULL, NULL, "Realtime Signals Extension"},
	{"SEM", NULL, NULL, "Semaphores"},
	{"SHM", NULL, NULL, "Shared Memory Objects"},
	{"SIO", NULL, NULL, "Synchronized Input and Output"},
	{"SPI", NULL, NULL, "Spin Locks"},
	{"SPN", NULL, NULL, "Spawn"},
	{"SS", NULL, NULL, "Process Sporadic Server"},
	{"TCT", NULL, NULL, "Thread CPU-Time Clocks"},
	{"TEF", NULL, NULL, "Trace Event Filter"},
	{"THR", NULL, NULL, "Threads"},
	{"TMO", NULL, NULL, "Timeouts"},
	{"TMR", NULL, NULL, "Timers"},
	{"TPI", NULL, NULL, "Non-Robust Mutex Priority Inheritance"},
	{"TPP", NULL, NULL, "Non-Robust Mutex Priority Protection"},
	{"TPS", NULL, NULL, "Thread Execution Scheduling"},
	{"TRC", NULL, NULL, "Trace"},
	{"TRI", NULL, NULL, "Trace Inherit"},
	{"TRL", NULL, NULL, "Trace Log"},
	{"TSA", NULL, NULL, "Thread Stack Address Attribute"},
	{"TSF", NULL, NULL, "Thread-Safe Functions"},
	{"TSH", NULL, NULL, "Thread Process-Shared Synchronization"},
	{"TSP", NULL, NULL, "Thread Sporadic Server"},
	{"TSS", NULL, NULL, "Thread Stack Size Attribute"},
	{"TYM", NULL, NULL, "Typed Memory Objects"},
	{"XSI", NULL, NULL, "X/Open System Interfaces"},
	{"XSR", NULL, NULL, "XSI STREAMS"},
};

/** Number of option groups. */
#define N_OPTS	(sizeof(gOpts) / sizeof(gOpts[0]))

const t_std	*stdFind(const char *s)
{
	for (size_t i = 0; i < N_STDS; i++)
		if (!strcasecmp(s, gStds[i].id) || !strcasecmp(s, gStds[i].name))
			return (&gStds[i]);
	return (NULL);
}

const t_std	*stdAt(size_t i)
{
	return (i < N_STDS ? &gStds[i] : NULL);
}

int	stdValid(const char *v)
{
	return (!strcmp(v, "EXT") || stdFind(v) != NULL);
}

void	stdPrintAll(FILE *fp)
{
	for (size_t i = 0; i < N_STDS; i++)
		fprintf(fp, "  %-11s %-13s %s\n", gStds[i].id, gStds[i].name,
			gStds[i].posix ? "POSIX (its C base + system interfaces)" : "ISO C");
}

int	stdIncludes(const t_target *t, const char *v)
{
	const t_std	*s;

	if (v == NULL || *v == '\0')
		return (1);
	if (!strcmp(v, "EXT"))
		return (t->ext);
	s = stdFind(v);
	if (s == NULL)
		return (0);
	if (s->posix == 0)
		return (s->c <= t->std->c);
	return (t->std->posix != 0 && s->posix <= t->std->posix);
}

int	stdIncludesAny(const t_target *t, const char *list)
{
	char	buf[256];
	char	*save;

	if (list == NULL)
		return (0);
	snprintf(buf, sizeof(buf), "%s", list);
	for (char *v = strtok_r(buf, ",", &save); v; v = strtok_r(NULL, ",", &save))
		if (stdIncludes(t, v))
			return (1);
	return (0);
}

const t_optdef	*optFind(const char *name)
{
	for (size_t i = 0; i < N_OPTS; i++)
		if (!strcasecmp(gOpts[i].name, name))
			return (&gOpts[i]);
	return (NULL);
}

const t_optdef	*optAt(size_t i)
{
	return (i < N_OPTS ? &gOpts[i] : NULL);
}

int	targetHasOpt(const t_target *t, const char *opt)
{
	for (char **o = t->opts; o && *o; o++)
		if (!strcasecmp(*o, opt))
			return (1);
	return (0);
}

int	targetCheck(const t_target *t)
{
	for (char **o = t->opts; o && *o; o++)
	{
		const t_optdef	*d = optFind(*o);

		if (d == NULL)
		{
			fprintf(stderr, "lcrun: unknown option group '%s' (make options)\n", *o);
			return (-1);
		}
		if (d->from == NULL && t->std->posix == 0)
		{
			fprintf(stderr, "lcrun: %s is a POSIX option, %s is an ISO C target\n",
				d->name, t->std->name);
			return (-1);
		}
	}
	return (0);
}

/* ----- Requirement of one entry ----- */

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

/* ----- Database ----- */

/**
 * @brief Report an error of a spec file ("file:line: msg: what") and count it.
 *
 * @param sp   Database (errors incremented).
 * @param file Spec file.
 * @param line Line number.
 * @param msg  Message.
 * @param what Offending token, or NULL.
 */
static void	parseErr(t_spec *sp, const char *file, int line, const char *msg,
		const char *what)
{
	fprintf(stderr, "%s:%d: %s%s%s\n", file, line, msg, what ? ": " : "",
		what ? what : "");
	sp->errors++;
}

/**
 * @brief Is every version of a comma list known?
 *
 * @param list "POSIX.1-2008,C23".
 * @return 1 if valid, 0 otherwise.
 */
static int	validList(const char *list)
{
	char	buf[256];
	char	*save;

	snprintf(buf, sizeof(buf), "%s", list);
	for (char *v = strtok_r(buf, ",", &save); v; v = strtok_r(NULL, ",", &save))
		if (!stdValid(v))
			return (0);
	return (1);
}

/**
 * @brief Recognize a kind tag (macro, type, either, var).
 *
 * @param s Tag.
 * @param k Set to the kind when recognized.
 * @return 1 if @p s is a kind, 0 otherwise.
 */
static int	parseKind(const char *s, t_kind *k)
{
	static const struct { const char *name; t_kind kind; }	kinds[] = {
		{"macro", KIND_MACRO}, {"type", KIND_TYPE}, {"either", KIND_EITHER},
		{"var", KIND_VAR},
	};

	for (size_t i = 0; i < sizeof(kinds) / sizeof(kinds[0]); i++)
		if (!strcmp(s, kinds[i].name))
			return (*k = kinds[i].kind, 1);
	return (0);
}

/**
 * @brief Parse the tags of one line (tok[2 .. n-1]) into @p e.
 *
 * Kinds, base=, ob=, removed=, ext=, and one option group (an upper-case
 * word, which must be in gOpts). Version values are checked.
 *
 * @param sp   Database, for the errors.
 * @param e    Entry being built.
 * @param tok  Tokens of the line (modified: '=' replaced by NUL).
 * @param n    Number of tokens.
 * @param path Spec file, for the errors.
 * @param ln   Line number, for the errors.
 */
static void	parseTags(t_spec *sp, t_entry *e, char **tok, int n, const char *path,
		int ln)
{
	for (int i = 2; i < n; i++)
	{
		char	*eq = strchr(tok[i], '=');
		char	*val = eq ? eq + 1 : NULL;

		if (eq)
			*eq = '\0';
		if (!val && parseKind(tok[i], &e->kind))
			continue ;
		if (val && !strcmp(tok[i], "base"))
			e->base = xstrdup(val);
		else if (val && !strcmp(tok[i], "ob"))
			e->ob = xstrdup(val);
		else if (val && !strcmp(tok[i], "removed"))
			e->removed = xstrdup(val);
		else if (val && !strcmp(tok[i], "ext"))
			e->ext = xstrdup(val);
		else if (!val && isupper((unsigned char)tok[i][0]))
		{
			if (optFind(tok[i]) == NULL)
				parseErr(sp, path, ln, "unknown option group (make options)", tok[i]);
			if (e->opt)
				parseErr(sp, path, ln, "more than one option group", tok[i]);
			e->opt = xstrdup(tok[i]);
		}
		else
			parseErr(sp, path, ln, "unknown tag", tok[i]);
		if (val && strcmp(tok[i], "ext") && !validList(val))
			parseErr(sp, path, ln, "unknown version", val);
	}
}

/**
 * @brief Is @p since an ISO C edition (rather than POSIX or EXT)?
 *
 * @param since Version.
 * @return 1 for C99 ... C23, 0 otherwise.
 */
static int	isCLadder(const char *since)
{
	const t_std	*s = stdFind(since);

	return (s && s->posix == 0);
}

/**
 * @brief Add a line to the database; a second line for the same name is
 *        the other ladder of the interface.
 *
 * At most one ISO C line and one POSIX line per name, with the same kind;
 * anything else is a duplicate. The second line is kept in alt of the
 * first one, so the database has one entry per interface.
 *
 * @param sp   Database.
 * @param e    Parsed line (copied).
 * @param path Spec file, for the errors.
 * @param ln   Line number, for the errors.
 */
static void	addLine(t_spec *sp, const t_entry *e, const char *path, int ln)
{
	for (size_t k = 0; k < sp->n; k++)
	{
		t_entry	*o = &sp->v[k];

		if (strcmp(o->cat, e->cat) || strcmp(o->name, e->name))
			continue ;
		if (o->alt || !strcmp(o->since, "EXT") || !strcmp(e->since, "EXT")
			|| isCLadder(o->since) == isCLadder(e->since))
			parseErr(sp, path, ln,
				"duplicate entry (at most one ISO C line and one POSIX line)", e->name);
		else if (o->kind != e->kind)
			parseErr(sp, path, ln, "the two lines disagree on the kind", e->name);
		else
		{
			o->alt = xmalloc(sizeof(*o->alt));
			*o->alt = *e;
		}
		return ;
	}
	sp->v = xrealloc(sp->v, (sp->n + 1) * sizeof(*sp->v));
	sp->v[sp->n++] = *e;
}

/**
 * @brief Load one spec file: a `header` line, then one line per interface.
 *
 * "#" starts a comment. The category is the file name without .spec.
 *
 * @param sp    Database.
 * @param dir   Spec directory.
 * @param fname File name (<cat>.spec).
 */
static void	loadFile(t_spec *sp, const char *dir, const char *fname)
{
	char	path[4096];
	char	line[1024];
	char	*cat;
	char	*header = NULL;
	FILE	*fp;
	int		ln = 0;

	snprintf(path, sizeof(path), "%s/%s", dir, fname);
	fp = fopen(path, "r");
	if (fp == NULL)
		return ;
	cat = xstrndup(fname, strlen(fname) - 5);
	while (fgets(line, sizeof(line), fp))
	{
		char	*tok[16];
		int		n = 0;
		char	*save;
		t_entry	e;

		ln++;
		line[strcspn(line, "#\r\n")] = '\0';
		for (char *s = strtok_r(line, " \t", &save); s && n < 16;
			s = strtok_r(NULL, " \t", &save))
			tok[n++] = s;
		if (n == 0)
			continue ;
		if (!strcmp(tok[0], "header"))
		{
			if (n != 2)
				parseErr(sp, path, ln, "usage: header <name.h>", NULL);
			else
				header = xstrdup(tok[1]);
			continue ;
		}
		if (n < 2)
		{
			parseErr(sp, path, ln, "missing version", tok[0]);
			continue ;
		}
		if (!stdValid(tok[1]))
		{
			parseErr(sp, path, ln, "unknown version (make standards)", tok[1]);
			continue ;
		}
		memset(&e, 0, sizeof(e));
		e.cat = cat;
		e.header = header;
		e.name = xstrdup(tok[0]);
		e.since = xstrdup(tok[1]);
		e.file = xstrdup(path);
		e.line = ln;
		if (header == NULL)
			parseErr(sp, path, ln, "entry before the `header` line", tok[0]);
		parseTags(sp, &e, tok, n, path, ln);
		if (!strcmp(e.since, "EXT") && e.ext == NULL)
			parseErr(sp, path, ln, "EXT entry without ext=NAME", e.name);
		addLine(sp, &e, path, ln);
	}
	fclose(fp);
}

/**
 * @brief qsort() order of entries: by category, then by name.
 *
 * @param a First entry.
 * @param b Second entry.
 * @return Negative, zero or positive.
 */
static int	cmpEntry(const void *a, const void *b)
{
	const t_entry	*x = a;
	const t_entry	*y = b;
	int				c = strcmp(x->cat, y->cat);

	return (c ? c : strcmp(x->name, y->name));
}

int	specLoad(t_spec *sp, const char *dir)
{
	DIR				*d;
	struct dirent	*de;

	memset(sp, 0, sizeof(*sp));
	d = opendir(dir);
	if (d == NULL)
		return (-1);
	while ((de = readdir(d)) != NULL)
	{
		size_t	len = strlen(de->d_name);

		if (len > 5 && !strcmp(de->d_name + len - 5, ".spec"))
			loadFile(sp, dir, de->d_name);
	}
	closedir(d);
	qsort(sp->v, sp->n, sizeof(*sp->v), cmpEntry);
	return (0);
}

const t_entry	*specFind(const t_spec *sp, const char *cat, const char *name)
{
	for (size_t i = 0; i < sp->n; i++)
		if (!strcmp(sp->v[i].cat, cat) && !strcmp(sp->v[i].name, name))
			return (&sp->v[i]);
	return (NULL);
}

/* ----- Target ----- */

/**
 * @brief Append a formatted string to @p buf (truncated to @p size).
 *
 * @param buf  NUL-terminated buffer.
 * @param size Size of @p buf.
 * @param fmt  Format with one %s.
 * @param s    String.
 */
static void	appendStr(char *buf, size_t size, const char *fmt, const char *s)
{
	size_t	len = strlen(buf);

	snprintf(buf + len, size - len, fmt, s);
}

void	targetCflags(const t_target *t, char *buf, size_t size)
{
	size_t	len;

	snprintf(buf, size, "-std=%s", t->std->cstd);
	if (t->std->posix)
	{
		len = strlen(buf);
		snprintf(buf + len, size - len, " -D_POSIX_C_SOURCE=%ldL", t->std->posixMacro);
		if (targetHasOpt(t, "XSI"))
		{
			len = strlen(buf);
			snprintf(buf + len, size - len, " -D_XOPEN_SOURCE=%d", t->std->xopen);
		}
	}
	for (char **o = t->opts; o && *o; o++)
	{
		const t_optdef	*d = optFind(*o);

		if (d && d->want)
			appendStr(buf, size, " -D%s", d->want);
	}
}

void	targetDescribe(const t_target *t, char *buf, size_t size)
{
	snprintf(buf, size, "%s", t->std->name);
	for (char **o = t->opts; o && *o; o++)
		appendStr(buf, size, " + %s", *o);
	if (t->ext)
		appendStr(buf, size, "%s", " + extensions");
}
