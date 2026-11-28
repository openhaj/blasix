/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_setschedprio.c
 * @brief Tests for pthread_setschedprio() (<pthread.h>).
 * @Created: 2026/11/28 18:15:45 by Moutig
 * @Updated: 2026/11/28 18:16:34 by Moutig
 *
 * pthread_setschedprio(thread, prio) sets the scheduling priority of
 * thread and returns 0 (TPS); if it fails, the priority does not change.
 * Every case changes a thread made for it; the SCHED_FIFO case is
 * skipped on EPERM (no privilege). Not tested: the place in the thread
 * list (XSH 2.8.4), EINVAL and EPERM (may fail), ESRCH (a dead thread ID
 * is undefined to use), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_setschedprio().
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

TEST_OPT(prototype, .ref = "XSH pthread_setschedprio()")
{
	EXPECT_TYPE(&pthread_setschedprio, int (*)(pthread_t, int));
}

/* -1 no privilege, 0 as expected, 1 not. */
static int	gPspResult;

static void	*pspSameAgain(void *arg)
{
	struct sched_param	p;
	int					pol = -1;

	(void)arg;
	lc_fill(&p, 0, sizeof(p));
	if (pthread_getschedparam(pthread_self(), &pol, &p) != 0)
		return (gPspResult = 1, NULL);
	gPspResult = pthread_setschedprio(pthread_self(), p.sched_priority) != 0;
	return (NULL);
}

/* The thread's own priority, set again: 0. */
TEST_OPT(current_priority_accepted, .ref = "XSH pthread_setschedprio()")
{
	gPspResult = 1;
	ASSERT_EQ(inThread(pspSameAgain, NULL), 0);
	EXPECT_EQ(gPspResult, 0);
}

static void	*pspFifo(void *arg)
{
	struct sched_param	p;
	int					pol = -1;
	int					lo = sched_get_priority_min(SCHED_FIFO);
	int					hi = sched_get_priority_max(SCHED_FIFO);

	(void)arg;
	lc_fill(&p, 0, sizeof(p));
	p.sched_priority = lo;
	if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &p) == EPERM
		|| pthread_setschedprio(pthread_self(), hi) == EPERM)
		return (gPspResult = -1, NULL);
	lc_fill(&p, 0, sizeof(p));
	gPspResult = pthread_getschedparam(pthread_self(), &pol, &p) != 0
		|| pol != SCHED_FIFO || p.sched_priority != hi;
	return (NULL);
}

/* SCHED_FIFO: the priority becomes prio, the policy stays. */
TEST_OPT(sets_the_priority, .ref = "XSH pthread_setschedprio()")
{
	gPspResult = 1;
	ASSERT_EQ(inThread(pspFifo, NULL), 0);
	if (gPspResult == -1)
		SKIP("no privilege for SCHED_FIFO (EPERM)");
	EXPECT_EQ(gPspResult, 0);
}

static void	*pspFailure(void *arg)
{
	struct sched_param	before;
	struct sched_param	after;
	int					pol = -1;
	int					bad;

	(void)arg;
	lc_fill(&before, 0, sizeof(before));
	lc_fill(&after, 0, sizeof(after));
	if (pthread_getschedparam(pthread_self(), &pol, &before) != 0)
		return (gPspResult = 1, NULL);
	/* Above the highest priority of the thread's policy. */
	bad = sched_get_priority_max(pol) + 1;
	if (pthread_setschedprio(pthread_self(), bad) == 0)
		return (gPspResult = -1, NULL);
	gPspResult = pthread_getschedparam(pthread_self(), &pol, &after) != 0
		|| after.sched_priority != before.sched_priority;
	return (NULL);
}

/* A call that fails leaves the priority as it was. */
TEST_OPT(failure_changes_nothing, .ref = "XSH pthread_setschedprio()")
{
	gPspResult = 1;
	ASSERT_EQ(inThread(pspFailure, NULL), 0);
	if (gPspResult == -1)
		SKIP("an out-of-range priority was accepted");
	EXPECT_EQ(gPspResult, 0);
}
