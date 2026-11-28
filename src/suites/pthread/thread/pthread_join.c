/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_join.c
 * @brief Tests for pthread_join() (<pthread.h>).
 * @Created: 2026/11/28 17:46:43 by Moutig
 * @Updated: 2026/11/28 17:47:16 by Moutig
 *
 * pthread_join(thread, value_ptr) waits until the thread terminates,
 * unless it already has, and returns 0; with a non-NULL value_ptr, the
 * value the thread passed to pthread_exit() is stored there.
 * Not tested: joining the calling thread or a thread that is not
 * joinable (undefined from 2008; in 2001 EDEADLK and EINVAL "may fail",
 * and the ESRCH of a thread ID that no longer exists cannot be obtained
 * without using a dead ID), EDEADLK (may fail), EINTR (never returned,
 * cannot be forced).
 *
 * Clauses: POSIX: XSH pthread_join().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_join()")
{
	EXPECT_TYPE(&pthread_join, int (*)(pthread_t, void **));
}

static void	*pjLate(void *arg)
{
	lc_sleep(50 * 1000000LL);
	pt_set(arg, 1);
	return (arg);
}

/* Waits for a running thread: once it returns, the thread is done. */
TEST_OPT(waits_for_termination, .ref = "XSH pthread_join()")
{
	static int	done;
	pthread_t	t;
	void		*v = NULL;

	pt_set(&done, 0);
	ASSERT_EQ(pthread_create(&t, NULL, pjLate, &done), 0);
	EXPECT_EQ(pthread_join(t, &v), 0);
	EXPECT_EQ(pt_get(&done), 1);
	EXPECT_PTREQ(v, &done);
}

static void	*pjQuick(void *arg)
{
	pt_set(arg, 1);
	return (arg);
}

/* A thread that has already terminated: 0 and its value at once. */
TEST_OPT(already_terminated_thread, .ref = "XSH pthread_join()")
{
	static int	done;
	pthread_t	t;
	void		*v = NULL;

	pt_set(&done, 0);
	ASSERT_EQ(pthread_create(&t, NULL, pjQuick, &done), 0);
	EXPECT_TRUE(pt_waitFor(&done, 1));
	lc_sleep(10 * 1000000LL);
	EXPECT_EQ(pthread_join(t, &v), 0);
	EXPECT_PTREQ(v, &done);
}

static int	gPjObject;

static void	*pjExit(void *arg)
{
	(void)arg;
	pthread_exit(&gPjObject);
	return (NULL);
}

/* value_ptr receives the value passed to pthread_exit(). */
TEST_OPT(value_ptr_gets_the_pthread_exit_value, .ref = "XSH pthread_join()")
{
	pthread_t	t;
	void		*v = NULL;

	ASSERT_EQ(pthread_create(&t, NULL, pjExit, NULL), 0);
	EXPECT_EQ(pthread_join(t, &v), 0);
	EXPECT_PTREQ(v, &gPjObject);
}

/* A NULL value_ptr: the value is not stored, the join still succeeds. */
TEST_OPT(null_value_ptr, .ref = "XSH pthread_join()")
{
	pthread_t	t;

	ASSERT_EQ(pthread_create(&t, NULL, pjExit, NULL), 0);
	EXPECT_EQ(pthread_join(t, NULL), 0);
}

# define PJ_N	4

/* Each join gives the value of its own thread, whatever the order. */
TEST_OPT(each_join_gets_its_thread_value, .ref = "XSH pthread_join()")
{
	static int	flags[PJ_N];
	pthread_t	t[PJ_N];
	int			made = 0;
	int			bad = 0;

	while (made < PJ_N && pthread_create(&t[made], NULL, pjQuick, &flags[made]) == 0)
		made++;
	for (int i = made - 1; i >= 0; i--)
	{
		void	*v = NULL;

		bad += pthread_join(t[i], &v) != 0 || v != &flags[i];
	}
	EXPECT_EQ(made, PJ_N);
	EXPECT_EQ(bad, 0);
}
