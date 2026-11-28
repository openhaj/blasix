/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutexattr_getprioceiling.c
 * @brief Tests for pthread_mutexattr_getprioceiling() (<pthread.h>).
 * @Created: 2026/11/28 18:29:13 by Moutig
 * @Updated: 2026/11/28 18:30:52 by Moutig
 *
 * pthread_mutexattr_getprioceiling(attr, prioceiling) stores the priority
 * ceiling attribute and returns 0 (TPP; RPP): the value last set, here
 * the lowest and the highest SCHED_FIFO priorities. Not tested: EINVAL
 * and EPERM (may fail), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutexattr_getprioceiling().
 */
#include "pthread/pthread_cases.h"

#include <sched.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutexattr_getprioceiling()")
{
	EXPECT_TYPE(&pthread_mutexattr_getprioceiling,
		int (*)(const pthread_mutexattr_t *, int *));
}

/* It gives the ceiling last set: the SCHED_FIFO bounds. */
TEST_OPT(gives_the_ceiling_set, .ref = "XSH pthread_mutexattr_getprioceiling()")
{
	pthread_mutexattr_t	a;
	int					want[2];
	int					bad = 0;

	want[0] = sched_get_priority_min(SCHED_FIFO);
	want[1] = sched_get_priority_max(SCHED_FIFO);
	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	for (int i = 0; i < 2; i++)
	{
		int	v = -12345;

		bad += pthread_mutexattr_setprioceiling(&a, want[i]) != 0
			|| pthread_mutexattr_getprioceiling(&a, &v) != 0 || v != want[i];
	}
	EXPECT_EQ(bad, 0);
	(void)pthread_mutexattr_destroy(&a);
}
