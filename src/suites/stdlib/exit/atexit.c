/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file atexit.c
 * @brief Tests for atexit() (<stdlib.h>).
 * @Created: 2026/11/27 13:52:54 by Moutig
 * @Updated: 2026/11/27 13:54:41 by Moutig
 *
 * atexit(func) registers func to be called without arguments at normal
 * program termination; 0 when the registration succeeds, non-zero
 * otherwise; at least 32 functions can be registered (7.24.4.2). exit()
 * calls them in the reverse order of their registration, each as many
 * times as it was registered, and a function registered while they run
 * is called after the ones already called (7.24.4.4p3, footnote 349).
 * Returning from main() is a call to exit() (5.1.2.3.4): not testable
 * from a test body.
 *
 * Every case registers and exits in a child (exit_cases.h).
 *
 * Clauses: C99 7.20.4.2; C11, C17 7.22.4.2; C23 7.24.4.2. POSIX: XSH
 * atexit().
 */
#include "stdlib/exit_cases.h"

TEST_OPT(prototype, .ref = "C11 7.22.4.2p1")
{
	EXPECT_TYPE(&atexit, int (*)(void (*)(void)));
}

static void	(*const g_h[8])(void) = {ex_h1, ex_h2, ex_h3, ex_h4, ex_h5, ex_h6, ex_h7, ex_h8};

/* Register h1 ... h8, check every return value, exit(0). */
static void	eight(void *arg)
{
	(void)arg;
	for (int i = 0; i < 8; i++)
		if (atexit(g_h[i]) != 0)
			lc_exitNow(90 + i);
	exit(0);
}

TEST_OPT(registration_returns_zero, .ref = "C11 7.22.4.2p4")
{
	EX_LOG();
	EXPECT_EQ(lc_child(eight, NULL), LC_EXIT(0));
}

TEST_OPT(called_in_reverse_order, .ref = "C11 7.22.4.4p3")
{
	static const int	want[] = {8, 7, 6, 5, 4, 3, 2, 1};

	EX_LOG();
	EXPECT_EQ(lc_child(eight, NULL), LC_EXIT(0));
	EXPECT_TRUE(ex_order(want, 8));
}

/* A function registered three times is called three times, in place. */
static void	repeated(void *arg)
{
	(void)arg;
	if (atexit(ex_h1) || atexit(ex_h2) || atexit(ex_h1) || atexit(ex_h3) || atexit(ex_h1))
		lc_exitNow(99);
	exit(0);
}

TEST_OPT(called_as_many_times_as_registered, .ref = "C23 7.24.4.4p3 (footnote 349)")
{
	static const int	want[] = {1, 3, 1, 2, 1};

	EX_LOG();
	EXPECT_EQ(lc_child(repeated, NULL), LC_EXIT(0));
	EXPECT_TRUE(ex_order(want, 5));
}

/* h4 registers h5 while the functions run. */
static void	registerLater(void)
{
	ex_put(4);
	if (atexit(ex_h5) != 0)
		ex_put(-1);
}

static void	nested(void *arg)
{
	(void)arg;
	if (atexit(ex_h1) || atexit(registerLater) || atexit(ex_h3))
		lc_exitNow(99);
	exit(0);
}

/*
 * A function registered during exit() is called after the ones already
 * called when it was registered: h3, then h4 (which registers h5), then
 * h5, then h1.
 */
TEST_OPT(registered_during_exit_is_called_next, .ref = "C11 7.22.4.4p3")
{
	static const int	want[] = {3, 4, 5, 1};

	EX_LOG();
	EXPECT_EQ(lc_child(nested, NULL), LC_EXIT(0));
	EXPECT_TRUE(ex_order(want, 4));
}

/* 32 registrations (the minimum) and more: all succeed, all called in reverse. */
static void	many(void *arg)
{
	int	n = *(int *)arg;

	for (int i = 0; i < n; i++)
		if (atexit(g_h[i % 8]) != 0)
			lc_exitNow(i < 32 ? 98 : 97);
	exit(0);
}

TEST_OPT(at_least_32_registrations, .ref = "C11 7.22.4.2p3")
{
	int	want[32];
	int	n = 32;

	for (int i = 0; i < 32; i++)
		want[i] = (31 - i) % 8 + 1;
	EX_LOG();
	EXPECT_EQ(lc_child(many, &n), LC_EXIT(0));
	EXPECT_TRUE(ex_order(want, 32));
}

/* Beyond 32: a registration may fail (non-zero), but never an earlier one. */
TEST_OPT(more_than_32_registrations, .ref = "C11 7.22.4.2p3-4")
{
	int	n = 300;
	int	r;

	EX_LOG();
	r = lc_child(many, &n);
	EXPECT_TRUE(r == LC_EXIT(0) || r == LC_EXIT(97));
	if (r == LC_EXIT(0))
	{
		EXPECT_EQ(ex_log()->n, 300);
		EXPECT_EQ(ex_log()->v[0], 299 % 8 + 1);
		EXPECT_EQ(ex_log()->v[299], 1);
	}
	else
		LOG("a registration beyond 32 failed (allowed)");
}

/* The functions also run when the status is not a success. */
static void	failing(void *arg)
{
	(void)arg;
	if (atexit(ex_h6) != 0)
		lc_exitNow(99);
	exit(3);
}

TEST_OPT(called_whatever_the_status, .ref = "C11 7.22.4.4p3")
{
	static const int	want[] = {6};

	EX_LOG();
	EXPECT_EQ(lc_child(failing, NULL), LC_EXIT(3));
	EXPECT_TRUE(ex_order(want, 1));
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_TYPE(&(atexit), int (*)(void (*)(void)));
}
