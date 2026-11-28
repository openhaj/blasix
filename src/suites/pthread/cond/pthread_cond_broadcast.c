/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_cond_broadcast.c
 * @brief Tests for pthread_cond_broadcast() (<pthread.h>).
 * @Created: 2026/11/28 19:02:16 by Moutig
 * @Updated: 2026/11/28 19:03:13 by Moutig
 *
 * pthread_cond_broadcast(cond) unblocks all the threads blocked on the
 * condition variable and returns 0; each returns from its wait owning the
 * mutex (one after the other). With no thread blocked it has no effect.
 * The waiters use pthread_cond_timedwait() with a deadline (pt_cvWaiter):
 * all of them must return 0 long before it. Not tested: the order
 * (the scheduling policy), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_cond_broadcast().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_cond_broadcast()")
{
	EXPECT_TYPE(&pthread_cond_broadcast, int (*)(pthread_cond_t *));
}

# define PCB_N	4

/* Four waiters, one broadcast: all woken, each owning the mutex on return. */
TEST_OPT(unblocks_every_waiter, .ref = "XSH pthread_cond_broadcast()")
{
	pt_cv		v;
	pthread_t	t[PCB_N];
	int			made = 0;
	long long	t0;

	ASSERT_EQ(pt_cvInit(&v, 5 * LC_NS_PER_SEC), 0);
	while (made < PCB_N && pthread_create(&t[made], NULL, pt_cvWaiter, &v) == 0)
		made++;
	EXPECT_EQ(made, PCB_N);
	EXPECT_TRUE(pt_cvBlocked(&v, made));
	t0 = lc_now();
	EXPECT_EQ(pthread_cond_broadcast(&v.c), 0);
	for (int i = 0; i < made; i++)
		(void)pthread_join(t[i], NULL);
	EXPECT_EQ(pt_get(&v.woken), made);
	EXPECT_EQ(pt_get(&v.owned), made);
	EXPECT_LT(lc_now() - t0, 4 * LC_NS_PER_SEC);
	(void)pthread_cond_destroy(&v.c);
}

/* No thread blocked: 0 and no effect, a later waiter is not woken by it. */
TEST_OPT(no_waiter_no_effect, .ref = "XSH pthread_cond_broadcast()")
{
	pt_cv		v;
	pthread_t	t;

	ASSERT_EQ(pt_cvInit(&v, 200 * 1000000LL), 0);
	EXPECT_EQ(pthread_cond_broadcast(&v.c), 0);
	ASSERT_EQ(pthread_create(&t, NULL, pt_cvWaiter, &v), 0);
	(void)pthread_join(t, NULL);
	EXPECT_EQ(pt_get(&v.done), 1);
	EXPECT_EQ(pt_get(&v.woken), 0);
	(void)pthread_cond_destroy(&v.c);
}
