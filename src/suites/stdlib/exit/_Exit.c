/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file _Exit.c
 * @brief Tests for _Exit() (<stdlib.h>).
 * @Created: 2026/11/27 13:52:46 by Moutig
 * @Updated: 2026/11/27 13:54:27 by Moutig
 *
 * _Exit(status) ends the program and returns control to the host with
 * the status as exit() would; no function registered with atexit() (or
 * at_quick_exit()) is called; whether streams are flushed is
 * implementation-defined (not tested); it does not return (7.24.4.5).
 * POSIX: the same as _exit(); the waiting parent gets status & 0377.
 *
 * Every case runs _Exit() in a child (exit_cases.h).
 *
 * Clauses: C99 7.20.4.4; C11, C17 7.22.4.5; C23 7.24.4.5. POSIX: XSH
 * _Exit().
 */
#include "stdlib/exit_cases.h"

TEST_OPT(prototype, .ref = "C11 7.22.4.5p1")
{
	EXPECT_TYPE(&_Exit, void (*)(int));
}

static void	exitWith(void *arg)
{
	_Exit(*(int *)arg);
}

TEST_OPT(status_is_returned, .ref = "C11 7.22.4.5p2")
{
	static const int	st[] = {0, 1, 2, 42, 125, 255};

	for (int i = 0; i < 6; i++)
	{
		int	s = st[i];

		EXPECT_EQ(lc_child(exitWith, &s), LC_EXIT(st[i]));
	}
}

TEST_OPT(exit_success_and_failure, .ref = "C11 7.22.4.5p2")
{
#if defined(EXIT_SUCCESS) && defined(EXIT_FAILURE)
	int	s = EXIT_SUCCESS;
	int	f = EXIT_FAILURE;

	EXPECT_EQ(lc_child(exitWith, &s), LC_EXIT(0));
	EXPECT_NE(lc_child(exitWith, &f), LC_EXIT(0));
#else
	NOT_IMPLEMENTED("EXIT_SUCCESS / EXIT_FAILURE (<stdlib.h>)");
#endif
}

TEST_OPT(parent_gets_the_low_8_bits, .since = "POSIX.1-2001", .ref = "XSH _Exit()")
{
	int	a = 512 + 9;
	int	c = -2;

	EXPECT_EQ(lc_child(exitWith, &a), LC_EXIT(9));
	EXPECT_EQ(lc_child(exitWith, &c), LC_EXIT(254));
}

/* Registered functions are not called. */
static void	registerThenExit(void *arg)
{
	(void)arg;
	if (atexit(ex_h1) != 0 || atexit(ex_h2) != 0)
		lc_exitNow(99);
	ex_put(7);
	_Exit(3);
	ex_put(8);
}

TEST_OPT(registered_functions_are_not_called, .ref = "C11 7.22.4.5p2")
{
	static const int	want[] = {7};

	EX_LOG();
	EXPECT_EQ(lc_child(registerThenExit, NULL), LC_EXIT(3));
	EXPECT_TRUE(ex_order(want, 1));
}

/* From inside a function registered with atexit(): the others are not called. */
static void	exitInHandler(void)
{
	ex_put(2);
	_Exit(4);
}

static void	handlerExits(void *arg)
{
	(void)arg;
	if (atexit(ex_h1) != 0 || atexit(exitInHandler) != 0 || atexit(ex_h3) != 0)
		lc_exitNow(99);
	exit(0);
}

TEST_OPT(ends_at_once_from_a_registered_function, .ref = "C11 7.22.4.5p2")
{
	static const int	want[] = {3, 2};

	EX_LOG();
	EXPECT_EQ(lc_child(handlerExits, NULL), LC_EXIT(4));
	EXPECT_TRUE(ex_order(want, 2));
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	int	s = 6;

	EXPECT_EQ(lc_child(exitWith, &s), LC_EXIT(6));
	EXPECT_TYPE(&(_Exit), void (*)(int));
}
