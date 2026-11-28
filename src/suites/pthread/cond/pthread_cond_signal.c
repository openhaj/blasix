/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_cond_signal.c
 * @brief Tests for pthread_cond_signal() (<pthread.h>).
 * @Created: 2026/11/28 19:02:45 by Moutig
 * @Updated: 2026/11/28 19:03:42 by Moutig
 *
 * pthread_cond_signal(cond) unblocks at least one of the threads blocked
 * on the condition variable and returns 0; each unblocked thread returns
 * from its wait owning the mutex. It may be called without owning the
 * mutex. With no thread blocked it has no effect: a later wait still
 * times out. The waiters use pthread_cond_timedwait() with a deadline
 * (pt_cvWaiter), so a lost signal fails the case instead of hanging it.
 * Not tested: which thread is unblocked (the scheduling policy), EINTR
 * (never returned).
 *
 * Clauses: POSIX: XSH pthread_cond_signal().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_cond_signal()")
{
	EXPECT_TYPE(&pthread_cond_signal, int (*)(pthread_cond_t *));
}

# define PCS_N	3

/* Three waiters, one signal: at least one is woken, owning the mutex. */
TEST_OPT(unblocks_at_least_one, .ref = "XSH pthread_cond_signal()", .timeout = 30)
{
	pt_cv		v;
	pthread_t	t[PCS_N];
	int			made = 0;

	ASSERT_EQ(pt_cvInit(&v, 2 * LC_NS_PER_SEC), 0);
	while (made < PCS_N && pthread_create(&t[made], NULL, pt_cvWaiter, &v) == 0)
		made++;
	EXPECT_TRUE(pt_cvBlocked(&v, made));
	EXPECT_EQ(pthread_cond_signal(&v.c), 0);
	for (int i = 0; i < made; i++)
		(void)pthread_join(t[i], NULL);
	EXPECT_GE(pt_get(&v.woken), 1);
	EXPECT_EQ(pt_get(&v.owned), pt_get(&v.woken));
	(void)pthread_cond_destroy(&v.c);
}

/* Called without owning the mutex: the waiter is still woken. */
TEST_OPT(caller_need_not_own_the_mutex, .ref = "XSH pthread_cond_signal()")
{
	pt_cv		v;
	pthread_t	t;
	long long	t0;

	ASSERT_EQ(pt_cvInit(&v, 5 * LC_NS_PER_SEC), 0);
	ASSERT_EQ(pthread_create(&t, NULL, pt_cvWaiter, &v), 0);
	EXPECT_TRUE(pt_cvBlocked(&v, 1));
	t0 = lc_now();
	EXPECT_EQ(pthread_cond_signal(&v.c), 0);
	(void)pthread_join(t, NULL);
	EXPECT_EQ(pt_get(&v.woken), 1);
	EXPECT_LT(lc_now() - t0, 4 * LC_NS_PER_SEC);
	(void)pthread_cond_destroy(&v.c);
}

/* No thread blocked: 0 and no effect, a later waiter is not woken by it. */
TEST_OPT(no_waiter_no_effect, .ref = "XSH pthread_cond_signal()")
{
	pt_cv		v;
	pthread_t	t;

	ASSERT_EQ(pt_cvInit(&v, 200 * 1000000LL), 0);
	EXPECT_EQ(pthread_cond_signal(&v.c), 0);
	ASSERT_EQ(pthread_create(&t, NULL, pt_cvWaiter, &v), 0);
	(void)pthread_join(t, NULL);
	EXPECT_EQ(pt_get(&v.done), 1);
	EXPECT_EQ(pt_get(&v.woken), 0);
	(void)pthread_cond_destroy(&v.c);
}
