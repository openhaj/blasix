/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file run.h
 * @brief A run: units, tests, results, and how they are found and run (run/).
 * @Created: 2026/11/24 08:28:05 by Moutig
 * @Updated: 2026/11/25 09:28:18 by Moutig
 *
 * run/discover.c  sources of the suites, units of the target, their
 *                 tests, probe results;
 * run/exec.c      the process pool (fork + exec, pipes, timeouts);
 * run/cache.c     the result cache (-C): tests whose binary did not
 *                 change are not run again.
 * Everything of one invocation lives in one t_run.
 */

#ifndef LCRUN_RUN_H
# define LCRUN_RUN_H

# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include "runner/spec.h"
# include <sys/types.h>

/** Final status of one test. */
typedef enum e_status
{
	ST_PASS,		/* Every check passed (and the expected outcome happened). */
	ST_FAIL,		/* A check failed, or the expected exit / signal did not. */
	ST_SKIP,		/* The test called SKIP (environment: no other locale, ...). */
	ST_UNIMPL,		/* The test called NOT_IMPLEMENTED: the libc lacks what it needs. */
	ST_CRASH,		/* Killed by an unexpected signal. */
	ST_TIMEOUT,		/* Killed after its timeout. */
	ST_ERROR,		/* The process ended before the end of the test. */
	ST_UNAVAIL,		/* The unit did not build against the libc. */
	ST_COUNT,		/* Number of statuses (array size). */
}	t_status;

/** Result of the probe of one interface (<probedir>/<cat>/<name>.status). */
typedef enum e_probe
{
	PR_UNKNOWN,		/* Not probed. */
	PR_OK,			/* Declared by the header and provided by the library. */
	PR_UNDECLARED,	/* Not declared by the header (or macro / type missing). */
	PR_UNLINKED,	/* Declared, but the library does not define it. */
	PR_BROKEN,		/* Declared, but the link failed for another reason than an
					   undefined symbol (compiler crash, broken archive, ...). */
}	t_probe;

/** Result of the hidden probe of one interface (<probedir>/<cat>/<name>.hidden). */
typedef enum e_hidden
{
	HID_UNKNOWN,	/* Not checked (lax mode, or visibility not VIS_HIDDEN). */
	HID_HIDDEN,		/* Not visible, as required. */
	HID_LEAKED,		/* Visible although the target forbids it. */
}	t_hidden;

/** One test binary: src/suites/<cat>/<func>.c -> <bindir>/<cat>/<func>. */
typedef struct s_unit
{
	char			*cat;	/* Category (directory). */
	char			*func;	/* Function (file name). */
	char			*rel;	/* Source path in the suites, without .c:
							   <cat>[/<group>...]/<func>; also the path of
							   the binary under BINDIR. */
	char			*bin;	/* NULL if the binary does not exist. */
	char			*err;	/* If unavailable: the error lines of the build log
							   (or its first lines when none says "error"). */
	char			*log;	/* If unavailable: the build log (first 64 KiB). */
	const t_entry	*entry;	/* Spec entry, NULL if unknown. */
	char			hash[17];	/* Hash of the binary (cache.c), "" until read. */
}	t_unit;

/** One test case and its result. */
typedef struct s_test
{
	t_unit		*unit;		/* Binary that holds it. */
	char		*cat;		/* Category. */
	char		*func;		/* Function (TEST_FN may differ from the unit's). */
	char		*name;		/* Case name. */
	char		*file;		/* Source file. */
	int			line;		/* Source line. */
	int			timeout;	/* Seconds (0 = default). */
	int			expect;		/* LC_NORMAL / LC_EXIT / LC_SIGNAL encoding. */
	char		*since;		/* Behaviour required from this version on. */
	char		*until;		/* Behaviour dropped in this version. */
	char		*ref;		/* Spec reference (section, clause). */

	t_status	st;			/* Status of the last run. */
	int			runs;		/* Times run (-r). */
	int			passes;		/* Runs that passed (flakiness). */
	double		ms;			/* Wall time of the last run. */
	double		cpuMs;		/* User + system time of the last run. */
	long		rssKb;		/* Peak RSS of the last run. */
	int			asserts;	/* Checks run (from @end). */
	int			failed;		/* Checks failed (from @end). */
	int			sig;		/* Signal that killed it, 0 otherwise. */
	int			code;		/* Exit status, -1 if killed. */
	char		*msgs;		/* @fail / @skip / @unimpl / @log lines, formatted. */
	char		*out;		/* Captured stdout + stderr (truncated). */
	int			cached;		/* Result taken from the cache, not run. */
}	t_test;

/** Command-line options (see usage() in main.c). */
typedef struct s_opts
{
	const char	*binDir;	/* Directory of the test binaries (run mode). */
	const char	*suitesDir;	/* -s: test sources. */
	const char	*specDir;	/* -p: spec database. */
	const char	*probeDir;	/* -P: probe results, or NULL. */
	const char	*libc;		/* Label for the report. */
	t_target	target;		/* -S, -O, -E. */
	char		**cats;		/* -c filter (NULL-terminated) */
	char		**funcs;	/* -f filter */
	const char	*glob;		/* -t filter on "cat/func/name" */
	int			jobs;		/* -j: parallel processes. */
	int			timeout;	/* -T: default timeout, seconds. */
	int			repeat;		/* -r: runs per test. */
	int			verbose;	/* -v */
	int			quiet;		/* -q */
	int			color;		/* ANSI colors (stdout is a tty, no --no-color). */
	int			listOnly;	/* -l */
	int			lax;		/* -x: lax mode, visibility not checked. */
	const char	*tsvOut;	/* -o: TSV results, or NULL. */
	const char	*jsonOut;	/* -J: JSON results, or NULL. */
	const char	*cacheFile;	/* -C: result cache, or NULL (no cache). */
	char		**libcFiles;	/* -K: files the binaries load at run time. */
}	t_opts;

/** Everything about one invocation of lcrun. */
typedef struct s_run
{
	t_opts	o;					/* Options. */
	t_spec	spec;				/* Spec database. */
	t_probe	*probes;			/* One per spec entry. */
	t_hidden	*hidden;		/* One per spec entry (hidden probes). */
	t_reject	*rejects;		/* Reject probes of the target. */
	size_t		nRejects;		/* Number of reject probes. */
	t_unit	*units;				/* Selected units, sorted. */
	size_t	nUnits;				/* Number of units. */
	t_test	*tests;				/* Selected tests, in source order. */
	size_t	nTests;				/* Number of tests. */
	size_t	nOutOfTarget;		/* Tests excluded by their since / until / opt. */
	size_t	nBadVersion;		/* Tests whose since / until / opt is unknown. */
	size_t	nUnitsOutOfTarget;	/* Units whose function the target lacks. */
	size_t	nCached;			/* Tests whose result came from the cache. */
	double	wallMs;				/* Wall time of the whole run. */
}	t_run;

/**
 * @brief Name of a status for TSV / JSON ("pass", "unavailable", ...).
 *
 * @param st Status.
 * @return Static string.
 */
const char	*statusName(t_status st);
/**
 * @brief Is @p st a failure: fail, crash, timeout or error?
 *
 * Skipped and not implemented tests are not failures (they did not run).
 *
 * @param st Status.
 * @return 1 or 0.
 */
int			statusFailed(t_status st);
/**
 * @brief Did a test with status @p st run: pass or a failure?
 *
 * @param st Status.
 * @return 1 or 0.
 */
int			statusRan(t_status st);
/**
 * @brief Split a comma list ("a,b,c") into a NULL-terminated array.
 *
 * @param s List.
 * @return New array of new strings, or NULL for an empty list.
 */
char		**splitList(const char *s);

/** A test source found in the suites (suitesScan()). */
typedef struct s_src
{
	char	*cat;	/* Category: first directory under the suites. */
	char	*func;	/* File name without .c (the interface). */
	char	*rel;	/* <cat>[/<group>...]/<func>, without .c. */
}	t_src;
/**
 * @brief Every <suites>/<cat>/.../<func>.c, at any depth under its
 *        category (groups are only folders: the unit is still <cat>/<func>).
 *
 * The directory `bits` (shared test headers, no unit) and hidden entries
 * are skipped. The list is sorted by rel.
 *
 * @param dir Suites directory.
 * @param out Set to a new array (suitesFree() it).
 * @return Number of sources, or -1 if @p dir cannot be opened.
 */
long		suitesScan(const char *dir, t_src **out);
/**
 * @brief Is @p s the definitions unit of its category, <cat>/defs.c
 *        (types and constants: cases of src/suites/bits/, several
 *        interfaces in one binary, each test attached to its symbol)?
 *
 * @param s Source.
 * @return 1 or 0.
 */
int			isDefsUnit(const t_src *s);
/**
 * @brief Free what suitesScan() returned.
 *
 * @param v Array.
 * @param n Number of sources.
 */
void		suitesFree(t_src *v, long n);

/**
 * @brief Run every selected test (r->o.repeat times), r->o.jobs at a time.
 *
 * Fills the result fields of each t_test, except those taken from the
 * cache (t->cached). Exits with status 2 if a
 * process cannot be created.
 *
 * @param r Run.
 */
void		runAll(t_run *r);

/**
 * @brief Take from the cache (-C) the result of every test whose binary,
 *        timeout and run-time libc did not change since it was stored
 *        (run/cache.c); runAll() does not run them (t->cached).
 *
 * Nothing without -C or with -r N > 1.
 *
 * @param r Run, after discover().
 */
void		cacheLoad(t_run *r);

/**
 * @brief Store the results of this run in the cache (-C), keeping the
 *        entries of the units that were not selected.
 *
 * Timeouts and errors are stored but never reused (cacheStatus() says
 * so). Nothing without -C or with -r N > 1.
 *
 * @param r Run, after runAll().
 */
void		cacheSave(t_run *r);

/**
 * @brief --cache-status: for every selected test, whether the next run
 *        takes its result from the cache, and why not (new test, binary
 *        or timeout changed, last run timed out, ...). Runs nothing.
 *
 * @param r Run, after discover().
 * @return 0.
 */
int			cacheStatus(t_run *r);

/**
 * @brief Add the results that come from the spec and the probes, once
 *        the tests ran (run/virtual.c): @not_in_target (SKIP, with why)
 *        for each interface of the target's headers the target does not
 *        require; @hidden (strict mode: PASS, or FAIL for a leak) for each
 *        one that must be hidden; @reject_<a>_<b> (PASS, or FAIL when the
 *        translation unit compiled) for each reject probe. They count in
 *        the report, the totals, the exit status and the TSV / JSON.
 *
 * @param r Run, after runAll() and readProbes().
 */
void		addVirtualResults(t_run *r);


/* ----- Shared between the files of the module ----- */

/**
 * @brief Every src/suites/<cat>/.../<func>.c whose function the target
 *        selects (groups are folders: the unit is <cat>/<func>), and the
 *        <cat>/defs.c of every header of the target.
 *
 * A unit without spec entry is reported and ignored; a unit whose
 * function the target does not require is counted in
 * nUnitsOutOfTarget. Units are sorted.
 *
 * @param r       Run, units appended to r->units.
 * @param filters Apply -c / -f (off for the make fragment, which must
 *                know every unit of the target).
 */
void	discoverUnits(t_run *r, int filters);

/**
 * @brief Find the units, their binaries, their tests, and the probe results.
 *
 * A unit without binary did not build: its build log (<bin>.err) is kept
 * for the report.
 *
 * @param r Run.
 */
void	discover(t_run *r);

#endif
