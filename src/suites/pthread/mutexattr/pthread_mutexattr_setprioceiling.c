/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutexattr_setprioceiling.c
 * @brief Tests for pthread_mutexattr_setprioceiling() (<pthread.h>).
 * @Created: 2026/11/28 18:30:00 by Moutig
 * @Updated: 2026/11/28 18:31:28 by Moutig
 *
 * pthread_mutexattr_setprioceiling(attr, prioceiling) sets the priority
 * ceiling attribute and returns 0 (TPP; RPP); the values are in the range
 * of the SCHED_FIFO priorities: both bounds are accepted, and a
 * PTHREAD_PRIO_PROTECT mutex is made with each. The case is skipped on
 * EPERM (may fail without privilege). Not tested: EINVAL (may fail), what
 * the ceiling does to priorities (needs privileges), EINTR (never
 * returned).
 *
 * Clauses: POSIX: XSH pthread_mutexattr_setprioceiling().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <sched.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutexattr_setprioceiling()")
{
	EXPECT_TYPE(&pthread_mutexattr_setprioceiling, int (*)(pthread_mutexattr_t *, int));
}

/* The SCHED_FIFO bounds: 0, and PTHREAD_PRIO_PROTECT mutexes are made. */
TEST_OPT(fifo_range_accepted, .ref = "XSH pthread_mutexattr_setprioceiling()")
{
	pthread_mutexattr_t	a;
	int					ceil[2];

	ceil[0] = sched_get_priority_min(SCHED_FIFO);
	ceil[1] = sched_get_priority_max(SCHED_FIFO);
	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	ASSERT_EQ(pthread_mutexattr_setprotocol(&a, PTHREAD_PRIO_PROTECT), 0);
	for (int i = 0; i < 2; i++)
	{
		pthread_mutex_t	m;
		int				e = pthread_mutexattr_setprioceiling(&a, ceil[i]);

		if (e == EPERM)
		{
			(void)pthread_mutexattr_destroy(&a);
			SKIP("no privilege for the priority ceiling (EPERM)");
		}
		EXPECT_EQ(e, 0);
		if (e == 0 && pthread_mutex_init(&m, &a) == 0)
			(void)pthread_mutex_destroy(&m);
		else
			EXPECT_TRUE(0);
	}
	(void)pthread_mutexattr_destroy(&a);
}
