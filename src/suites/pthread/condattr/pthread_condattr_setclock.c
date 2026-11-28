/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_condattr_setclock.c
 * @brief Tests for pthread_condattr_setclock() (<pthread.h>).
 * @Created: 2026/11/28 19:00:29 by Moutig
 * @Updated: 2026/11/28 19:01:05 by Moutig
 *
 * pthread_condattr_setclock(attr, clock_id) sets the clock attribute and
 * returns 0 (CS in 2001, base from 2008): CLOCK_REALTIME and
 * CLOCK_MONOTONIC are accepted, and a condition variable made with
 * CLOCK_MONOTONIC measures its timedwait timeouts on it. A CPU-time clock
 * (CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID, when the header
 * has them) shall make the call fail. Not tested: an unknown clock
 * (EINVAL may fail), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_condattr_setclock().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <time.h>

TEST_OPT(prototype, .ref = "XSH pthread_condattr_setclock()")
{
	EXPECT_TYPE(&pthread_condattr_setclock, int (*)(pthread_condattr_t *, clockid_t));
}

/* CLOCK_REALTIME and CLOCK_MONOTONIC: 0. */
TEST_OPT(realtime_and_monotonic_accepted, .ref = "XSH pthread_condattr_setclock()")
{
	pthread_condattr_t	a;

	ASSERT_EQ(pthread_condattr_init(&a), 0);
	EXPECT_EQ(pthread_condattr_setclock(&a, CLOCK_REALTIME), 0);
	EXPECT_EQ(pthread_condattr_setclock(&a, CLOCK_MONOTONIC), 0);
	(void)pthread_condattr_destroy(&a);
}

/*
 * Wait on c (and m) until a CLOCK_MONOTONIC deadline 100 ms ahead.
 * Measured on CLOCK_REALTIME instead, that deadline (the time since boot)
 * is decades past: the wait would end at once.
 *
 * @return The elapsed monotonic time, or -1 when it did not end in ETIMEDOUT.
 */
static long long	monoWait(pthread_cond_t *c, pthread_mutex_t *m)
{
	struct timespec	at = pt_absMono(100 * 1000000LL);
	long long		t0 = lc_now();
	int				e;

	if (pthread_mutex_lock(m) != 0)
		return (-1);
	while ((e = pthread_cond_timedwait(c, m, &at)) == 0)
		;
	(void)pthread_mutex_unlock(m);
	return (e == ETIMEDOUT ? lc_now() - t0 : -1);
}

/* CLOCK_MONOTONIC: pthread_cond_timedwait() measures abstime on it. */
TEST_OPT(timedwait_uses_the_clock, .ref = "XSH pthread_condattr_setclock()")
{
	pthread_condattr_t	a;
	pthread_cond_t		c;
	pthread_mutex_t		m = PTHREAD_MUTEX_INITIALIZER;

	ASSERT_EQ(pthread_condattr_init(&a), 0);
	ASSERT_EQ(pthread_condattr_setclock(&a, CLOCK_MONOTONIC), 0);
	ASSERT_EQ(pthread_cond_init(&c, &a), 0);
	(void)pthread_condattr_destroy(&a);
	EXPECT_GE(monoWait(&c, &m), 100 * 1000000LL);
	(void)pthread_cond_destroy(&c);
}

/* A CPU-time clock: the call fails. */
TEST_OPT(cpu_time_clock_fails, .ref = "XSH pthread_condattr_setclock()")
{
	pthread_condattr_t	a;
	int					tried = 0;

	ASSERT_EQ(pthread_condattr_init(&a), 0);
#ifdef CLOCK_PROCESS_CPUTIME_ID
	EXPECT_NE(pthread_condattr_setclock(&a, CLOCK_PROCESS_CPUTIME_ID), 0);
	tried++;
#endif
#ifdef CLOCK_THREAD_CPUTIME_ID
	EXPECT_NE(pthread_condattr_setclock(&a, CLOCK_THREAD_CPUTIME_ID), 0);
	tried++;
#endif
	(void)pthread_condattr_destroy(&a);
	if (tried == 0)
		SKIP("no CPU-time clock in <time.h>");
}
