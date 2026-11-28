/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_init.c
 * @brief Tests for pthread_attr_init() (<pthread.h>).
 * @Created: 2026/11/28 18:01:58 by Moutig
 * @Updated: 2026/11/28 18:04:30 by Moutig
 *
 * pthread_attr_init(attr) initializes a thread attributes object with the
 * default value of every attribute and returns 0; pthread_create() then
 * creates threads with those attributes. One object can serve several
 * pthread_create() calls, and changing it later does not change a thread
 * already created. Not tested: ENOMEM (cannot be caused), initializing an
 * object twice (undefined), EBUSY (may fail, 2001), EINTR (never returned).
 * The default of each attribute is in the unit of its get function.
 *
 * Clauses: POSIX: XSH pthread_attr_init(), XSH pthread_create().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_attr_init()")
{
	EXPECT_TYPE(&pthread_attr_init, int (*)(pthread_attr_t *));
}

/* 0, and the object creates a thread. */
TEST_OPT(returns_zero_and_creates_threads, .ref = "XSH pthread_attr_init()")
{
	pthread_attr_t	a;
	pt_run			r;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pt_runWith(&a, 1, &r), 0);
	EXPECT_EQ(pthread_attr_destroy(&a), 0);
}

/* One object in several pthread_create() calls. */
TEST_OPT(one_object_for_several_threads, .ref = "XSH pthread_attr_init()")
{
	pthread_attr_t	a;
	pt_run			r[4];
	int				bad = 0;

	lc_fill(r, 0, sizeof(r));
	ASSERT_EQ(pthread_attr_init(&a), 0);
	for (int i = 0; i < 4; i++)
		bad += pt_runWith(&a, 1, &r[i]) != 0;
	EXPECT_EQ(bad, 0);
	(void)pthread_attr_destroy(&a);
}

static void	*paiWaitGo(void *arg)
{
	(void)pt_waitFor(arg, 1);
	return (arg);
}

/* Changing the object after pthread_create() leaves the thread joinable. */
TEST_OPT(later_changes_do_not_affect_the_thread, .ref = "XSH pthread_create()")
{
	static int		go;
	pthread_attr_t	a;
	pthread_t		t;
	void			*v = NULL;

	pt_set(&go, 0);
	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_setdetachstate(&a, PTHREAD_CREATE_JOINABLE), 0);
	ASSERT_EQ(pthread_create(&t, &a, paiWaitGo, &go), 0);
	EXPECT_EQ(pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED), 0);
	pt_set(&go, 1);
	EXPECT_EQ(pthread_join(t, &v), 0);
	EXPECT_PTREQ(v, &go);
	(void)pthread_attr_destroy(&a);
}
