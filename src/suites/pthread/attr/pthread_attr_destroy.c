/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_destroy.c
 * @brief Tests for pthread_attr_destroy() (<pthread.h>).
 * @Created: 2026/11/28 18:00:29 by Moutig
 * @Updated: 2026/11/28 18:03:31 by Moutig
 *
 * pthread_attr_destroy(attr) destroys a thread attributes object and
 * returns 0; the object can then be initialized again by
 * pthread_attr_init(). Threads created with it are not affected. Not
 * tested: any other use of a destroyed object (undefined), EINVAL (may
 * fail, 2001), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_destroy().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_attr_destroy()")
{
	EXPECT_TYPE(&pthread_attr_destroy, int (*)(pthread_attr_t *));
}

/* 0 for an initialized object. */
TEST_OPT(returns_zero, .ref = "XSH pthread_attr_destroy()")
{
	pthread_attr_t	a;

	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_destroy(&a), 0);
}

/* A destroyed object can be initialized again and used. */
TEST_OPT(destroyed_object_can_be_reinitialized, .ref = "XSH pthread_attr_destroy()")
{
	pthread_attr_t	a;
	pt_run			r;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_destroy(&a), 0);
	EXPECT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pt_runWith(&a, 1, &r), 0);
	EXPECT_EQ(pthread_attr_destroy(&a), 0);
}

static void	*padWaitGo(void *arg)
{
	(void)pt_waitFor(arg, 1);
	return (arg);
}

/* Destroying the object does not affect a thread created with it. */
TEST_OPT(threads_created_are_not_affected, .ref = "XSH pthread_attr_destroy()")
{
	static int		go;
	pthread_attr_t	a;
	pthread_t		t;
	void			*v = NULL;

	pt_set(&go, 0);
	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_create(&t, &a, padWaitGo, &go), 0);
	EXPECT_EQ(pthread_attr_destroy(&a), 0);
	pt_set(&go, 1);
	EXPECT_EQ(pthread_join(t, &v), 0);
	EXPECT_PTREQ(v, &go);
}
