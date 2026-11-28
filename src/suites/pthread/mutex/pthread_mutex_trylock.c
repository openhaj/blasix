/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutex_trylock.c
 * @brief Tests for pthread_mutex_trylock() (<pthread.h>).
 * @Created: 2026/11/28 18:35:58 by Moutig
 * @Updated: 2026/11/28 18:36:53 by Moutig
 *
 * pthread_mutex_trylock(mutex) is pthread_mutex_lock() except that it
 * returns at once when the mutex is locked, by any thread, the caller
 * included: EBUSY. Unlocked: 0, the caller owning it. RECURSIVE and
 * owned by the caller: 0, and the count goes up. Not tested: EAGAIN
 * (recursion limit unknown), robust and PRIO_PROTECT mutexes (their
 * units), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutex_trylock().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutex_trylock()")
{
	EXPECT_TYPE(&pthread_mutex_trylock, int (*)(pthread_mutex_t *));
}

/* Unlocked: 0, and the caller owns it. */
TEST_OPT(locks_an_unlocked_mutex, .ref = "XSH pthread_mutex_trylock()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pthread_mutex_init(&m, NULL), 0);
	EXPECT_EQ(pthread_mutex_trylock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), EBUSY);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* Owned by another thread: EBUSY at once. */
TEST_OPT(locked_by_another_thread_ebusy, .ref = "XSH pthread_mutex_trylock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	pt_hold					h;
	long long				t0;

	ASSERT_EQ(pt_holdStart(&h, &m), 0);
	t0 = lc_now();
	EXPECT_EQ(pthread_mutex_trylock(&m), EBUSY);
	EXPECT_LT(lc_now() - t0, LC_NS_PER_SEC);
	pt_holdEnd(&h);
}

/* Owned by the caller, NORMAL, ERRORCHECK or DEFAULT: EBUSY. */
TEST_OPT(locked_by_the_caller_ebusy, .ref = "XSH pthread_mutex_trylock()")
{
	const int		type[3] = {PTHREAD_MUTEX_NORMAL, PTHREAD_MUTEX_ERRORCHECK,
		PTHREAD_MUTEX_DEFAULT};
	int				bad = 0;

	for (int i = 0; i < 3; i++)
	{
		pthread_mutex_t	m;

		if (pt_mutexOfType(&m, type[i]) != 0 || pthread_mutex_lock(&m) != 0)
		{
			bad++;
			continue ;
		}
		bad += pthread_mutex_trylock(&m) != EBUSY;
		(void)pthread_mutex_unlock(&m);
		(void)pthread_mutex_destroy(&m);
	}
	EXPECT_EQ(bad, 0);
}

/* RECURSIVE, owned by the caller: 0, one more unlock needed. */
TEST_OPT(recursive_owner_counts, .ref = "XSH pthread_mutex_trylock()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_RECURSIVE), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_trylock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), EBUSY);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), 0);
	(void)pthread_mutex_destroy(&m);
}
