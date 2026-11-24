/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file report.h
 * @brief Reports of a run (report/).
 * @Created: 2026/11/24 08:27:51 by Moutig
 * @Updated: 2026/11/24 10:24:58 by Moutig
 *
 * report/console.c      the console report;
 * report/conformance.c  its conformance table;
 * report/export.c       TSV and JSON results;
 * report/compare.c      --compare of two TSV results.
 */

#ifndef LCRUN_REPORT_H
# define LCRUN_REPORT_H

# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include "runner/run.h"

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


/* ----- Colors, shared by the files of the module ----- */

/** Use ANSI colors (set from the run, or by compareTsv()). */
extern int	gColor;

/* Each C_* expands to an escape sequence, or to "" without colors. */
# define C_RESET	(gColor ? "\033[0m" : "")
# define C_BOLD		(gColor ? "\033[1m" : "")
# define C_DIM		(gColor ? "\033[2m" : "")
# define C_RED		(gColor ? "\033[31m" : "")
# define C_GREEN	(gColor ? "\033[32m" : "")
# define C_YEL		(gColor ? "\033[33m" : "")
# define C_MAG		(gColor ? "\033[35m" : "")
# define C_CYAN		(gColor ? "\033[36m" : "")

/* ----- Shared between the files of the module ----- */

/**
 * @brief Percentage, 0 when @p whole is 0.
 *
 * @param part  Part.
 * @param whole Whole.
 * @return 100 * part / whole.
 */
double	pct(int part, int whole);

/**
 * @brief Conformance per header for the target.
 *
 * For each required interface: its probe result (present, undeclared,
 * unlinked) and whether its tests ran and passed. Lists the undeclared,
 * unlinked, failing and untested ones. In strict mode, also the
 * interfaces the target forbids and the header declares anyway (leak).
 *
 * @param r Run.
 */
void	sectionConformance(const t_run *r);

/* ----- What a run says of each required interface (conformance.c) ----- */

/**
 * Verdict on one required interface, first matching line wins. Shared by
 * the console table and the JSON, so both always agree.
 */
typedef enum e_verdict
{
	VD_MISSING,		/* Not implemented: undeclared or unlinked (or, without
					   probes, its unit did not build for that reason). */
	VD_BROKEN,		/* Its probe did not link for another reason. */
	VD_FAILING,		/* A test failed, crashed, timed out or ended early. */
	VD_VERIFIED,	/* Tests ran and every one passed. */
	VD_UNBUILT,		/* Its unit did not build, for another reason. */
	VD_UNTESTED,	/* No test ran: none written, or all skipped / not
					   implemented. */
	VD_COUNT,		/* Number of verdicts (array size). */
}	t_verdict;

/** Tests of one interface and the verdict. */
typedef struct s_iresult
{
	const t_unit	*unit;		/* Its unit (own, or <cat>/defs), or NULL. */
	int				tests;		/* Tests attached to it. */
	int				passed;		/* ... that passed. */
	int				failed;		/* ... that failed (statusFailed()). */
	int				skipped;	/* ... that were skipped. */
	int				unimpl;		/* ... that were not implemented. */
	t_verdict		verdict;
}	t_iresult;

/** Counters of one header for the target (a row of the table). */
typedef struct s_conf
{
	int	req;		/* required interfaces (base + options + extensions) */
	int	opt;		/* of which in an option group */
	int	ob;			/* of which obsolescent */
	int	ok;			/* declared and linkable */
	int	undecl;		/* header does not declare it */
	int	unlinked;	/* declared, library does not define it */
	int	broken;		/* declared, the probe did not link for another reason */
	int	unknown;	/* not probed */
	int	tested;		/* at least one test ran (passed or failed) */
	int	pass;		/* verified: tests ran, every one passed */
	int	fail;		/* failing: a test failed */
	int	unbuilt;	/* its unit did not build (not for a missing interface) */
	int	untested;	/* present (or not probed), no test ran */
	int	leak;		/* must be hidden (strict mode), visible anyway */
	int	nodiag;		/* reject probes that compiled (diagnostic missing) */
}	t_conf;

/**
 * @brief Name of a verdict in the JSON ("verified", "missing", ...).
 *
 * @param v Verdict.
 * @return Static string.
 */
const char	*verdictName(t_verdict v);

/**
 * @brief Tests and verdict of the interface r->spec.v[@p i].
 *
 * @param r Run.
 * @param i Index of the entry in r->spec.v.
 * @param x Filled in.
 */
void		interfaceResult(const t_run *r, size_t i, t_iresult *x);

/**
 * @brief Why a unit did not build: "unimplemented" (the interface it tests
 *        is missing from the libc) or "build" (anything else: compiler
 *        error or crash, missing helper, ...).
 *
 * @param r Run.
 * @param u Unit without a binary.
 * @return Static string.
 */
const char	*unitReason(const t_run *r, const t_unit *u);

/**
 * @brief Is the interface r->spec.v[@p i] left out by -c / -f?
 *
 * @param r Run.
 * @param i Index of the entry.
 * @return 1 if filtered out, 0 if selected.
 */
int			interfaceFiltered(const t_run *r, size_t i);

/**
 * @brief Counters of the header whose first entry is r->spec.v[@p i].
 *
 * @param r Run.
 * @param i First entry of the header (category).
 * @param c Filled in.
 * @return Index of the first entry of the next header.
 */
size_t		confHeader(const t_run *r, size_t i, t_conf *c);

#endif
