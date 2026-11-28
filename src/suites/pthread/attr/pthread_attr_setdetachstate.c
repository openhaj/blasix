/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_setdetachstate.c
 * @brief Tests for pthread_attr_setdetachstate() (<pthread.h>).
 * @Created: 2026/11/28 18:02:18 by Moutig
 * @Updated: 2026/11/28 18:04:43 by Moutig
 *
 * pthread_attr_setdetachstate(attr, detachstate) sets the detachstate
 * attribute and returns 0: with PTHREAD_CREATE_DETACHED every thread
 * created with attr is detached, with PTHREAD_CREATE_JOINABLE joinable.
 * Any other value: EINVAL. A detached thread is shown to run and to give
 * its storage back (many in a row); joining or detaching it would be an
 * error that is undefined to make. Not tested: EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_setdetachstate().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_attr_setdetachstate()")
{
	EXPECT_TYPE(&pthread_attr_setdetachstate, int (*)(pthread_attr_t *, int));
}

/* PTHREAD_CREATE_JOINABLE: 0, and the threads can be joined. */
TEST_OPT(joinable_threads_can_be_joined, .ref = "XSH pthread_attr_setdetachstate()")
{
	pthread_attr_t	a;
	pt_run			r;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_setdetachstate(&a, PTHREAD_CREATE_JOINABLE), 0);
	EXPECT_EQ(pt_runWith(&a, 1, &r), 0);
	(void)pthread_attr_destroy(&a);
}

/* PTHREAD_CREATE_DETACHED: 0, and the threads run detached. */
TEST_OPT(detached_threads_run, .ref = "XSH pthread_attr_setdetachstate()")
{
	pthread_attr_t	a;
	pt_run			r;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED), 0);
	EXPECT_EQ(pt_runWith(&a, 0, &r), 0);
	(void)pthread_attr_destroy(&a);
}

# define PSD_TOTAL	500
# define PSD_ALIVE	16

static int	gPsdEnded;

static void	*psdEnd(void *arg)
{
	(void)arg;
	(void)pt_add(&gPsdEnded, 1);
	return (NULL);
}

/* Detached at creation: storage is reclaimed, 500 in a row are created. */
TEST_OPT(detached_threads_give_storage_back, .ref = "XSH pthread_attr_setdetachstate()",
	.timeout = 60)
{
	pthread_attr_t	a;
	int				created = 0;

	pt_set(&gPsdEnded, 0);
	ASSERT_EQ(pthread_attr_init(&a), 0);
	ASSERT_EQ(pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED), 0);
	while (created < PSD_TOTAL)
	{
		pthread_t	t;
		long long	end = lc_now() + PT_WAIT_NS;

		while (created - pt_get(&gPsdEnded) >= PSD_ALIVE && lc_now() < end)
			lc_sleep(100000);
		if (pthread_create(&t, &a, psdEnd, NULL) != 0)
			break ;
		created++;
	}
	EXPECT_EQ(created, PSD_TOTAL);
	EXPECT_TRUE(pt_waitFor(&gPsdEnded, created));
	(void)pthread_attr_destroy(&a);
}

/* Neither value: EINVAL, and the attribute keeps its value. */
TEST_OPT(invalid_value_einval, .ref = "XSH pthread_attr_setdetachstate()")
{
	pthread_attr_t	a;
	int				bad = 1;
	int				v = -12345;

	while (bad == PTHREAD_CREATE_DETACHED || bad == PTHREAD_CREATE_JOINABLE)
		bad++;
	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_setdetachstate(&a, bad), EINVAL);
	EXPECT_EQ(pthread_attr_getdetachstate(&a, &v), 0);
	EXPECT_EQ(v, PTHREAD_CREATE_JOINABLE);
	(void)pthread_attr_destroy(&a);
}
