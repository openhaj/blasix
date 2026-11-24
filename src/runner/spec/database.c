/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file database.c
 * @brief The spec database: spec/<category>.spec files.
 * @Created: 2026/11/24 08:15:30 by Moutig
 * @Updated: 2026/11/24 08:21:41 by Moutig
 *
 * Loads every spec file into a t_spec: the header, the `reserved` lines,
 * then one line per interface and per ladder (an ISO C line and a POSIX
 * line of one name become one entry with its alt). The format is
 * described in CONTRIBUTING.md and templates/spec.spec.
 */

#include "runner/lcrun.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

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
		else if (val && !strcmp(tok[i], "same"))
			e->same = xstrdup(val);
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
		if (val && strcmp(tok[i], "ext") && strcmp(tok[i], "same") && !validList(val))
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
 * @brief Add the patterns of a `reserved` line to category @p cat.
 *
 * @param sp  Database.
 * @param cat Category.
 * @param tok tok[0]: edition, tok[1 .. n-1]: patterns.
 * @param n   Number of tokens.
 */
static void	addReserved(t_spec *sp, const char *cat, char **tok, int n)
{
	t_hdr	*d = NULL;

	for (size_t h = 0; h < sp->nHdrs && !d; h++)
		if (!strcmp(sp->hdrs[h].cat, cat))
			d = &sp->hdrs[h];
	if (d == NULL)
	{
		sp->hdrs = xrealloc(sp->hdrs, (sp->nHdrs + 1) * sizeof(*sp->hdrs));
		d = &sp->hdrs[sp->nHdrs++];
		memset(d, 0, sizeof(*d));
		d->cat = xstrdup(cat);
	}
	for (int i = 1; i < n; i++)
	{
		if (strpbrk(tok[i], "<>"))
		{
			fprintf(stderr, "spec/%s.spec: reserved: placeholder left from the template: %s\n",
				cat, tok[i]);
			sp->errors++;
			continue ;
		}
		d->pat = xrealloc(d->pat, (d->n + 1) * sizeof(*d->pat));
		d->since = xrealloc(d->since, (d->n + 1) * sizeof(*d->since));
		d->pat[d->n] = xstrdup(tok[i]);
		d->since[d->n] = xstrdup(tok[0]);
		d->n++;
	}
}

/**
 * @brief Load one spec file: a `header` line, then one line per interface.
 *
 * "#" starts a comment. The category is the file name without .spec.
 * `reserved` lines may appear anywhere.
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
		if (!strcmp(tok[0], "reserved"))
		{
			if (n < 3 || !stdValid(tok[1]) || !strcmp(tok[1], "EXT"))
				parseErr(sp, path, ln, "usage: reserved <edition> <pattern>...", NULL);
			else
				addReserved(sp, cat, tok + 1, n - 1);
			continue ;
		}
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

const char	*kindName(t_kind k)
{
	static const char	*n[] = {
		[KIND_FUNC] = "function", [KIND_MACRO] = "macro", [KIND_TYPE] = "type",
		[KIND_EITHER] = "either", [KIND_VAR] = "var",
	};

	return (n[k]);
}
