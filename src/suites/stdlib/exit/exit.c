/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file exit.c
 * @brief Tests for exit() (<stdlib.h>).
 * @Created: 2026/11/27 13:53:05 by Moutig
 * @Updated: 2026/11/27 13:55:03 by Moutig
 *
 * exit(status) ends the program normally: first the functions registered
 * with atexit() are called in the reverse order of their registration
 * (their own cases are in atexit.c), then the open streams are flushed
 * and closed, then control returns to the host: 0 or EXIT_SUCCESS give a
 * successful termination, EXIT_FAILURE an unsuccessful one; it does not
 * return (7.24.4.4). POSIX: any status, the waiting parent gets status &
 * 0377.
 *
 * The flush of the streams (7.24.4.4p4) is tested with <stdio.h>, in its
 * own units: a libc whose <stdio.h> is incomplete must not hide exit().
 *
 * Every case runs exit() in a child (exit_cases.h). EXIT_SUCCESS and
 * EXIT_FAILURE are only used where they are tested: a <stdlib.h> without
 * them (their defs.c cases say it) does not hide the rest of exit().
 *
 * Clauses: C99 7.20.4.3; C11, C17 7.22.4.4; C23 7.24.4.4. POSIX: XSH
 * exit().
 */
#include "stdlib/exit_cases.h"

TEST_OPT(prototype, .ref = "C11 7.22.4.4p1")
{
	EXPECT_TYPE(&exit, void (*)(int));
}

static void	exitWith(void *arg)
{
	exit(*(int *)arg);
}

/* The status reaches the parent: 0 to 255. */
TEST_OPT(status_is_returned, .ref = "C11 7.22.4.4p5")
{
	static const int	st[] = {0, 1, 2, 42, 125, 126, 255};

	for (int i = 0; i < 7; i++)
	{
		int	s = st[i];

		EXPECT_EQ(lc_child(exitWith, &s), LC_EXIT(st[i]));
	}
}

/* EXIT_SUCCESS is a successful termination (0), EXIT_FAILURE is not. */
TEST_OPT(exit_success_and_failure, .ref = "C11 7.22.4.4p5")
{
#if defined(EXIT_SUCCESS) && defined(EXIT_FAILURE)
	int	s = EXIT_SUCCESS;
	int	f = EXIT_FAILURE;
	int	r;

	EXPECT_EQ(lc_child(exitWith, &s), LC_EXIT(0));
	r = lc_child(exitWith, &f);
	EXPECT_EQ(r & ~0xff, LC_EXIT(0) & ~0xff);
	EXPECT_NE(r, LC_EXIT(0));
#else
	NOT_IMPLEMENTED("EXIT_SUCCESS / EXIT_FAILURE (<stdlib.h>)");
#endif
}

/* POSIX: only the low 8 bits reach a waiting parent. */
TEST_OPT(parent_gets_the_low_8_bits, .since = "POSIX.1-2001", .ref = "XSH exit()")
{
	int	a = 256 + 3;
	int	b = 0x7f00 + 42;
	int	c = -1;

	EXPECT_EQ(lc_child(exitWith, &a), LC_EXIT(3));
	EXPECT_EQ(lc_child(exitWith, &b), LC_EXIT(42));
	EXPECT_EQ(lc_child(exitWith, &c), LC_EXIT(255));
}

/* exit() does not return: nothing after the call runs. */
static void	exitThenLog(void *arg)
{
	(void)arg;
	ex_put(1);
	exit(7);
	ex_put(2);
}

TEST_OPT(does_not_return, .ref = "C11 7.22.4.4p6")
{
	static const int	want[] = {1};

	EX_LOG();
	EXPECT_EQ(lc_child(exitThenLog, NULL), LC_EXIT(7));
	EXPECT_TRUE(ex_order(want, 1));
}

/* The registered functions run, before the process ends. */
static void	registerThenExit(void *arg)
{
	(void)arg;
	if (atexit(ex_h1) != 0 || atexit(ex_h2) != 0)
		lc_exitNow(99);
	exit(5);
}

TEST_OPT(calls_the_registered_functions, .ref = "C11 7.22.4.4p3")
{
	static const int	want[] = {2, 1};

	EX_LOG();
	EXPECT_EQ(lc_child(registerThenExit, NULL), LC_EXIT(5));
	EXPECT_TRUE(ex_order(want, 2));
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	int	s = 9;

	EXPECT_EQ(lc_child(exitWith, &s), LC_EXIT(9));
}
