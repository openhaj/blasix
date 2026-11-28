/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_cond_init.c
 * @brief Tests for pthread_cond_init() (<pthread.h>).
 * @Created: 2026/11/28 19:02:36 by Moutig
 * @Updated: 2026/11/28 19:03:48 by Moutig
 *
 * pthread_cond_init(cond, attr) initializes the condition variable with
 * the attributes of attr (the defaults when attr is NULL, as a default
 * attributes object) and returns 0. PTHREAD_COND_INITIALIZER gives a
 * static condition variable the same as init with a NULL attr. Each is
 * shown to wake a waiter. Not tested: EAGAIN and ENOMEM (cannot be
 * caused), initializing twice (undefined), copies (XSH 2.9.9), EINTR
 * (never returned).
 *
 * Clauses: POSIX: XSH pthread_cond_init().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <time.h>

TEST_OPT(prototype, .ref = "XSH pthread_cond_init()")
{
	EXPECT_TYPE(&pthread_cond_init,
		int (*)(pthread_cond_t *, const pthread_condattr_t *));
}

/* A waiter on v->c, woken by a signal. @return 1 when it was. */
static int	wakesAWaiter(pt_cv *v)
{
	pthread_t	t;

	if (pthread_create(&t, NULL, pt_cvWaiter, v) != 0)
		return (0);
	if (pt_cvBlocked(v, 1))
		(void)pthread_cond_signal(&v->c);
	(void)pthread_join(t, NULL);
	return (pt_get(&v->woken) == 1);
}

/* attr NULL: 0, and the condition variable works. */
TEST_OPT(null_attr, .ref = "XSH pthread_cond_init()")
{
	pt_cv	v;

	ASSERT_EQ(pt_cvInit(&v, 5 * LC_NS_PER_SEC), 0);
	EXPECT_TRUE(wakesAWaiter(&v));
	(void)pthread_cond_destroy(&v.c);
}

/* A default attributes object: as attr NULL. */
TEST_OPT(default_attr_object, .ref = "XSH pthread_cond_init()")
{
	pthread_condattr_t	a;
	pt_cv				v;

	ASSERT_EQ(pt_cvInit(&v, 5 * LC_NS_PER_SEC), 0);
	(void)pthread_cond_destroy(&v.c);
	ASSERT_EQ(pthread_condattr_init(&a), 0);
	EXPECT_EQ(pthread_cond_init(&v.c, &a), 0);
	(void)pthread_condattr_destroy(&a);
	EXPECT_TRUE(wakesAWaiter(&v));
	(void)pthread_cond_destroy(&v.c);
}

/* PTHREAD_COND_INITIALIZER: a working condition variable (not copied: XSH 2.9.9). */
TEST_OPT(static_initializer, .ref = "XSH pthread_cond_init()")
{
	static pt_cv	v = {
		.m = PTHREAD_MUTEX_INITIALIZER,
		.c = PTHREAD_COND_INITIALIZER,
		.waitNs = 5 * LC_NS_PER_SEC,
	};

	EXPECT_TRUE(wakesAWaiter(&v));
}
