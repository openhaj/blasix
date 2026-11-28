/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutex_clocklock.c
 * @brief Tests for pthread_mutex_clocklock() (<pthread.h>).
 * @Created: 2026/11/28 18:37:52 by Moutig
 * @Updated: 2026/11/28 18:38:08 by Moutig
 *
 * pthread_mutex_clocklock(mutex, clock_id, abstime) is
 * pthread_mutex_timedlock() with the timeout measured on clock_id (2024);
 * CLOCK_REALTIME and CLOCK_MONOTONIC are supported. The cases use
 * CLOCK_MONOTONIC, and CLOCK_REALTIME once. If it would block, an
 * invalid clock_id or a tv_nsec outside [0, 1e9) is EINVAL. Not tested:
 * robust and PRIO_PROTECT mutexes, priority inheritance, EAGAIN, EINTR
 * (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutex_clocklock().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <time.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutex_clocklock()")
{
	EXPECT_TYPE(&pthread_mutex_clocklock,
		int (*)(pthread_mutex_t *, clockid_t, const struct timespec *));
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
TEST_OPT(free_mutex_locked_even_past_abstime, .ref = "XSH pthread_mutex_clocklock()")
{
	pthread_mutex_t	m;
	struct timespec	past = pt_absMono(-LC_NS_PER_SEC);

	ASSERT_EQ(pthread_mutex_init(&m, NULL), 0);
	EXPECT_EQ(pthread_mutex_clocklock(&m, CLOCK_MONOTONIC, &past), 0);
	EXPECT_EQ(pt_tryOther(&m), EBUSY);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* Owned by another thread: ETIMEDOUT, not before the clock reaches abstime. */
TEST_OPT(times_out_at_abstime, .ref = "XSH pthread_mutex_clocklock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pt_hold					h;
	struct timespec			at;

	ASSERT_EQ(pt_holdStart(&h, &m), 0);
	at = pt_absMono(100 * 1000000LL);
	EXPECT_EQ(pthread_mutex_clocklock(&m, CLOCK_MONOTONIC, &at), ETIMEDOUT);
	EXPECT_GE(lc_now(), (long long)at.tv_sec * LC_NS_PER_SEC + at.tv_nsec);
	pt_holdEnd(&h);
}

/* Owned by another thread, abstime already past: ETIMEDOUT at once. */
TEST_OPT(past_abstime_times_out, .ref = "XSH pthread_mutex_clocklock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pt_hold					h;
	struct timespec			past = pt_absMono(-LC_NS_PER_SEC);
	long long				t0;

	ASSERT_EQ(pt_holdStart(&h, &m), 0);
	t0 = lc_now();
	EXPECT_EQ(pthread_mutex_clocklock(&m, CLOCK_MONOTONIC, &past), ETIMEDOUT);
	EXPECT_LT(lc_now() - t0, LC_NS_PER_SEC);
	pt_holdEnd(&h);
}

/* Released before the timeout: 0, long before abstime. */
TEST_OPT(acquires_when_released_in_time, .ref = "XSH pthread_mutex_clocklock()")
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
	at = pt_absMono(5 * LC_NS_PER_SEC);
	t0 = lc_now();
	EXPECT_EQ(pthread_mutex_clocklock(&m, CLOCK_MONOTONIC, &at), 0);
	EXPECT_LT(lc_now() - t0, 4 * LC_NS_PER_SEC);
	(void)pthread_mutex_unlock(&m);
	ASSERT_EQ(pthread_join(t, NULL), 0);
}

/* Would block, tv_nsec < 0 or >= 1000 million: EINVAL. */
TEST_OPT(bad_nanoseconds_einval, .ref = "XSH pthread_mutex_clocklock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pt_hold					h;
	struct timespec			at = pt_absMono(LC_NS_PER_SEC);

	ASSERT_EQ(pt_holdStart(&h, &m), 0);
	at.tv_nsec = 1000000000L;
	EXPECT_EQ(pthread_mutex_clocklock(&m, CLOCK_MONOTONIC, &at), EINVAL);
	at.tv_nsec = -1;
	EXPECT_EQ(pthread_mutex_clocklock(&m, CLOCK_MONOTONIC, &at), EINVAL);
	pt_holdEnd(&h);
}

/* ERRORCHECK, relocked by its owner: EDEADLK. */
TEST_OPT(errorcheck_relock_edeadlk, .ref = "XSH pthread_mutex_clocklock()")
{
	pthread_mutex_t	m;
	struct timespec	at = pt_absMono(LC_NS_PER_SEC);

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_ERRORCHECK), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_clocklock(&m, CLOCK_MONOTONIC, &at), EDEADLK);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* An invalid clock_id, when it would block: EINVAL. */
TEST_OPT(invalid_clock_einval, .ref = "XSH pthread_mutex_clocklock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pt_hold					h;
	struct timespec			at = pt_absMono(LC_NS_PER_SEC);

	ASSERT_EQ(pt_holdStart(&h, &m), 0);
	EXPECT_EQ(pthread_mutex_clocklock(&m, (clockid_t)12345, &at), EINVAL);
	pt_holdEnd(&h);
}

/* CLOCK_REALTIME is supported too: ETIMEDOUT once it reaches abstime. */
TEST_OPT(realtime_clock_supported, .ref = "XSH pthread_mutex_clocklock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pt_hold					h;
	struct timespec			at;

	ASSERT_EQ(pt_holdStart(&h, &m), 0);
	at = pt_absReal(100 * 1000000LL);
	EXPECT_EQ(pthread_mutex_clocklock(&m, CLOCK_REALTIME, &at), ETIMEDOUT);
	EXPECT_GE(lc_nowReal(), (long long)at.tv_sec * LC_NS_PER_SEC + at.tv_nsec);
	pt_holdEnd(&h);
}
