/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file main.c
 * @brief lcrun: options, target selection, discovery of units and tests.
 * @Created: 2026/11/23 04:42:42 by Moutig
 * @Updated: 2026/11/23 04:42:42 by Moutig
 *
 * What runs is decided by the spec, never by another libc:
 *   - spec/<cat>.spec says which interfaces the target standard
 *     requires (see spec.c);
 *   - src/suites/<cat>/<func>.c is a unit; it is in the run only if its
 *     function is required by the target (or is a selected extension);
 *   - a test whose `since` / `until` / `opt` excludes the target is
 *     not run;
 *   - the bin directory says which units were built. A selected unit
 *     without a binary did not compile or link against the libc: it is
 *     "unavailable", with the first lines of <bin>.err;
 *   - the probe directory says, for every required interface, whether
 *     the header declares it and the library provides it.
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

/* ----- Utilities (documented in lcrun.h) ----- */

void	*xmalloc(size_t n)
{
	void	*p = malloc(n ? n : 1);

	if (p == NULL)
	{
		perror("lcrun");
		exit(2);
	}
	return (p);
}

void	*xrealloc(void *p, size_t n)
{
	p = realloc(p, n ? n : 1);
	if (p == NULL)
	{
		perror("lcrun");
		exit(2);
	}
	return (p);
}

char	*xstrndup(const char *s, size_t n)
{
	char	*d = xmalloc(n + 1);

	memcpy(d, s, n);
	d[n] = '\0';
	return (d);
}

char	*xstrdup(const char *s)
{
	return (xstrndup(s, strlen(s)));
}

void	strAppend(char **dst, const char *s, size_t n)
{
	size_t	len = *dst ? strlen(*dst) : 0;

	*dst = xrealloc(*dst, len + n + 1);
	memcpy(*dst + len, s, n);
	(*dst)[len + n] = '\0';
}

double	nowMs(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((double)ts.tv_sec * 1e3 + (double)ts.tv_nsec / 1e6);
}

const char	*statusName(t_status st)
{
	static const char	*names[ST_COUNT] = {
		"pass", "fail", "skip", "crash", "timeout", "error", "unavailable"
	};

	return (names[st]);
}

char	**splitList(const char *s)
{
	char	**v = NULL;
	size_t	n = 0;
	char	*copy = xstrdup(s);
	char	*save;

	for (char *tok = strtok_r(copy, ",", &save); tok; tok = strtok_r(NULL, ",", &save))
	{
		v = xrealloc(v, (n + 2) * sizeof(*v));
		v[n++] = xstrdup(tok);
	}
	if (v)
		v[n] = NULL;
	free(copy);
	return (v);
}

int	inList(char **list, const char *s)
{
	if (list == NULL)
		return (1);
	for (; *list; list++)
		if (strcmp(*list, s) == 0)
			return (1);
	return (0);
}

/* ----- options ----- */

/** What this invocation does (one per command of usage()). */
typedef enum e_mode
{
	MODE_RUN,		/* lcrun BINDIR: run the tests. */
	MODE_MAKE,		/* --spec-make */
	MODE_PROBES,	/* --spec-probes DIR */
	MODE_LIST,		/* --spec-list */
	MODE_CHECK,		/* --spec-check */
	MODE_MATRIX,	/* --spec-matrix */
	MODE_TODO		/* --spec-todo */
}	t_mode;

/**
 * @brief Print the command line help on stderr.
 */
static void	usage(void)
{
	fputs(
"usage: lcrun [options] BINDIR          run the tests\n"
"       lcrun [options] --spec-list      interfaces required by the target\n"
"       lcrun [options] --spec-check     lint spec/ and src/suites/\n"
"       lcrun [options] --spec-make      make variables for the target\n"
"       lcrun [options] --spec-probes DIR  write one probe per interface\n"
"       lcrun [options] --spec-matrix    tested / required in the 8 standards\n"
"       lcrun [options] --spec-todo      required by the target, not tested\n"
"       lcrun --options                  list the option groups\n"
"       lcrun --compare A.tsv B.tsv      compare two runs\n"
"       lcrun --standards                list the known standards\n"
"\n"
"target:\n"
"  -S STD      standard (see --standards)                 [posix-2024]\n"
"  -O OPT,...  option groups (see --options)              [none]\n"
"  -E          include extensions (spec entries since EXT)\n"
"paths:\n"
"  -s DIR      suites                                     [src/suites]\n"
"  -p DIR      spec database                              [spec]\n"
"  -P DIR      probe results                              [none]\n"
"selection and run:\n"
"  -c CAT,...  only these categories\n"
"  -f FN,...   only these functions\n"
"  -t GLOB     only tests matching \"cat/func/name\"\n"
"  -j N        parallel jobs                              [nproc]\n"
"  -T SEC      default timeout per test                   [10]\n"
"  -r N        run each test N times (flakiness)          [1]\n"
"report:\n"
"  -L NAME     libc label                                 [?]\n"
"  -v          verbose: every test, per-function table, full lists\n"
"  -q          quiet: tables only\n"
"  -o FILE     results as TSV (for --compare)\n"
"  -J FILE     results as JSON\n"
"  -l          list the selected tests, do not run them\n"
"  --no-color\n", stderr);
}

/**
 * @brief Parse the command line into @p o and @p mode.
 *
 * Defaults: target POSIX.1-2024 without option, suites in src/suites,
 * spec in spec/, one job per CPU, 10 s timeout, colors on a tty.
 *
 * @param o        Options to fill.
 * @param argc     Argument count.
 * @param argv     Arguments.
 * @param mode     Set to the command.
 * @param probeOut Set to the directory of --spec-probes.
 * @return 0, or -1 on a usage error (usage printed).
 */
static int	parseOpts(t_opts *o, int argc, char **argv, t_mode *mode,
		const char **probeOut)
{
	memset(o, 0, sizeof(*o));
	o->suitesDir = "src/suites";
	o->specDir = "spec";
	o->libc = "?";
	o->target.std = stdFind("posix-2024");
	o->jobs = (int)sysconf(_SC_NPROCESSORS_ONLN);
	o->timeout = 10;
	o->repeat = 1;
	o->color = isatty(1);
	*mode = MODE_RUN;
	for (int i = 1; i < argc; i++)
	{
		const char	*a = argv[i];
		const char	*v = (i + 1 < argc) ? argv[i + 1] : NULL;

		if (!strcmp(a, "-v"))
			o->verbose = 1;
		else if (!strcmp(a, "-q"))
			o->quiet = 1;
		else if (!strcmp(a, "-l"))
			o->listOnly = 1;
		else if (!strcmp(a, "-E"))
			o->target.ext = 1;
		else if (!strcmp(a, "--no-color"))
			o->color = 0;
		else if (!strcmp(a, "--spec-make"))
			*mode = MODE_MAKE;
		else if (!strcmp(a, "--spec-list"))
			*mode = MODE_LIST;
		else if (!strcmp(a, "--spec-check"))
			*mode = MODE_CHECK;
		else if (!strcmp(a, "--spec-matrix"))
			*mode = MODE_MATRIX;
		else if (!strcmp(a, "--spec-todo"))
			*mode = MODE_TODO;
		else if (!strcmp(a, "--spec-probes") && v)
		{
			*mode = MODE_PROBES;
			*probeOut = argv[++i];
		}
		else if (!strcmp(a, "-h") || !strcmp(a, "--help"))
			return (usage(), -1);
		else if (a[0] == '-' && a[1] && !a[2] && strchr("SOspPLcftjTroJ", a[1]))
		{
			if (v == NULL)
				return (usage(), -1);
			i++;
			switch (a[1])
			{
				case 'S':
					o->target.std = stdFind(v);
					if (o->target.std == NULL)
					{
						fprintf(stderr, "lcrun: unknown standard '%s', one of:\n", v);
						stdPrintAll(stderr);
						return (-1);
					}
					break ;
				case 'O': o->target.opts = splitList(v); break ;
				case 's': o->suitesDir = v; break ;
				case 'p': o->specDir = v; break ;
				case 'P': o->probeDir = v; break ;
				case 'L': o->libc = v; break ;
				case 'c': o->cats = splitList(v); break ;
				case 'f': o->funcs = splitList(v); break ;
				case 't': o->glob = v; break ;
				case 'j': o->jobs = atoi(v); break ;
				case 'T': o->timeout = atoi(v); break ;
				case 'r': o->repeat = atoi(v); break ;
				case 'o': o->tsvOut = v; break ;
				case 'J': o->jsonOut = v; break ;
			}
		}
		else if (a[0] != '-' && o->binDir == NULL)
			o->binDir = a;
		else
			return (usage(), -1);
	}
	if (*mode == MODE_RUN && o->binDir == NULL)
		return (usage(), -1);
	if (o->jobs < 1)
		o->jobs = 1;
	if (o->repeat < 1)
		o->repeat = 1;
	return (0);
}

/* ----- discovery ----- */

/**
 * @brief The first lines of a file (build log of an unavailable unit).
 *
 * @param path     File.
 * @param maxLines Lines to keep.
 * @return New string, or NULL if the file cannot be read.
 */
static char	*readHead(const char *path, int maxLines)
{
	FILE	*fp = fopen(path, "r");
	char	line[512];
	char	*s = NULL;
	int		n = 0;

	if (fp == NULL)
		return (NULL);
	while (n < maxLines && fgets(line, sizeof(line), fp))
	{
		strAppend(&s, line, strlen(line));
		n++;
	}
	fclose(fp);
	return (s);
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

/**
 * @brief Every src/suites/<cat>/<func>.c whose function the target selects.
 *
 * A unit without spec entry is reported and ignored; a unit whose
 * function the target does not require is counted in
 * nUnitsOutOfTarget. Units are sorted.
 *
 * @param r       Run, units appended to r->units.
 * @param filters Apply -c / -f (off for the make fragment, which must
 *                know every unit of the target).
 */
static void	discoverUnits(t_run *r, int filters)
{
	DIR				*top;
	struct dirent	*c;
	char			path[4096];

	top = opendir(r->o.suitesDir);
	if (top == NULL)
	{
		fprintf(stderr, "lcrun: %s: %s\n", r->o.suitesDir, strerror(errno));
		exit(2);
	}
	while ((c = readdir(top)) != NULL)
	{
		DIR				*sub;
		struct dirent	*f;

		if (c->d_name[0] == '.' || (filters && !inList(r->o.cats, c->d_name)))
			continue ;
		snprintf(path, sizeof(path), "%s/%s", r->o.suitesDir, c->d_name);
		sub = opendir(path);
		if (sub == NULL)
			continue ;
		while ((f = readdir(sub)) != NULL)
		{
			size_t			len = strlen(f->d_name);
			char			*func;
			const t_entry	*e;
			int				ob;

			if (len < 3 || strcmp(f->d_name + len - 2, ".c") != 0)
				continue ;
			func = xstrndup(f->d_name, len - 2);
			if (filters && !inList(r->o.funcs, func))
			{
				free(func);
				continue ;
			}
			e = specFind(&r->spec, c->d_name, func);
			if (e == NULL || !reqSelected(specRequired(e, &r->o.target, &ob)))
			{
				if (e == NULL)
					fprintf(stderr, "lcrun: %s/%s.c: no entry in %s/%s.spec, ignored\n",
						path, func, r->o.specDir, c->d_name);
				r->nUnitsOutOfTarget++;
				free(func);
				continue ;
			}
			r->units = xrealloc(r->units, (r->nUnits + 1) * sizeof(t_unit));
			r->units[r->nUnits++] = (t_unit){.cat = xstrdup(c->d_name),
				.func = func, .entry = e};
		}
		closedir(sub);
	}
	closedir(top);
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
 * @brief Read <probedir>/<cat>/<name>.status for every spec entry.
 *
 * Left PR_UNKNOWN without -P or for an interface that was not probed.
 *
 * @param r Run, r->probes allocated (one per entry).
 */
static void	readProbes(t_run *r)
{
	char	path[4096];
	char	word[32];

	r->probes = xmalloc(r->spec.n * sizeof(*r->probes));
	for (size_t i = 0; i < r->spec.n; i++)
	{
		FILE	*fp;

		r->probes[i] = PR_UNKNOWN;
		if (r->o.probeDir == NULL)
			continue ;
		snprintf(path, sizeof(path), "%s/%s/%s.status", r->o.probeDir,
			r->spec.v[i].cat, r->spec.v[i].name);
		fp = fopen(path, "r");
		if (fp == NULL)
			continue ;
		if (fscanf(fp, "%31s", word) == 1)
			r->probes[i] = !strcmp(word, "ok") ? PR_OK
				: !strcmp(word, "unlinked") ? PR_UNLINKED : PR_UNDECLARED;
		fclose(fp);
	}
}

/**
 * @brief Find the units, their binaries, their tests, and the probe results.
 *
 * A unit without binary did not build: its build log (<bin>.err) is kept
 * for the report.
 *
 * @param r Run.
 */
static void	discover(t_run *r)
{
	char		path[4096];
	struct stat	sb;

	discoverUnits(r, 1);
	for (size_t i = 0; i < r->nUnits; i++)
	{
		t_unit	*u = &r->units[i];

		snprintf(path, sizeof(path), "%s/%s/%s", r->o.binDir, u->cat, u->func);
		if (stat(path, &sb) == 0 && (sb.st_mode & S_IXUSR))
		{
			u->bin = xstrdup(path);
			listUnit(r, u);
		}
		else
		{
			snprintf(path, sizeof(path), "%s/%s/%s.err", r->o.binDir, u->cat, u->func);
			u->err = readHead(path, 6);
		}
	}
	qsort(r->tests, r->nTests, sizeof(t_test), cmpTest);
	readProbes(r);
}

/* ----- main ----- */

/**
 * @brief Entry point: parse, load the spec, then do the command.
 *
 * @param argc Argument count.
 * @param argv Arguments.
 * @return 0 when everything passed (or the command succeeded), 1 when a
 *         test did not pass or a .since / .until / .opt is unknown, 2 on
 *         a usage or system error.
 */
int	main(int argc, char **argv)
{
	t_run		r;
	t_mode		mode;
	const char	*probeOut = NULL;
	int			bad;

	if (argc == 4 && !strcmp(argv[1], "--compare"))
		return (compareTsv(argv[2], argv[3], isatty(1)));
	if (argc == 2 && !strcmp(argv[1], "--standards"))
		return (stdPrintAll(stdout), 0);
	if (argc == 2 && !strcmp(argv[1], "--options"))
		return (toolOptions());
	memset(&r, 0, sizeof(r));
	if (parseOpts(&r.o, argc, argv, &mode, &probeOut) < 0)
		return (2);
	if (specLoad(&r.spec, r.o.specDir) < 0)
	{
		fprintf(stderr, "lcrun: %s: %s\n", r.o.specDir, strerror(errno));
		return (2);
	}
	if (targetCheck(&r.o.target) < 0)
		return (2);
	if (mode == MODE_CHECK)
		return (toolCheck(&r));
	if (mode == MODE_MATRIX)
		return (toolMatrix(&r));
	if (mode == MODE_TODO)
		return (toolTodo(&r));
	if (mode == MODE_LIST)
		return (toolList(&r));
	if (mode == MODE_PROBES)
		return (toolProbes(&r, probeOut));
	if (mode == MODE_MAKE)
	{
		discoverUnits(&r, 0);
		return (toolMake(&r));
	}
	discover(&r);
	if (r.o.listOnly)
	{
		for (size_t i = 0; i < r.nTests; i++)
			printf("%s/%s/%s\t%s:%d%s%s\n", r.tests[i].cat, r.tests[i].func,
				r.tests[i].name, r.tests[i].file, r.tests[i].line,
				r.tests[i].ref ? "\t" : "", r.tests[i].ref ? r.tests[i].ref : "");
		return (0);
	}
	r.wallMs = nowMs();
	runAll(&r);
	r.wallMs = nowMs() - r.wallMs;
	reportConsole(&r);
	if (r.o.tsvOut && reportTsv(&r, r.o.tsvOut) < 0)
		perror(r.o.tsvOut);
	if (r.o.jsonOut && reportJson(&r, r.o.jsonOut) < 0)
		perror(r.o.jsonOut);
	if (r.nBadVersion)
		fprintf(stderr, "lcrun: %zu test%s not run: unknown .since / .until / .opt "
			"(see above)\n", r.nBadVersion, r.nBadVersion == 1 ? "" : "s");
	bad = r.nBadVersion > 0;
	for (size_t i = 0; i < r.nTests; i++)
		if (r.tests[i].st != ST_PASS && r.tests[i].st != ST_SKIP)
			bad = 1;
	return (bad);
}
