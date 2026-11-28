/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file _exit.c
 * @brief Tests for _exit() (<unistd.h>).
 * @Created: 2026/11/28 10:12:36 by Moutig
 * @Updated: 2026/11/28 10:13:08 by Moutig
 *
 * _exit(status) is _Exit(): it ends the process at once; the waiting
 * parent gets status & 0377 (wait(), waitpid()); no function registered
 * with atexit() is called; it does not return. Whether streams are
 * flushed is implementation-defined (not tested).
 *
 * Each case runs _exit() in a child (lc_child: its exit status comes
 * back as LC_EXIT(n)); the functions registered with atexit() (of
 * <stdlib.h>, only to prepare the case) write to a shared mapping.
 *
 * Clauses: POSIX: XSH _Exit() (_exit).
 */
#include "unistd/unistd_cases.h"
#include <stdlib.h>

TEST_OPT(prototype, .ref = "XSH _Exit()")
{
	EXPECT_TYPE(&_exit, void (*)(int));
}

static void	exitWith(void *arg)
{
	_exit(*(int *)arg);
}

/* The status reaches the parent; only its low 8 bits. */
TEST_OPT(status_reaches_the_parent, .ref = "XSH _Exit()")
{
	static const int	st[] = {0, 1, 42, 125, 255, 256 + 7, -1};
	static const int	want[] = {0, 1, 42, 125, 255, 7, 255};

	for (int i = 0; i < 7; i++)
	{
		int	s = st[i];

		EXPECT_EQ(lc_child(exitWith, &s), LC_EXIT(want[i]));
	}
}

/* A log shared with the parent: what ran in the child. */
static int	*g_log;

static void	logHandler(void)
{
	if (g_log)
		g_log[g_log[0]++ + 1] = 7;
}

static void	registerThenExit(void *arg)
{
	(void)arg;
	g_log[0] = 0;
	if (atexit(logHandler) != 0)
		lc_exitNow(99);
	g_log[g_log[0]++ + 1] = 1;
	_exit(3);
	g_log[g_log[0]++ + 1] = 2;
}

/* No registered function runs, and nothing after the call. */
TEST_OPT(registered_functions_are_not_called, .ref = "XSH _Exit()")
{
	g_log = lc_sysMmapShared(16 * sizeof(int));
	ASSERT_NOTNULL(g_log);
	EXPECT_EQ(lc_child(registerThenExit, NULL), LC_EXIT(3));
	EXPECT_EQ(g_log[0], 1);
	EXPECT_EQ(g_log[1], 1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_TYPE(&(_exit), void (*)(int));
}
