/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_condattr_destroy.c
 * @brief Tests for pthread_condattr_destroy() (<pthread.h>).
 * @Created: 2026/11/28 19:00:00 by Moutig
 * @Updated: 2026/11/28 19:00:46 by Moutig
 *
 * pthread_condattr_destroy(attr) destroys a condition variable attributes
 * object and returns 0; it can be initialized again. Changing or
 * destroying the object does not affect a condition variable already made
 * with it: one made with CLOCK_MONOTONIC keeps measuring its timeouts on
 * it. Not tested: any other use of a destroyed object (undefined), EINTR
 * (never returned).
 *
 * Clauses: POSIX: XSH pthread_condattr_destroy().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <time.h>

TEST_OPT(prototype, .ref = "XSH pthread_condattr_destroy()")
{
	EXPECT_TYPE(&pthread_condattr_destroy, int (*)(pthread_condattr_t *));
}

/* 0, and the object can be initialized again. */
TEST_OPT(returns_zero_and_can_be_reinitialized, .ref = "XSH pthread_condattr_destroy()")
{
	pthread_condattr_t	a;

	ASSERT_EQ(pthread_condattr_init(&a), 0);
	EXPECT_EQ(pthread_condattr_destroy(&a), 0);
	EXPECT_EQ(pthread_condattr_init(&a), 0);
	EXPECT_EQ(pthread_condattr_destroy(&a), 0);
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

/* Clock changed then object destroyed: the condition keeps CLOCK_MONOTONIC. */
TEST_OPT(conds_already_initialized_unaffected, .since = "POSIX.1-2008",
	.ref = "XSH pthread_condattr_destroy()")
{
	pthread_condattr_t	a;
	pthread_cond_t		c;
	pthread_mutex_t		m = PTHREAD_MUTEX_INITIALIZER;

	ASSERT_EQ(pthread_condattr_init(&a), 0);
	ASSERT_EQ(pthread_condattr_setclock(&a, CLOCK_MONOTONIC), 0);
	ASSERT_EQ(pthread_cond_init(&c, &a), 0);
	EXPECT_EQ(pthread_condattr_setclock(&a, CLOCK_REALTIME), 0);
	EXPECT_EQ(pthread_condattr_destroy(&a), 0);
	EXPECT_GE(monoWait(&c, &m), 100 * 1000000LL);
	(void)pthread_cond_destroy(&c);
}
