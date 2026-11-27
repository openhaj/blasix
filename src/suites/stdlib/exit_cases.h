/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file exit_cases.h
 * @brief Helpers of the termination units (exit, atexit, _Exit).
 * @Created: 2026/11/27 13:52:33 by Moutig
 * @Updated: 2026/11/27 13:53:43 by Moutig
 *
 * A termination ends the process: every case runs it in a child
 * (lc_child: the checks of the child count, its exit status comes back
 * as LC_EXIT(n)). The functions registered with atexit() write their
 * number in a log shared with the parent (an anonymous shared mapping,
 * without the libc), which the parent reads after the child ended:
 *
 *   - ex_log, ex_logReset: the shared log (numbers in call order);
 *   - ex_h1 ... ex_h8: registrable functions that log 1 ... 8;
 *   - ex_order: the log is exactly the given sequence.
 *
 * Registration and exit happen in the child only: the test process
 * itself registers nothing.
 */

#ifndef EXIT_CASES_H
# define EXIT_CASES_H

# include "stdlib/stdlib_cases.h"
# include "framework/sys.h"

/** The shared log: n numbers in call order. */
typedef struct s_exLog
{
	int	n;
	int	v[512];
}	t_exLog;

/* The log, mapped once, shared with the children. */
static inline t_exLog	*ex_log(void)
{
	static t_exLog	*log;

	if (log == NULL)
		log = lc_sysMmapShared(sizeof(*log));
	return (log);
}

static inline void	ex_logReset(void)
{
	if (ex_log())
		ex_log()->n = 0;
}

static inline void	ex_put(int v)
{
	t_exLog	*l = ex_log();

	if (l && l->n < 512)
		l->v[l->n++] = v;
}

static inline void	ex_h1(void) { ex_put(1); }
static inline void	ex_h2(void) { ex_put(2); }
static inline void	ex_h3(void) { ex_put(3); }
static inline void	ex_h4(void) { ex_put(4); }
static inline void	ex_h5(void) { ex_put(5); }
static inline void	ex_h6(void) { ex_put(6); }
static inline void	ex_h7(void) { ex_put(7); }
static inline void	ex_h8(void) { ex_put(8); }

/* Is the log exactly want[0 .. n)? */
static inline int	ex_order(const int *want, int n)
{
	t_exLog	*l = ex_log();

	if (l == NULL || l->n != n)
		return (0);
	for (int i = 0; i < n; i++)
		if (l->v[i] != want[i])
			return (0);
	return (1);
}

/* Stop the case when the shared log cannot be mapped. */
# define EX_LOG()																\
	if (ex_log() == NULL)														\
		SKIP("no shared mapping");												\
	ex_logReset()

#endif
