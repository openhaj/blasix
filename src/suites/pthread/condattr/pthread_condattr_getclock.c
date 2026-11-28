/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_condattr_getclock.c
 * @brief Tests for pthread_condattr_getclock() (<pthread.h>).
 * @Created: 2026/11/28 19:00:07 by Moutig
 * @Updated: 2026/11/28 19:01:17 by Moutig
 *
 * pthread_condattr_getclock(attr, clock_id) stores the clock attribute,
 * the clock of the pthread_cond_timedwait() timeouts, and returns 0 (CS in
 * 2001, base from 2008). The default is the system clock,
 * CLOCK_REALTIME; otherwise the clock last set. Not tested: an object
 * that is not initialized (undefined), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_condattr_getclock().
 */
#include "pthread/pthread_cases.h"

#include <time.h>

TEST_OPT(prototype, .ref = "XSH pthread_condattr_getclock()")
{
	EXPECT_TYPE(&pthread_condattr_getclock,
		int (*)(const pthread_condattr_t *, clockid_t *));
}

/* The default is the system clock, CLOCK_REALTIME. */
TEST_OPT(default_is_the_system_clock, .ref = "XSH pthread_condattr_getclock()")
{
	pthread_condattr_t	a;
	clockid_t			v = (clockid_t)-12345;

	ASSERT_EQ(pthread_condattr_init(&a), 0);
	EXPECT_EQ(pthread_condattr_getclock(&a, &v), 0);
	EXPECT_EQ((long)v, (long)CLOCK_REALTIME);
	(void)pthread_condattr_destroy(&a);
}

/* It gives the clock last set: CLOCK_MONOTONIC, then CLOCK_REALTIME. */
TEST_OPT(gives_the_clock_set, .ref = "XSH pthread_condattr_getclock()")
{
	pthread_condattr_t	a;
	clockid_t			v = (clockid_t)-12345;

	ASSERT_EQ(pthread_condattr_init(&a), 0);
	ASSERT_EQ(pthread_condattr_setclock(&a, CLOCK_MONOTONIC), 0);
	EXPECT_EQ(pthread_condattr_getclock(&a, &v), 0);
	EXPECT_EQ((long)v, (long)CLOCK_MONOTONIC);
	ASSERT_EQ(pthread_condattr_setclock(&a, CLOCK_REALTIME), 0);
	EXPECT_EQ(pthread_condattr_getclock(&a, &v), 0);
	EXPECT_EQ((long)v, (long)CLOCK_REALTIME);
	(void)pthread_condattr_destroy(&a);
}
