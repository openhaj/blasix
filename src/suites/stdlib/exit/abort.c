/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file abort.c
 * @brief Tests for abort() (<stdlib.h>).
 * @Created: 2026/11/27 14:04:16 by Moutig
 * @Updated: 2026/11/27 14:07:19 by Moutig
 *
 * abort() causes abnormal termination, unless SIGABRT is caught and the
 * handler does not return; the unsuccessful status is returned by means
 * of raise(SIGABRT); it does not return (7.24.4.1). Abnormal termination
 * is not exit(): no function registered with atexit() runs. Whether
 * streams are flushed is implementation-defined (not tested).
 * POSIX (CX): the waiting parent sees a process terminated by SIGABRT,
 * and abort() overrides a SIGABRT that is blocked or ignored.
 *
 * The first case ends the test process itself (.expect); the others run
 * abort() in a child (lc_child: LC_SIGNAL(s) when a signal killed it)
 * and set the signal disposition with raw syscalls (framework/sys.h).
 *
 * Clauses: C99 7.20.4.1; C11, C17 7.22.4.1; C23 7.24.4.1. POSIX: XSH
 * abort().
 */
#include "stdlib/exit_cases.h"

TEST_OPT(prototype, .ref = "C11 7.22.4.1p1")
{
	EXPECT_TYPE(&abort, void (*)(void));
}

/* The test process itself ends by SIGABRT. */
TEST_OPT(terminates_the_process_by_sigabrt, .expect = LC_SIGNAL(LC_SIGABRT),
	.ref = "C11 7.22.4.1p2")
{
	abort();
}

static void	justAbort(void *arg)
{
	(void)arg;
	ex_put(1);
	abort();
	ex_put(2);
}

/* Killed by SIGABRT, and nothing after the call runs. */
TEST_OPT(does_not_return, .ref = "C11 7.22.4.1p3")
{
	static const int	want[] = {1};

	EX_LOG();
	EXPECT_EQ(lc_child(justAbort, NULL), LC_SIGNAL(LC_SIGABRT));
	EXPECT_TRUE(ex_order(want, 1));
}

static void	registerThenAbort(void *arg)
{
	(void)arg;
	if (atexit(ex_h1) != 0 || atexit(ex_h2) != 0)
		lc_exitNow(99);
	abort();
}

/* Abnormal termination: the functions registered with atexit() do not run. */
TEST_OPT(registered_functions_are_not_called, .ref = "C11 7.22.4.1p2")
{
	EX_LOG();
	EXPECT_EQ(lc_child(registerThenAbort, NULL), LC_SIGNAL(LC_SIGABRT));
	EXPECT_TRUE(ex_order(NULL, 0));
}

static void	ignoredThenAbort(void *arg)
{
	(void)arg;
	if (lc_sysSigaction(LC_SIGABRT, LC_SIG_IGN, 0) != 0)
		lc_exitNow(99);
	abort();
}

/* POSIX: SIGABRT ignored, abort() still terminates by SIGABRT. */
TEST_OPT(overrides_an_ignored_sigabrt, .since = "POSIX.1-2001", .ref = "XSH abort()")
{
	EXPECT_EQ(lc_child(ignoredThenAbort, NULL), LC_SIGNAL(LC_SIGABRT));
}

static void	blockedThenAbort(void *arg)
{
	(void)arg;
	if (lc_sysBlockSignal(LC_SIGABRT) != 0)
		lc_exitNow(99);
	abort();
}

/* POSIX: SIGABRT blocked, abort() still terminates by SIGABRT. */
TEST_OPT(overrides_a_blocked_sigabrt, .since = "POSIX.1-2001", .ref = "XSH abort()")
{
	EXPECT_EQ(lc_child(blockedThenAbort, NULL), LC_SIGNAL(LC_SIGABRT));
}

static void	blockedAndIgnoredThenAbort(void *arg)
{
	(void)arg;
	if (lc_sysBlockSignal(LC_SIGABRT) != 0 || lc_sysSigaction(LC_SIGABRT, LC_SIG_IGN, 0) != 0)
		lc_exitNow(99);
	abort();
}

TEST_OPT(overrides_a_blocked_and_ignored_sigabrt, .since = "POSIX.1-2001", .ref = "XSH abort()")
{
	EXPECT_EQ(lc_child(blockedAndIgnoredThenAbort, NULL), LC_SIGNAL(LC_SIGABRT));
}

/* A handler that returns: it runs (raise(SIGABRT)), the process still ends. */
static void	returningHandler(int sig)
{
	ex_put(sig == LC_SIGABRT ? 10 : -1);
}

static void	caughtThenAbort(void *arg)
{
	(void)arg;
	if (lc_sysSigaction(LC_SIGABRT, returningHandler, 0) != 0)
		lc_exitNow(99);
	abort();
	ex_put(20);
}

TEST_OPT(handler_that_returns_does_not_stop_it, .ref = "C11 7.22.4.1p2")
{
	EX_LOG();
	EXPECT_EQ(lc_child(caughtThenAbort, NULL), LC_SIGNAL(LC_SIGABRT));
	EXPECT_GT(ex_log()->n, 0);
	EXPECT_EQ(ex_log()->v[0], 10);
	for (int i = 0; i < ex_log()->n; i++)
		EXPECT_NE(ex_log()->v[i], 20);
}

/* A handler that does not return (it ends the process itself): abort() does not terminate. */
static void	leavingHandler(int sig)
{
	ex_put(sig == LC_SIGABRT ? 11 : -1);
	lc_exitNow(5);
}

static void	caughtAndLeft(void *arg)
{
	(void)arg;
	if (lc_sysSigaction(LC_SIGABRT, leavingHandler, 0) != 0)
		lc_exitNow(99);
	abort();
}

TEST_OPT(handler_that_does_not_return_takes_over, .ref = "C11 7.22.4.1p2")
{
	static const int	want[] = {11};

	EX_LOG();
	EXPECT_EQ(lc_child(caughtAndLeft, NULL), LC_EXIT(5));
	EXPECT_TRUE(ex_order(want, 1));
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_TYPE(&(abort), void (*)(void));
}
