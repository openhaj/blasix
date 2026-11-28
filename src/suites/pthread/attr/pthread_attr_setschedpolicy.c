/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_setschedpolicy.c
 * @brief Tests for pthread_attr_setschedpolicy() (<pthread.h>).
 * @Created: 2026/11/28 18:08:09 by Moutig
 * @Updated: 2026/11/28 18:09:19 by Moutig
 *
 * pthread_attr_setschedpolicy(attr, policy) sets the schedpolicy attribute
 * and returns 0. The supported values include SCHED_FIFO, SCHED_RR and
 * SCHED_OTHER, so none of them is ENOTSUP. What a thread gets from it is
 * in pthread_attr_setinheritsched.c. Not tested: an invalid policy
 * (EINVAL may fail), the mutex priority order of the realtime policies
 * (the mutex units), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_setschedpolicy().
 */
#include "pthread/pthread_cases.h"

#include <sched.h>

TEST_OPT(prototype, .ref = "XSH pthread_attr_setschedpolicy()")
{
	EXPECT_TYPE(&pthread_attr_setschedpolicy, int (*)(pthread_attr_t *, int));
}

/* SCHED_FIFO, SCHED_RR, SCHED_OTHER are supported: 0 for each. */
TEST_OPT(required_policies_supported, .ref = "XSH pthread_attr_setschedpolicy()")
{
	pthread_attr_t	a;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_setschedpolicy(&a, SCHED_FIFO), 0);
	EXPECT_EQ(pthread_attr_setschedpolicy(&a, SCHED_RR), 0);
	EXPECT_EQ(pthread_attr_setschedpolicy(&a, SCHED_OTHER), 0);
	(void)pthread_attr_destroy(&a);
}

/* SCHED_OTHER with PTHREAD_INHERIT_SCHED: threads are created. */
TEST_OPT(threads_are_created_with_it, .ref = "XSH pthread_attr_setschedpolicy()")
{
	pthread_attr_t	a;
	pt_run			r;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_setinheritsched(&a, PTHREAD_INHERIT_SCHED), 0);
	ASSERT_EQ(pthread_attr_setschedpolicy(&a, SCHED_OTHER), 0);
	EXPECT_EQ(pt_runWith(&a, 1, &r), 0);
	(void)pthread_attr_destroy(&a);
}
