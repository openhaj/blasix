/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutexattr_setpshared.c
 * @brief Tests for pthread_mutexattr_setpshared() (<pthread.h>).
 * @Created: 2026/11/28 18:30:17 by Moutig
 * @Updated: 2026/11/28 18:31:40 by Moutig
 *
 * pthread_mutexattr_setpshared(attr, pshared) sets the process-shared
 * attribute and returns 0 (TSH). With PTHREAD_PROCESS_SHARED a mutex in
 * memory shared by several processes can be used by all of them: while
 * the parent holds it, a child of fork() gets EBUSY from trylock; once it
 * is released, a child locks it. Not tested: an invalid value (EINVAL
 * may fail), copies and other mappings (XSH 2.9.9), EINTR (never
 * returned).
 *
 * Clauses: POSIX: XSH pthread_mutexattr_setpshared().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutexattr_setpshared()")
{
	EXPECT_TYPE(&pthread_mutexattr_setpshared, int (*)(pthread_mutexattr_t *, int));
}

/* Both values: 0. */
TEST_OPT(both_values_accepted, .ref = "XSH pthread_mutexattr_setpshared()")
{
	pthread_mutexattr_t	a;

	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	EXPECT_EQ(pthread_mutexattr_setpshared(&a, PTHREAD_PROCESS_SHARED), 0);
	EXPECT_EQ(pthread_mutexattr_setpshared(&a, PTHREAD_PROCESS_PRIVATE), 0);
	(void)pthread_mutexattr_destroy(&a);
}

static int	psmTry(void *arg)
{
	int	e = pthread_mutex_trylock(arg);

	if (e == 0)
		(void)pthread_mutex_unlock(arg);
	return (e);
}

/* Shared: another process sees it locked (EBUSY), then locks it once released. */
TEST_OPT(shared_mutex_works_across_processes, .ref = "XSH pthread_mutexattr_setpshared()")
{
	pthread_mutexattr_t	a;
	pthread_mutex_t		*m = pt_shared(sizeof(*m));

	ASSERT_NOTNULL(m);
	ASSERT_EQ(pthread_mutexattr_init(&a), 0);
	ASSERT_EQ(pthread_mutexattr_setpshared(&a, PTHREAD_PROCESS_SHARED), 0);
	ASSERT_EQ(pthread_mutex_init(m, &a), 0);
	(void)pthread_mutexattr_destroy(&a);
	ASSERT_EQ(pthread_mutex_lock(m), 0);
	{
		PT_FORK(busy, psmTry, m);
		EXPECT_EQ(busy, EBUSY);
	}
	ASSERT_EQ(pthread_mutex_unlock(m), 0);
	{
		PT_FORK(free_, psmTry, m);
		EXPECT_EQ(free_, 0);
	}
	(void)pthread_mutex_destroy(m);
}
