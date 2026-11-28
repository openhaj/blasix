/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_cond_clockwait.c
 * @brief Tests for pthread_cond_clockwait() (<pthread.h>).
 * @Created: 2026/11/28 19:02:23 by Moutig
 * @Updated: 2026/11/28 19:04:03 by Moutig
 *
 * pthread_cond_clockwait(cond, mutex, clock_id, abstime) is
 * pthread_cond_timedwait() with abstime measured on clock_id, whatever
 * the clock attribute of the condition variable (2024). CLOCK_REALTIME
 * and CLOCK_MONOTONIC are supported; the cases use CLOCK_MONOTONIC, and
 * CLOCK_REALTIME once. An invalid clock_id or a tv_nsec outside
 * [0, 1e9): EINVAL. Not tested: robust mutexes, cancellation, EINTR
 * (never returned).
 *
 * Clauses: POSIX: XSH pthread_cond_clockwait().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <time.h>

TEST_OPT(prototype, .ref = "XSH pthread_cond_clockwait()")
{
	EXPECT_TYPE(&pthread_cond_clockwait,
		int (*)(pthread_cond_t *, pthread_mutex_t *, clockid_t, const struct timespec *));
}

/* Wait with no signal until at: the result, spurious wakeups (0) skipped. */
static int	waitUntil(pthread_cond_t *c, pthread_mutex_t *m, const struct timespec *at)
{
	int	e;

	while ((e = pthread_cond_clockwait(c, m, CLOCK_MONOTONIC, at)) == 0)
		;
	return (e);
}

/* No signal: ETIMEDOUT, not before the clock reaches abstime, mutex owned again. */
TEST_OPT(times_out_at_abstime, .ref = "XSH pthread_cond_clockwait()")
{
	pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t	c = PTHREAD_COND_INITIALIZER;
	struct timespec	at = pt_absMono(100 * 1000000LL);

	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(waitUntil(&c, &m, &at), ETIMEDOUT);
	EXPECT_GE(lc_now(), pt_nsOf(&at));
	EXPECT_EQ(pt_tryOther(&m), EBUSY);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
}

/* abstime already past: ETIMEDOUT at once. */
TEST_OPT(past_abstime_times_out, .ref = "XSH pthread_cond_clockwait()")
{
	pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t	c = PTHREAD_COND_INITIALIZER;
	struct timespec	at = pt_absMono(-LC_NS_PER_SEC);
	long long		t0 = lc_now();

	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(waitUntil(&c, &m, &at), ETIMEDOUT);
	EXPECT_LT(lc_now() - t0, LC_NS_PER_SEC);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
}

typedef struct tw_state
{
	pthread_mutex_t	m;
	pthread_cond_t	c;
	int				waiting;
	int				go;
	int				ret;
}	tw_state;

static void	*twWaiter(void *arg)
{
	tw_state		*s = arg;
	struct timespec	at = pt_absMono(5 * LC_NS_PER_SEC);
	int				e = 0;

	if (pthread_mutex_lock(&s->m) != 0)
		return (NULL);
	pt_set(&s->waiting, 1);
	while (!pt_get(&s->go) && e == 0)
		e = pthread_cond_clockwait(&s->c, &s->m, CLOCK_MONOTONIC, &at);
	s->ret = e;
	(void)pthread_mutex_unlock(&s->m);
	return (NULL);
}

/* Signaled before abstime: 0, long before it. */
TEST_OPT(signaled_in_time, .ref = "XSH pthread_cond_clockwait()")
{
	static tw_state	s;
	pthread_t		t;
	long long		t0;

	lc_fill(&s, 0, sizeof(s));
	ASSERT_EQ(pthread_mutex_init(&s.m, NULL), 0);
	ASSERT_EQ(pthread_cond_init(&s.c, NULL), 0);
	ASSERT_EQ(pthread_create(&t, NULL, twWaiter, &s), 0);
	ASSERT_TRUE(pt_waitFor(&s.waiting, 1));
	t0 = lc_now();
	ASSERT_EQ(pthread_mutex_lock(&s.m), 0);
	pt_set(&s.go, 1);
	EXPECT_EQ(pthread_cond_signal(&s.c), 0);
	ASSERT_EQ(pthread_mutex_unlock(&s.m), 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	EXPECT_EQ(s.ret, 0);
	EXPECT_LT(lc_now() - t0, 4 * LC_NS_PER_SEC);
}

/* tv_nsec < 0 or >= 1000 million: EINVAL. */
TEST_OPT(bad_nanoseconds_einval, .ref = "XSH pthread_cond_clockwait()")
{
	pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t	c = PTHREAD_COND_INITIALIZER;
	struct timespec	at = pt_absMono(LC_NS_PER_SEC);

	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	at.tv_nsec = 1000000000L;
	EXPECT_EQ(pthread_cond_clockwait(&c, &m, CLOCK_MONOTONIC, &at), EINVAL);
	at.tv_nsec = -1;
	EXPECT_EQ(pthread_cond_clockwait(&c, &m, CLOCK_MONOTONIC, &at), EINVAL);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
}

/* An ERRORCHECK mutex the caller does not own: EPERM (2008). */
TEST_OPT(errorcheck_not_owner_eperm, .since = "POSIX.1-2008", .ref = "XSH pthread_cond_clockwait()")
{
	pthread_mutex_t	m;
	pthread_cond_t	c = PTHREAD_COND_INITIALIZER;
	struct timespec	at = pt_absMono(LC_NS_PER_SEC);

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_ERRORCHECK), 0);
	EXPECT_EQ(pthread_cond_clockwait(&c, &m, CLOCK_MONOTONIC, &at), EPERM);
	(void)pthread_mutex_destroy(&m);
}

/* An invalid clock_id: EINVAL. */
TEST_OPT(invalid_clock_einval, .ref = "XSH pthread_cond_clockwait()")
{
	pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t	c = PTHREAD_COND_INITIALIZER;
	struct timespec	at = pt_absMono(LC_NS_PER_SEC);

	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_cond_clockwait(&c, &m, (clockid_t)12345, &at), EINVAL);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
}

/* CLOCK_REALTIME is supported too. */
TEST_OPT(realtime_clock_supported, .ref = "XSH pthread_cond_clockwait()")
{
	pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t	c = PTHREAD_COND_INITIALIZER;
	struct timespec	at = pt_absReal(100 * 1000000LL);
	int				e;

	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	while ((e = pthread_cond_clockwait(&c, &m, CLOCK_REALTIME, &at)) == 0)
		;
	EXPECT_EQ(e, ETIMEDOUT);
	EXPECT_GE(lc_nowReal(), pt_nsOf(&at));
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
}

/* The clock attribute of the condition has no effect: clock_id is used. */
TEST_OPT(clock_attribute_ignored, .ref = "XSH pthread_condattr_getclock()")
{
	pthread_condattr_t	a;
	pthread_cond_t		c;
	pthread_mutex_t		m = PTHREAD_MUTEX_INITIALIZER;
	struct timespec		at;

	ASSERT_EQ(pthread_condattr_init(&a), 0);
	ASSERT_EQ(pthread_condattr_setclock(&a, CLOCK_REALTIME), 0);
	ASSERT_EQ(pthread_cond_init(&c, &a), 0);
	(void)pthread_condattr_destroy(&a);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	at = pt_absMono(100 * 1000000LL);
	EXPECT_EQ(waitUntil(&c, &m, &at), ETIMEDOUT);
	EXPECT_GE(lc_now(), pt_nsOf(&at));
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_cond_destroy(&c);
}
