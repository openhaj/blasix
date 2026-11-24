/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file lcrun.h
 * @brief Shared types and functions of the lcrun test runner.
 * @Created: 2026/11/23 04:42:42 by Moutig
 * @Updated: 2026/11/24 03:17:14 by Moutig
 *
 * lcrun is built with the host libc. It reads the spec database,
 * selects what a target standard requires, discovers the test binaries,
 * runs every test in its own process (fork + exec), enforces timeouts,
 * collects the events written on fd 3, and reports conformance stats.
 *
 * Sources:
 *   - spec.c    standards, option groups, the spec database, targets;
 *   - main.c    command line, discovery of units and tests, utilities;
 *   - tools.c   --spec-make, --spec-probes, --spec-list, --spec-check,
 *               --spec-matrix, --spec-todo, --options;
 *   - exec.c    the process pool that runs the tests;
 *   - report.c  console report, TSV / JSON results, --compare.
 *
 * Data flow of a run: specLoad() fills a t_spec; main.c finds the units
 * (t_unit) the target selects and asks each binary for its tests
 * (t_test); runAll() runs them and fills their results; report.c prints
 * and exports them. Everything lives in one t_run.
 */

#ifndef LCRUN_H
# define LCRUN_H

# define _GNU_SOURCE
# include <stddef.h>
# include <stdio.h>
# include <sys/types.h>

/* ----- Standards and targets (spec.c) ----- */

/**
 * @brief One of the 8 standards a libc can be checked against.
 *
 * ISO C editions have posix == 0. A POSIX edition is built on an ISO C
 * edition, its "C base", given by @c c.
 */
typedef struct s_std
{
	const char	*id;		/* Command-line id: posix-2024. */
	const char	*name;		/* Spec name: POSIX.1-2024. */
	int			c;			/* ISO C level (C99=1 ... C23=4). */
	int			posix;		/* POSIX level (0 for a pure ISO C target). */
	long		posixMacro;	/* _POSIX_C_SOURCE value. */
	int			xopen;		/* _XOPEN_SOURCE value when XSI is on. */
	const char	*cstd;		/* -std= value. */
}	t_std;

/**
 * @brief What a libc is checked against: a standard, options, extensions.
 */
typedef struct s_target
{
	const t_std	*std;		/* The standard. */
	char		**opts;		/* Option groups turned on (XSI, THREADS, ...). */
	int			ext;		/* Include extensions (spec entries since EXT). */
}	t_target;

/** One option group (spec.c): a POSIX margin code or an ISO C feature. */
typedef struct s_optdef
{
	const char	*name;		/* XSI, TSF, ..., THREADS, LIB_EXT1. */
	const char	*from;		/* ISO C feature: conditional from this edition;
							   NULL for a POSIX option. */
	const char	*want;		/* Macro defined when selected, or NULL. */
	const char	*desc;		/* Description, for --options. */
}	t_optdef;

/** What an interface is: decides how it is probed. */
typedef enum e_kind
{
	KIND_FUNC,		/* A function (maybe also a macro). */
	KIND_MACRO,		/* A macro (object-like or function-like). */
	KIND_TYPE,		/* A type. */
	KIND_EITHER,	/* A macro or a function: setjmp, errno, generic functions. */
	KIND_VAR,		/* An object with external linkage: optarg, environ. */
}	t_kind;

/**
 * One interface of the spec database. An interface of both ISO C and
 * POSIX may have one line per ladder (memccpy: C23, and POSIX.1-2001
 * XSI): the second line is @c alt; the interface is required if either
 * line requires it.
 */
typedef struct s_entry
{
	char			*cat;		/* spec file name (= src/suites/ directory). */
	char			*header;	/* Header to include: string.h, sys/stat.h. */
	char			*name;		/* Identifier. */
	t_kind			kind;		/* Function, macro, type, either, var. */
	char			*since;		/* Version that introduced it, or EXT. */
	char			*opt;		/* Option group (XSI, THREADS, ...), NULL = base. */
	char			*base;		/* Version where it moved to the base. */
	char			*ob;		/* Obsolescent from (comma list). */
	char			*removed;	/* Removed in (comma list). */
	char			*ext;		/* Extension family (GNU, BSD). */
	char			*same;		/* Other headers that declare it too (comma
								   list): the probe checks they agree. */
	char			*file;		/* spec file of the line, for messages. */
	int				line;		/* Line in that file. */
	struct s_entry	*alt;		/* Line of the other ladder, or NULL. */
}	t_entry;

/** The spec database: every spec/<cat>.spec, sorted by category and name. */
typedef struct s_spec
{
	t_entry	*v;			/* Entries (one per interface). */
	size_t	n;			/* Number of entries. */
	int		errors;		/* Parse errors (already reported on stderr). */
}	t_spec;

/** What a target says of one interface. */
typedef enum e_req
{
	REQ_NOT_YET,	/* Newer than the target. */
	REQ_REMOVED,	/* Removed in or before the target. */
	REQ_OPTION_OFF,	/* In an option group that is not selected. */
	REQ_BASE,		/* Required. */
	REQ_OPTION,		/* Required by a selected option group. */
	REQ_EXT,		/* Extension, selected with EXT=1. */
}	t_req;

/**
 * @brief Find a standard by id (posix-2024) or name (POSIX.1-2024).
 *
 * @param s Id or name, case-insensitive.
 * @return The standard, or NULL if unknown.
 */
const t_std		*stdFind(const char *s);

/**
 * @brief The i-th standard, in the order C99 ... C23, POSIX.1-2001 ... 2024.
 *
 * @param i Index.
 * @return The standard, or NULL past the last one.
 */
const t_std		*stdAt(size_t i);

/**
 * @brief Is @p version usable in a spec file or in .since / .until?
 *
 * @param version A standard id or name, or "EXT".
 * @return 1 if valid, 0 otherwise.
 */
int				stdValid(const char *version);

/**
 * @brief Print the standards, one per line (--standards).
 *
 * @param fp Output stream.
 */
void			stdPrintAll(FILE *fp);

/**
 * @brief Does target @p t include @p version?
 *
 * An ISO C edition is included when it is not newer than the target's
 * C base; a POSIX edition when the target is a POSIX edition not older
 * than it; "EXT" when extensions are on.
 *
 * @param t       Target.
 * @param version Standard name, "EXT", or NULL / "" (always included).
 * @return 1 if included, 0 otherwise.
 */
int				stdIncludes(const t_target *t, const char *version);

/**
 * @brief Does target @p t include any version of a comma list?
 *
 * @param t    Target.
 * @param list "POSIX.1-2008,C23", or NULL (never included).
 * @return 1 if one of them is included, 0 otherwise.
 */
int				stdIncludesAny(const t_target *t, const char *list);

/**
 * @brief Find an option group by name (case-insensitive).
 *
 * @param name XSI, THREADS, ...
 * @return The option group, or NULL if unknown.
 */
const t_optdef	*optFind(const char *name);

/**
 * @brief The i-th option group (ISO C features first, then POSIX codes).
 *
 * @param i Index.
 * @return The option group, or NULL past the last one.
 */
const t_optdef	*optAt(size_t i);

/**
 * @brief Check the options of a target.
 *
 * @param t Target.
 * @return 0 if every option is known and fits the target (no POSIX
 *         option on an ISO C target), -1 otherwise (reported on stderr).
 */
int				targetCheck(const t_target *t);

/**
 * @brief Is option group @p opt selected in @p t?
 *
 * @param t   Target.
 * @param opt Option name, case-insensitive.
 * @return 1 if selected, 0 otherwise.
 */
int				targetHasOpt(const t_target *t, const char *opt);

/**
 * @brief What target @p t says of interface @p e.
 *
 * Each line of the entry (and of its other ladder, if any) is decided
 * separately; the strongest requirement wins (base > option > extension
 * > option off > removed > not yet).
 *
 * @param e  Entry of the spec.
 * @param t  Target.
 * @param ob Set to 1 if the interface is obsolescent in the target.
 * @return The requirement.
 */
t_req			specRequired(const t_entry *e, const t_target *t, int *ob);

/**
 * @brief Does @p r put the interface in the run (base, option, extension)?
 *
 * @param r Requirement.
 * @return 1 if selected, 0 otherwise.
 */
int				reqSelected(t_req r);

/**
 * @brief Name of a requirement, for listings ("option off", ...).
 *
 * @param r Requirement.
 * @return Static string.
 */
const char		*reqName(t_req r);

/**
 * @brief Load every <dir>/<cat>.spec into @p sp, sorted.
 *
 * Errors in the files are reported on stderr and counted in
 * sp->errors; the valid lines are still loaded.
 *
 * @param sp  Database to fill (overwritten).
 * @param dir Spec directory.
 * @return 0, or -1 if @p dir cannot be opened (errno set).
 */
int				specLoad(t_spec *sp, const char *dir);

/**
 * @brief Find an interface in the database.
 *
 * @param sp   Database.
 * @param cat  Category (spec file name).
 * @param name Identifier.
 * @return The entry, or NULL.
 */
const t_entry	*specFind(const t_spec *sp, const char *cat, const char *name);

/**
 * @brief Compiler flags of a target: -std=, feature-test and want macros.
 *
 * @param t    Target.
 * @param buf  Destination.
 * @param size Size of @p buf.
 */
void			targetCflags(const t_target *t, char *buf, size_t size);

/**
 * @brief Human description of a target: "POSIX.1-2024 + XSI".
 *
 * @param t    Target.
 * @param buf  Destination.
 * @param size Size of @p buf.
 */
void			targetDescribe(const t_target *t, char *buf, size_t size);

/* ----- Runs ----- */

/** Final status of one test. */
typedef enum e_status
{
	ST_PASS,		/* Every check passed (and the expected outcome happened). */
	ST_FAIL,		/* A check failed, or the expected exit / signal did not. */
	ST_SKIP,		/* The test called SKIP. */
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
}	t_probe;

/** One test binary: src/suites/<cat>/<func>.c -> <bindir>/<cat>/<func>. */
typedef struct s_unit
{
	char			*cat;	/* Category (directory). */
	char			*func;	/* Function (file name). */
	char			*bin;	/* NULL if the binary does not exist. */
	char			*err;	/* First lines of the build log if unavailable. */
	const t_entry	*entry;	/* Spec entry, NULL if unknown. */
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
	char		*msgs;		/* @fail / @skip / @log lines, formatted. */
	char		*out;		/* Captured stdout + stderr (truncated). */
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
	const char	*tsvOut;	/* -o: TSV results, or NULL. */
	const char	*jsonOut;	/* -J: JSON results, or NULL. */
}	t_opts;

/** Everything about one invocation of lcrun. */
typedef struct s_run
{
	t_opts	o;					/* Options. */
	t_spec	spec;				/* Spec database. */
	t_probe	*probes;			/* One per spec entry. */
	t_unit	*units;				/* Selected units, sorted. */
	size_t	nUnits;				/* Number of units. */
	t_test	*tests;				/* Selected tests, in source order. */
	size_t	nTests;				/* Number of tests. */
	size_t	nOutOfTarget;		/* Tests excluded by their since / until / opt. */
	size_t	nBadVersion;		/* Tests whose since / until / opt is unknown. */
	size_t	nUnitsOutOfTarget;	/* Units whose function the target lacks. */
	double	wallMs;				/* Wall time of the whole run. */
}	t_run;

/* ----- Utilities (main.c) ----- */

/**
 * @brief malloc() that exits on failure (status 2).
 *
 * @param n Size (0 allocates 1 byte).
 * @return The block.
 */
void		*xmalloc(size_t n);

/**
 * @brief realloc() that exits on failure (status 2).
 *
 * @param p Block, or NULL.
 * @param n New size (0 means 1 byte).
 * @return The block.
 */
void		*xrealloc(void *p, size_t n);

/**
 * @brief strdup() that exits on failure.
 *
 * @param s String.
 * @return New copy.
 */
char		*xstrdup(const char *s);

/**
 * @brief Copy the first @p n bytes of @p s into a new string.
 *
 * @param s Source (at least @p n bytes).
 * @param n Number of bytes.
 * @return New NUL-terminated string.
 */
char		*xstrndup(const char *s, size_t n);

/**
 * @brief Append @p n bytes of @p s to the heap string *@p dst.
 *
 * @param dst Heap string, or pointer to NULL (then created).
 * @param s   Bytes to append.
 * @param n   Number of bytes.
 */
void		strAppend(char **dst, const char *s, size_t n);

/**
 * @brief Monotonic clock, in milliseconds.
 *
 * @return Current time (only differences are meaningful).
 */
double		nowMs(void);

/**
 * @brief Name of a status for TSV / JSON ("pass", "unavailable", ...).
 *
 * @param st Status.
 * @return Static string.
 */
const char	*statusName(t_status st);

/**
 * @brief Split a comma list ("a,b,c") into a NULL-terminated array.
 *
 * @param s List.
 * @return New array of new strings, or NULL for an empty list.
 */
char		**splitList(const char *s);

/**
 * @brief Is @p s in a filter list?
 *
 * @param list NULL-terminated list; NULL means "no filter".
 * @param s    String to look for.
 * @return 1 if found or @p list is NULL, 0 otherwise.
 */
int			inList(char **list, const char *s);

/* ----- Spec tools (tools.c) ----- */

/**
 * @brief Name of a kind, as written in spec files ("function", ...).
 *
 * @param k Kind.
 * @return Static string.
 */
const char	*kindName(t_kind k);

/**
 * @brief --spec-make: the make variables of the target, on stdout.
 *
 * LC_TARGET, LC_STD_CFLAGS, LC_EXT_CFLAGS, LC_UNITS, LC_EXT_UNITS,
 * LC_PROBES_LINK, LC_PROBES_DECL (read by mk/target.mk).
 *
 * @param r Run, with its units discovered (without the -c / -f filters).
 * @return 0.
 */
int			toolMake(const t_run *r);

/**
 * @brief --spec-probes: one probe program per required interface.
 *
 * @param r   Run.
 * @param dir Output directory: <dir>/<cat>/<name>.c.
 * @return 0, or 2 if a file cannot be written.
 */
int			toolProbes(const t_run *r, const char *dir);

/**
 * @brief --spec-list: what the target says of every interface.
 *
 * @param r Run (-v: the excluded interfaces too).
 * @return 0.
 */
int			toolList(const t_run *r);

/**
 * @brief --spec-check: lint the database and the suites against it.
 *
 * @param r Run.
 * @return 0 without error, 1 with errors, 2 if the suites cannot be read.
 */
int			toolCheck(const t_run *r);

/**
 * @brief --options: the option groups and what they define.
 *
 * @return 0.
 */
int			toolOptions(void);

/**
 * @brief --spec-matrix: tested / required per header, in the 8 standards.
 *
 * @param r Run (its options are applied to each standard they fit).
 * @return 0.
 */
int			toolMatrix(const t_run *r);

/**
 * @brief --spec-todo: interfaces the target requires that have no unit.
 *
 * @param r Run.
 * @return 0.
 */
int			toolTodo(const t_run *r);

/* ----- Execution (exec.c) ----- */

/**
 * @brief Run every selected test (r->o.repeat times), r->o.jobs at a time.
 *
 * Fills the result fields of each t_test. Exits with status 2 if a
 * process cannot be created.
 *
 * @param r Run.
 */
void		runAll(t_run *r);

/* ----- Reports (report.c) ----- */

/**
 * @brief Print the console report of a run (see report.c for the sections).
 *
 * @param r Run, after runAll().
 */
void		reportConsole(const t_run *r);

/**
 * @brief Write the results as TSV (input of --compare).
 *
 * One row per test, per unavailable unit, and per required interface
 * (test "@interface", status = probe result).
 *
 * @param r    Run.
 * @param path Output file.
 * @return 0, or -1 on error (errno set).
 */
int			reportTsv(const t_run *r, const char *path);

/**
 * @brief Write the results as JSON (every field, the target, interfaces).
 *
 * @param r    Run.
 * @param path Output file.
 * @return 0, or -1 on error (errno set).
 */
int			reportJson(const t_run *r, const char *path);

/**
 * @brief --compare: what changed between two TSV results.
 *
 * @param a     TSV of the first run.
 * @param b     TSV of the second run.
 * @param color Use ANSI colors.
 * @return 1 if something passed in @p a and does not in @p b, 0 otherwise.
 */
int			compareTsv(const char *a, const char *b, int color);

#endif /* LCRUN_H */
