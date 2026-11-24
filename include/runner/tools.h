/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file tools.h
 * @brief The spec tools of lcrun (tools/).
 * @Created: 2026/11/24 08:28:33 by Moutig
 * @Updated: 2026/11/24 08:29:22 by Moutig
 *
 * tools/make.c      --spec-make, the make fragment of a target;
 * tools/probes.c    --spec-probes, the probe sources;
 * tools/list.c      --spec-list, --options;
 * tools/check.c     --spec-check;
 * tools/matrix.c    --spec-matrix, --spec-todo;
 * tools/coverage.c  is an interface tested (shared by check and matrix).
 */

#ifndef LCRUN_TOOLS_H
# define LCRUN_TOOLS_H

# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include "runner/run.h"

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


/* ----- Shared between the files of the module ----- */

/**
 * @brief Does <cat>/defs.c (or a bits/ file it includes) have cases for
 *        @p e: the token LC_REQ_<name>, that guards them?
 *
 * @param r Run.
 * @param e Interface.
 * @return 1 or 0.
 */
int	defsCovers(const t_run *r, const t_entry *e);

/**
 * @brief Is there a file of its own for @p e, in any group of its
 *        category?
 *
 * The suites are scanned once.
 *
 * @param r Run.
 * @param e Interface.
 * @return 1 or 0.
 */
int	ownUnit(const t_run *r, const t_entry *e);

/**
 * @brief Is @p e tested: by a unit of its own, or (a definition) by the
 *        cases of <cat>/defs.c?
 *
 * @param r Run.
 * @param e Interface.
 * @return 1 or 0.
 */
int	hasUnit(const t_run *r, const t_entry *e);

#endif
