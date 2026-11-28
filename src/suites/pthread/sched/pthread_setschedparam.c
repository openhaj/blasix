/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_setschedparam.c
 * @brief Tests for pthread_setschedparam() (<pthread.h>).
 * @Created: 2026/11/28 18:15:36 by Moutig
 * @Updated: 2026/11/28 18:16:24 by Moutig
 *
 * pthread_setschedparam(thread, policy, param) sets the scheduling policy
 * and parameters of thread and returns 0 (TPS); policy may be SCHED_OTHER,
 * SCHED_FIFO or SCHED_RR. If it fails, the parameters of the thread do
 * not change. Every case changes a thread made for it, never the thread
 * that runs the tests; the SCHED_FIFO and SCHED_RR cases are skipped on
 * EPERM (no privilege). Not tested: ENOTSUP (no value is known to be
 * unsupported), EINVAL (may fail), SCHED_SPORADIC (TSP), ESRCH (a dead
 * thread ID is undefined to use), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_setschedparam().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <sched.h>

/* Run fn(arg) in a new thread (default attributes) and join it. */
static int	inThread(void *(*fn)(void *), void *arg)
{
	pt_run	r;

	lc_fill(&r, 0, sizeof(r));
	r.fn = fn;
	r.arg = arg;
	return (pt_runWith(NULL, 1, &r));
}

TEST_OPT(prototype, .ref = "XSH pthread_setschedparam()")
{
	EXPECT_TYPE(&pthread_setschedparam,
		int (*)(pthread_t, int, const struct sched_param *));
}

/* -1 no privilege, 0 as expected, 1 not. */
static int	gPssResult;

static void	*pssSameAgain(void *arg)
{
	struct sched_param	p;
	struct sched_param	q;
	int					pol = -1;
	int					pol2 = -2;

	(void)arg;
	lc_fill(&p, 0, sizeof(p));
	lc_fill(&q, 0, sizeof(q));
	gPssResult = pthread_getschedparam(pthread_self(), &pol, &p) != 0
		|| pthread_setschedparam(pthread_self(), pol, &p) != 0
		|| pthread_getschedparam(pthread_self(), &pol2, &q) != 0
		|| pol2 != pol || q.sched_priority != p.sched_priority;
	return (NULL);
}

/* The thread's own policy and parameters, set again: 0, unchanged. */
TEST_OPT(current_values_accepted, .ref = "XSH pthread_setschedparam()")
{
	gPssResult = 1;
	ASSERT_EQ(inThread(pssSameAgain, NULL), 0);
	EXPECT_EQ(gPssResult, 0);
}

static void	*pssRealtime(void *arg)
{
	int					pol = *(int *)arg;
	struct sched_param	p;
	int					got = -1;
	int					want = sched_get_priority_max(pol);

	lc_fill(&p, 0, sizeof(p));
	p.sched_priority = want;
	if (pthread_setschedparam(pthread_self(), pol, &p) == EPERM)
		return (gPssResult = -1, NULL);
	lc_fill(&p, 0, sizeof(p));
	gPssResult = pthread_getschedparam(pthread_self(), &got, &p) != 0
		|| got != pol || p.sched_priority != want;
	return (NULL);
}

/* SCHED_FIFO and SCHED_RR at their highest priority: set as given. */
TEST_OPT(sets_a_realtime_policy, .ref = "XSH pthread_setschedparam()")
{
	int	pol[2] = {SCHED_FIFO, SCHED_RR};

	for (int i = 0; i < 2; i++)
	{
		gPssResult = 1;
		ASSERT_EQ(inThread(pssRealtime, &pol[i]), 0);
		if (gPssResult == -1)
			SKIP("no privilege for SCHED_FIFO / SCHED_RR (EPERM)");
		EXPECT_EQ(gPssResult, 0);
	}
}

static void	*pssFailure(void *arg)
{
	struct sched_param	before;
	struct sched_param	p;
	struct sched_param	after;
	int					pol = -1;
	int					pol2 = -2;

	(void)arg;
	lc_fill(&before, 0, sizeof(before));
	lc_fill(&after, 0, sizeof(after));
	lc_fill(&p, 0, sizeof(p));
	if (pthread_getschedparam(pthread_self(), &pol, &before) != 0)
		return (gPssResult = 1, NULL);
	/* A priority above the highest: invalid, or EPERM without privilege. */
	p.sched_priority = sched_get_priority_max(SCHED_FIFO) + 1;
	if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &p) == 0)
		return (gPssResult = -1, NULL);
	gPssResult = pthread_getschedparam(pthread_self(), &pol2, &after) != 0
		|| pol2 != pol || after.sched_priority != before.sched_priority;
	return (NULL);
}

/* A call that fails leaves the policy and priority of the thread as they were. */
TEST_OPT(failure_changes_nothing, .ref = "XSH pthread_setschedparam()")
{
	gPssResult = 1;
	ASSERT_EQ(inThread(pssFailure, NULL), 0);
	if (gPssResult == -1)
		SKIP("an out-of-range SCHED_FIFO priority was accepted");
	EXPECT_EQ(gPssResult, 0);
}
