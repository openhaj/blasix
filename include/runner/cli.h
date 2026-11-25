/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file cli.h
 * @brief Command line of lcrun (cli.c).
 * @Created: 2026/11/24 08:27:32 by Moutig
 * @Updated: 2026/11/25 09:28:14 by Moutig
 *
 *
 * The command of an invocation (t_mode) and the options (t_opts, run.h).
 */

#ifndef LCRUN_CLI_H
# define LCRUN_CLI_H

# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include "runner/run.h"

/** What this invocation does (one per command of usage()). */
typedef enum e_mode
{
	MODE_RUN,		/* lcrun BINDIR: run the tests. */
	MODE_MAKE,		/* --spec-make */
	MODE_PROBES,	/* --spec-probes DIR */
	MODE_LIST,		/* --spec-list */
	MODE_CHECK,		/* --spec-check */
	MODE_MATRIX,	/* --spec-matrix */
	MODE_TODO,		/* --spec-todo */
	MODE_CACHE		/* lcrun --cache-status BINDIR */
}	t_mode;

/* ----- Shared between the files of the module ----- */

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
int	parseOpts(t_opts *o, int argc, char **argv, t_mode *mode,
		const char **probeOut);

#endif
