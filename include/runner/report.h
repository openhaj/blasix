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
 * @Updated: 2026/11/24 08:29:08 by Moutig
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

#endif
