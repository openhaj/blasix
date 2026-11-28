/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutex_timedlock.c
 * @brief Tests for pthread_mutex_timedlock() (<pthread.h>).
 * @Created: 2026/11/28 18:38:13 by Moutig
 * @Updated: 2026/11/28 18:38:21 by Moutig
 *
 * pthread_mutex_timedlock(mutex, abstime) locks the mutex as
 * pthread_mutex_lock() does, but stops waiting when abstime passes on
 * CLOCK_REALTIME: ETIMEDOUT, not sooner. A mutex that can be locked at
 * once is locked (0), even with an abstime in the past. Released before
 * the timeout: 0. If it would block, a tv_nsec outside [0, 1e9) is
 * EINVAL. ERRORCHECK relock: EDEADLK (2008; may fail in 2001). TMO in
 * 2001, base from 2008. The deadlines come from the raw clocks
 * (lc_nowReal, lc_now). Not tested: robust and PRIO_PROTECT mutexes,
 * priority inheritance, EAGAIN, EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutex_timedlock().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <time.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutex_timedlock()")
{
	EXPECT_TYPE(&pthread_mutex_timedlock,
		int (*)(pthread_mutex_t *, const struct timespec *));
}

/* A thread that owns the mutex for 50 ms, then unlocks it. */
typedef struct tl_brief
{
	pthread_mutex_t	*m;
	int				held;
}	tl_brief;

static void	*tlBrief(void *arg)
{
	tl_brief	*b = arg;

	if (pthread_mutex_lock(b->m) != 0)
		return (NULL);
	pt_set(&b->held, 1);
	lc_sleep(50 * 1000000LL);
	(void)pthread_mutex_unlock(b->m);
	return (NULL);
}

/* Lockable at once: 0, even when abstime has passed. */
TEST_OPT(free_mutex_locked_even_past_abstime, .ref = "XSH pthread_mutex_timedlock()")
{
	pthread_mutex_t	m;
	struct timespec	past = pt_absReal(-LC_NS_PER_SEC);

	ASSERT_EQ(pthread_mutex_init(&m, NULL), 0);
	EXPECT_EQ(pthread_mutex_timedlock(&m, &past), 0);
	EXPECT_EQ(pt_tryOther(&m), EBUSY);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* Owned by another thread: ETIMEDOUT, not before the clock reaches abstime. */
TEST_OPT(times_out_at_abstime, .ref = "XSH pthread_mutex_timedlock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pt_hold					h;
	struct timespec			at;

	ASSERT_EQ(pt_holdStart(&h, &m), 0);
	at = pt_absReal(100 * 1000000LL);
	EXPECT_EQ(pthread_mutex_timedlock(&m, &at), ETIMEDOUT);
	EXPECT_GE(lc_nowReal(), (long long)at.tv_sec * LC_NS_PER_SEC + at.tv_nsec);
	pt_holdEnd(&h);
}

/* Owned by another thread, abstime already past: ETIMEDOUT at once. */
TEST_OPT(past_abstime_times_out, .ref = "XSH pthread_mutex_timedlock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pt_hold					h;
	struct timespec			past = pt_absReal(-LC_NS_PER_SEC);
	long long				t0;

	ASSERT_EQ(pt_holdStart(&h, &m), 0);
	t0 = lc_now();
	EXPECT_EQ(pthread_mutex_timedlock(&m, &past), ETIMEDOUT);
	EXPECT_LT(lc_now() - t0, LC_NS_PER_SEC);
	pt_holdEnd(&h);
}

/* Released before the timeout: 0, long before abstime. */
TEST_OPT(acquires_when_released_in_time, .ref = "XSH pthread_mutex_timedlock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	static tl_brief			b;
	pthread_t				t;
	struct timespec			at;
	long long				t0;

	b.m = &m;
	pt_set(&b.held, 0);
	ASSERT_EQ(pthread_create(&t, NULL, tlBrief, &b), 0);
	ASSERT_TRUE(pt_waitFor(&b.held, 1));
	at = pt_absReal(5 * LC_NS_PER_SEC);
	t0 = lc_now();
	EXPECT_EQ(pthread_mutex_timedlock(&m, &at), 0);
	EXPECT_LT(lc_now() - t0, 4 * LC_NS_PER_SEC);
	(void)pthread_mutex_unlock(&m);
	ASSERT_EQ(pthread_join(t, NULL), 0);
}

/* Would block, tv_nsec < 0 or >= 1000 million: EINVAL. */
TEST_OPT(bad_nanoseconds_einval, .ref = "XSH pthread_mutex_timedlock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pt_hold					h;
	struct timespec			at = pt_absReal(LC_NS_PER_SEC);

	ASSERT_EQ(pt_holdStart(&h, &m), 0);
	at.tv_nsec = 1000000000L;
	EXPECT_EQ(pthread_mutex_timedlock(&m, &at), EINVAL);
	at.tv_nsec = -1;
	EXPECT_EQ(pthread_mutex_timedlock(&m, &at), EINVAL);
	pt_holdEnd(&h);
}

/* ERRORCHECK, relocked by its owner: EDEADLK. */
TEST_OPT(errorcheck_relock_edeadlk, .since = "POSIX.1-2008", .ref = "XSH pthread_mutex_timedlock()")
{
	pthread_mutex_t	m;
	struct timespec	at = pt_absReal(LC_NS_PER_SEC);

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_ERRORCHECK), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_timedlock(&m, &at), EDEADLK);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}
