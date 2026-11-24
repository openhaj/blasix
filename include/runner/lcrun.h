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
 * @Updated: 2026/11/24 08:05:37 by Moutig
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

# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include "runner/util.h"
# include "runner/spec.h"
# include "runner/run.h"
# include "runner/cli.h"
# include "runner/tools.h"
# include "runner/report.h"

#endif
