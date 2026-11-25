/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file cli.c
 * @brief Command line of lcrun: usage and options.
 * @Created: 2026/11/24 08:25:17 by Moutig
 * @Updated: 2026/11/25 09:21:04 by Moutig
 *
 * Every option fills t_opts; the command (run, --spec-make, ...) is a
 * t_mode. --compare, --standards and --options are handled by main.c
 * before (they need no spec database).
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
"  -x          lax: do not check what the headers must hide\n"
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
"  -C FILE     result cache: reuse the result of a test whose binary,\n"
"              timeout and run-time libc did not change    [none]\n"
"  -K F,...    files the binaries load at run time (shared libc), part\n"
"              of the cache key                           [none]\n"
"report:\n"
"  -L NAME     libc label                                 [?]\n"
"  -v          verbose: every test, per-function table, full lists\n"
"  -q          quiet: tables only\n"
"  -o FILE     results as TSV (for --compare)\n"
"  -J FILE     results as JSON\n"
"  -l          list the selected tests, do not run them\n"
"  --no-color\n", stderr);
}

int	parseOpts(t_opts *o, int argc, char **argv, t_mode *mode,
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
		else if (!strcmp(a, "-x"))
			o->lax = 1;
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
		else if (a[0] == '-' && a[1] && !a[2] && strchr("SOspPLcftjTroJCK", a[1]))
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
				case 'C': o->cacheFile = v; break ;
				case 'K': o->libcFiles = splitList(v); break ;
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
