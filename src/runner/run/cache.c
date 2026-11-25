/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file cache.c
 * @brief Result cache: a test whose binary did not change is not run again.
 * @Created: 2026/11/25 09:13:28 by Moutig
 * @Updated: 2026/11/25 09:28:28 by Moutig
 *
 * The cache file (-C FILE, one per libc and target) holds the last result
 * of every test, with what it depends on:
 *
 *     #lcrun-cache 1 <lcrun> <libc>
 *     <unit>\t<func>\t<name>\t<binary>\t<timeout>\t<status>\t<ms>\t<cpu_ms>
 *         \t<rss_kb>\t<checks>\t<failed>\t<signal>\t<exit>\t<msgs>\t<out>
 *
 * <binary> is a hash of the content of the unit's binary: the test code,
 * the framework and, for a static libc, every libc object the unit links
 * (a relink that gives the same binary keeps the results). <libc> is the
 * identity of the files a dynamically linked binary loads at run time
 * (-K, from the profile's LIBC_RUNTIME), <lcrun> a hash of lcrun itself
 * (it decides the statuses); if either changed, the whole cache is
 * dropped. Messages and output are escaped (\t \n \\).
 *
 * Only reproducible results are reused: pass, fail, skip, unimplemented,
 * crash. A timeout or an error depends on the load of the machine: it is
 * stored (so that --cache-status can say why the test runs again) but
 * always run again. With -r N (flakiness) the cache is not used.
 *
 * --cache-status (cacheStatus()) says, without running anything, which
 * tests the next run takes from the cache and why the others run.
 */

#include "runner/lcrun.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/** Format version, first field of the header line. */
#define LC_CACHE_VERSION	1

/** Fields of an entry line. */
#define LC_CACHE_FIELDS		15

/** FNV-1a 64-bit offset basis and prime. */
#define LC_FNV_BASIS	0xcbf29ce484222325ULL
#define LC_FNV_PRIME	0x100000001b3ULL

/** One line of the cache file. */
typedef struct s_centry
{
	char	*f[LC_CACHE_FIELDS];	/* Fields, unescaped (f[0] owns the line). */
}	t_centry;

/**
 * @brief Add @p n bytes to an FNV-1a hash.
 *
 * @param h Hash so far.
 * @param p Bytes.
 * @param n Number of bytes.
 * @return New hash.
 */
static uint64_t	fnv(uint64_t h, const void *p, size_t n)
{
	const unsigned char	*b = p;

	for (size_t i = 0; i < n; i++)
		h = (h ^ b[i]) * LC_FNV_PRIME;
	return (h);
}

/**
 * @brief Hash of the content of a file.
 *
 * @param path File.
 * @param out  Receives the hash.
 * @return 0, or -1 if the file cannot be read.
 */
static int	hashFile(const char *path, uint64_t *out)
{
	char		buf[1 << 16];
	uint64_t	h = LC_FNV_BASIS;
	ssize_t		n;
	int			fd = open(path, O_RDONLY | O_CLOEXEC);

	if (fd < 0)
		return (-1);
	while ((n = read(fd, buf, sizeof(buf))) > 0)
		h = fnv(h, buf, (size_t)n);
	close(fd);
	if (n < 0)
		return (-1);
	*out = h;
	return (0);
}

/**
 * @brief Identity of the run-time libc: path, size, date and inode of
 *        each file of -K (a package update replaces them).
 *
 * @param files NULL-terminated list, or NULL (static libc).
 * @return Hash ("missing" counts for a file that does not exist).
 */
static uint64_t	libcId(char **files)
{
	uint64_t	h = LC_FNV_BASIS;
	struct stat	sb;

	for (size_t i = 0; files && files[i]; i++)
	{
		h = fnv(h, files[i], strlen(files[i]) + 1);
		if (stat(files[i], &sb) != 0)
		{
			h = fnv(h, "missing", 7);
			continue ;
		}
		h = fnv(h, &sb.st_size, sizeof(sb.st_size));
		h = fnv(h, &sb.st_mtim, sizeof(sb.st_mtim));
		h = fnv(h, &sb.st_ino, sizeof(sb.st_ino));
	}
	return (h);
}

/**
 * @brief The header line this run expects ("#lcrun-cache 1 <lcrun> <libc>").
 *
 * @param r   Run.
 * @param buf Destination.
 * @param n   Size of @p buf.
 */
static void	header(const t_run *r, char *buf, size_t n)
{
	uint64_t	self = 0;

	hashFile("/proc/self/exe", &self);
	snprintf(buf, n, "#lcrun-cache %d %016llx %016llx", LC_CACHE_VERSION,
		(unsigned long long)self, (unsigned long long)libcId(r->o.libcFiles));
}

/**
 * @brief Write @p s escaped (\t \n \\); nothing for NULL.
 *
 * @param fp Stream.
 * @param s  String, or NULL.
 */
static void	putEsc(FILE *fp, const char *s)
{
	for (; s && *s; s++)
	{
		if (*s == '\t')
			fputs("\\t", fp);
		else if (*s == '\n')
			fputs("\\n", fp);
		else if (*s == '\\')
			fputs("\\\\", fp);
		else
			fputc(*s, fp);
	}
}

/**
 * @brief Undo putEsc() in place.
 *
 * @param s Field.
 */
static void	unEsc(char *s)
{
	char	*d = s;

	for (; *s; s++)
	{
		if (*s == '\\' && s[1])
		{
			s++;
			*d++ = (*s == 't') ? '\t' : (*s == 'n') ? '\n' : *s;
		}
		else
			*d++ = *s;
	}
	*d = '\0';
}

/**
 * @brief Can a result with status @p st be reused?
 *
 * @param st Status.
 * @return 1 or 0.
 */
static int	cacheable(t_status st)
{
	return (st == ST_PASS || st == ST_FAIL || st == ST_SKIP
		|| st == ST_UNIMPL || st == ST_CRASH);
}

/**
 * @brief Status from its name (statusName()), or -1.
 *
 * @param s Name.
 * @return Status, or -1 if unknown.
 */
static int	statusFromName(const char *s)
{
	for (int i = 0; i < ST_COUNT; i++)
		if (!strcmp(statusName((t_status)i), s))
			return (i);
	return (-1);
}

/** Entries read by cacheLoad(), written back by cacheSave(). */
static t_centry	*gEntries;
static size_t	gN;

/** What readFile() found. */
typedef enum e_cfile
{
	CF_OK,			/* Entries loaded. */
	CF_MISSING,		/* No cache file yet. */
	CF_FORMAT,		/* Another format version, or not a cache file. */
	CF_LCRUN,		/* Written by another lcrun. */
	CF_LIBC,		/* Written with another run-time libc (-K). */
}	t_cfile;

/**
 * @brief Read the cache file; nothing is loaded unless it was written by
 *        this lcrun for this run-time libc.
 *
 * @param r Run.
 * @return What was found.
 */
static t_cfile	readFile(const t_run *r)
{
	char	want[128];
	char	*line = NULL;
	size_t	cap = 0;
	ssize_t	len;
	FILE	*fp = fopen(r->o.cacheFile, "r");
	t_cfile	st = CF_OK;

	if (fp == NULL)
		return (CF_MISSING);
	header(r, want, sizeof(want));
	len = getline(&line, &cap, fp);
	if (len > 0 && line[len - 1] == '\n')
		line[--len] = '\0';
	/* "#lcrun-cache V " (15 bytes), lcrun hash (16), ' ', libc hash (16) */
	if (len != (ssize_t)strlen(want) || strncmp(line, want, 15) != 0)
		st = CF_FORMAT;
	else if (strncmp(line + 15, want + 15, 16) != 0)
		st = CF_LCRUN;
	else if (strcmp(line + 32, want + 32) != 0)
		st = CF_LIBC;
	if (st != CF_OK)
	{
		free(line);
		fclose(fp);
		return (st);
	}
	while ((len = getline(&line, &cap, fp)) > 0)
	{
		t_centry	e = {0};
		char		*s;
		int			n = 0;

		if (line[len - 1] == '\n')
			line[len - 1] = '\0';
		s = xstrdup(line);
		while (n < LC_CACHE_FIELDS && s)
		{
			e.f[n++] = s;
			s = strchr(s, '\t');
			if (s)
				*s++ = '\0';
		}
		if (n != LC_CACHE_FIELDS)
		{
			free(e.f[0]);
			continue ;
		}
		for (int i = 0; i < n; i++)
			unEsc(e.f[i]);
		gEntries = xrealloc(gEntries, (gN + 1) * sizeof(*gEntries));
		gEntries[gN++] = e;
	}
	free(line);
	fclose(fp);
	return (CF_OK);
}

/**
 * @brief Hash of a unit's binary, as 16 hex digits (computed once).
 *
 * @param u   Unit (built).
 * @param buf At least 17 bytes.
 * @return @p buf, or NULL if the binary cannot be read.
 */
static const char	*unitHash(t_unit *u, char *buf)
{
	uint64_t	h;

	if (!u->hash[0])
	{
		if (hashFile(u->bin, &h) < 0)
			return (NULL);
		snprintf(u->hash, sizeof(u->hash), "%016llx", (unsigned long long)h);
	}
	return (strcpy(buf, u->hash));
}

/**
 * @brief The entry of test @p t, or NULL.
 *
 * @param t Test.
 * @return Entry (any key), or NULL.
 */
static t_centry	*findEntry(const t_test *t)
{
	for (size_t i = 0; i < gN; i++)
	{
		t_centry	*e = &gEntries[i];

		if (!strcmp(e->f[0], t->unit->rel) && !strcmp(e->f[1], t->func)
			&& !strcmp(e->f[2], t->name))
			return (e);
	}
	return (NULL);
}

/**
 * @brief Why test @p t must run, or NULL when its stored result is reused.
 *
 * @param r Run, entries loaded.
 * @param t Test.
 * @param e Set to its entry (when the result is reused).
 * @return Static reason, or NULL.
 */
static const char	*whyRerun(const t_run *r, t_test *t, t_centry **e)
{
	char	hash[17];
	int		st;

	*e = findEntry(t);
	if (*e == NULL)
		return ("new test (no stored result)");
	if (unitHash(t->unit, hash) == NULL)
		return ("binary unreadable");
	if (strcmp((*e)->f[3], hash) != 0)
		return ("binary changed");
	if (atoi((*e)->f[4]) != (t->timeout ? t->timeout : r->o.timeout))
		return ("timeout changed");
	st = statusFromName((*e)->f[5]);
	if (st == ST_TIMEOUT)
		return ("last run timed out");
	if (st == ST_ERROR)
		return ("last run ended in error");
	if (st < 0 || !cacheable((t_status)st))
		return ("stored status not reusable");
	return (NULL);
}

void	cacheLoad(t_run *r)
{
	if (r->o.cacheFile == NULL || r->o.repeat > 1)
		return ;
	readFile(r);
	for (size_t i = 0; i < r->nTests; i++)
	{
		t_test		*t = &r->tests[i];
		t_centry	*e;

		if (whyRerun(r, t, &e) != NULL)
			continue ;
		t->st = (t_status)statusFromName(e->f[5]);
		t->ms = atof(e->f[6]);
		t->cpuMs = atof(e->f[7]);
		t->rssKb = atol(e->f[8]);
		t->asserts = atoi(e->f[9]);
		t->failed = atoi(e->f[10]);
		t->sig = atoi(e->f[11]);
		t->code = atoi(e->f[12]);
		t->msgs = e->f[13][0] ? xstrdup(e->f[13]) : NULL;
		t->out = e->f[14][0] ? xstrdup(e->f[14]) : NULL;
		t->runs = 1;
		t->passes = (t->st == ST_PASS);
		t->cached = 1;
		r->nCached++;
	}
}

/**
 * @brief Is @p e the entry of a test of a selected unit (then it is
 *        replaced by this run, or dropped: that test no longer exists)?
 *
 * @param r Run.
 * @param e Entry.
 * @return 1 or 0.
 */
static int	ofSelectedUnit(const t_run *r, const t_centry *e)
{
	for (size_t i = 0; i < r->nUnits; i++)
		if (r->units[i].bin && !strcmp(r->units[i].rel, e->f[0]))
			return (1);
	return (0);
}

/**
 * @brief Write one test's result as an entry line.
 *
 * @param fp   Stream.
 * @param r    Run.
 * @param t    Test (run or cached).
 * @param hash Hash of its binary.
 */
static void	putTest(FILE *fp, const t_run *r, const t_test *t, const char *hash)
{
	fprintf(fp, "%s\t%s\t%s\t%s\t%d\t%s\t%.3f\t%.3f\t%ld\t%d\t%d\t%d\t%d\t",
		t->unit->rel, t->func, t->name, hash,
		t->timeout ? t->timeout : r->o.timeout, statusName(t->st), t->ms,
		t->cpuMs, t->rssKb, t->asserts, t->failed, t->sig, t->code);
	putEsc(fp, t->msgs);
	fputc('\t', fp);
	putEsc(fp, t->out);
	fputc('\n', fp);
}

void	cacheSave(t_run *r)
{
	char	path[4096];
	char	head[128];
	char	hash[17];
	FILE	*fp;

	if (r->o.cacheFile == NULL || r->o.repeat > 1)
		return ;
	snprintf(path, sizeof(path), "%s.tmp", r->o.cacheFile);
	fp = fopen(path, "w");
	if (fp == NULL)
	{
		perror(path);
		return ;
	}
	header(r, head, sizeof(head));
	fprintf(fp, "%s\n", head);
	/* entries of the units not selected this time stay as they were */
	for (size_t i = 0; i < gN; i++)
	{
		t_centry	*e = &gEntries[i];

		if (ofSelectedUnit(r, e))
			continue ;
		for (int k = 0; k < LC_CACHE_FIELDS; k++)
		{
			if (k)
				fputc('\t', fp);
			putEsc(fp, e->f[k]);
		}
		fputc('\n', fp);
	}
	/* the virtual results (no unit) are not cached: they cost nothing */
	for (size_t i = 0; i < r->nTests; i++)
	{
		t_test	*t = &r->tests[i];

		if (t->unit == NULL || t->unit->bin == NULL || t->st == ST_UNAVAIL
			|| unitHash(t->unit, hash) == NULL)
			continue ;
		putTest(fp, r, t, hash);
	}
	if (fclose(fp) != 0 || rename(path, r->o.cacheFile) != 0)
		perror(r->o.cacheFile);
	for (size_t i = 0; i < gN; i++)
		free(gEntries[i].f[0]);
	free(gEntries);
	gEntries = NULL;
	gN = 0;
}

/**
 * @brief Why nothing can be reused, for a cache file that was not loaded.
 *
 * @param st What readFile() found.
 * @return Static text.
 */
static const char	*fileReason(t_cfile st)
{
	if (st == CF_MISSING)
		return ("no cache yet");
	if (st == CF_LCRUN)
		return ("lcrun changed since the cache was written");
	if (st == CF_LIBC)
		return ("the run-time libc changed (LIBC_RUNTIME)");
	return ("not a cache file of this lcrun version");
}

/** Reasons counted by cacheStatus() (whyRerun() strings, by address). */
#define LC_MAX_REASONS	16

int	cacheStatus(t_run *r)
{
	const char	*why[LC_MAX_REASONS];
	size_t		count[LC_MAX_REASONS] = {0};
	int			nWhy = 0;
	size_t		reused = 0;
	size_t		shown = 0;
	t_cfile		st;

	if (r->o.cacheFile == NULL)
		return (printf("cache: off (CACHE=0 or coverage): all %zu tests run\n",
			r->nTests), 0);
	if (r->o.repeat > 1)
		return (printf("cache: not used with -r %d (REPEAT): all %zu tests run\n",
			r->o.repeat, r->nTests), 0);
	st = readFile(r);
	printf("cache: %s\n", r->o.cacheFile);
	if (st != CF_OK)
		return (printf("all %zu tests run: %s\n", r->nTests, fileReason(st)), 0);
	for (size_t i = 0; i < r->nTests; i++)
	{
		t_test		*t = &r->tests[i];
		t_centry	*e;
		const char	*w = whyRerun(r, t, &e);
		int			k;

		if (w == NULL)
		{
			reused++;
			continue ;
		}
		for (k = 0; k < nWhy && why[k] != w; k++)
			;
		if (k == nWhy && nWhy < LC_MAX_REASONS)
			why[nWhy++] = w;
		if (k < LC_MAX_REASONS)
			count[k]++;
		if (r->o.verbose || shown < 30)
			printf("  run   %s/%s/%s  %s\n", t->cat, t->func, t->name, w);
		shown++;
	}
	if (shown > 30 && !r->o.verbose)
		printf("  ... %zu more (V=1 lists them all)\n", shown - 30);
	printf("%zu tests: %zu from the cache, %zu to run", r->nTests, reused, shown);
	for (int k = 0; k < nWhy; k++)
		printf("%s%zu %s", k ? ", " : " (", count[k], why[k]);
	printf("%s\n", nWhy ? ")" : "");
	for (size_t i = 0; i < gN; i++)
		free(gEntries[i].f[0]);
	free(gEntries);
	gEntries = NULL;
	gN = 0;
	return (0);
}
