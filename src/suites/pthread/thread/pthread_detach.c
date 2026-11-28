/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_detach.c
 * @brief Tests for pthread_detach() (<pthread.h>).
 * @Created: 2026/11/28 17:48:41 by Moutig
 * @Updated: 2026/11/28 17:49:11 by Moutig
 *
 * pthread_detach(thread) returns 0 and lets the implementation reclaim
 * the storage of the thread when it terminates; a thread still running
 * is not terminated. Its storage is reclaimed: many detached threads in a
 * row can be created. Not tested: a thread that is not joinable
 * (undefined from 2008; EINVAL and ESRCH "may fail" in 2001), a second
 * detach (unspecified), EINTR (never returned, cannot be forced).
 *
 * Clauses: POSIX: XSH pthread_detach().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_detach()")
{
	EXPECT_TYPE(&pthread_detach, int (*)(pthread_t));
}

typedef struct pd_meet
{
	int	go;
	int	done;
	int	ret;
}	pd_meet;

static void	*pdWaitGo(void *arg)
{
	pd_meet	*m = arg;

	if (pt_waitFor(&m->go, 1))
		pt_set(&m->done, 1);
	return (NULL);
}

/* A running thread: 0, and the thread is not terminated by it. */
TEST_OPT(running_thread_goes_on, .ref = "XSH pthread_detach()")
{
	static pd_meet	m;
	pthread_t		t;

	lc_fill(&m, 0, sizeof(m));
	ASSERT_EQ(pthread_create(&t, NULL, pdWaitGo, &m), 0);
	EXPECT_EQ(pthread_detach(t), 0);
	pt_set(&m.go, 1);
	EXPECT_TRUE(pt_waitFor(&m.done, 1));
}

static void	*pdQuick(void *arg)
{
	pt_set(arg, 1);
	return (NULL);
}

/* A thread that has terminated but was not joined: 0. */
TEST_OPT(terminated_thread, .ref = "XSH pthread_detach()")
{
	static int	done;
	pthread_t	t;

	pt_set(&done, 0);
	ASSERT_EQ(pthread_create(&t, NULL, pdQuick, &done), 0);
	EXPECT_TRUE(pt_waitFor(&done, 1));
	lc_sleep(10 * 1000000LL);
	EXPECT_EQ(pthread_detach(t), 0);
}

static void	*pdSelf(void *arg)
{
	pd_meet	*m = arg;

	m->ret = pthread_detach(pthread_self());
	pt_set(&m->done, 1);
	return (NULL);
}

/* A thread can detach itself. */
TEST_OPT(thread_detaches_itself, .ref = "XSH pthread_detach()")
{
	static pd_meet	m;
	pthread_t		t;

	lc_fill(&m, 0, sizeof(m));
	ASSERT_EQ(pthread_create(&t, NULL, pdSelf, &m), 0);
	ASSERT_TRUE(pt_waitFor(&m.done, 1));
	EXPECT_EQ(m.ret, 0);
}

# define PD_TOTAL	1000
# define PD_ALIVE	16

static int	gPdEnded;

static void	*pdEnd(void *arg)
{
	(void)arg;
	(void)pt_add(&gPdEnded, 1);
	return (NULL);
}

/*
 * Storage is reclaimed when a detached thread ends: 1000 threads created
 * and detached in a row, at most 16 alive at once, all succeed.
 */
TEST_OPT(storage_is_reclaimed, .ref = "XSH pthread_detach()", .timeout = 60)
{
	int	created = 0;
	int	failed = 0;

	pt_set(&gPdEnded, 0);
	while (created < PD_TOTAL && failed == 0)
	{
		pthread_t	t;
		long long	end = lc_now() + PT_WAIT_NS;

		while (created - pt_get(&gPdEnded) >= PD_ALIVE && lc_now() < end)
			lc_sleep(100000);
		if (pthread_create(&t, NULL, pdEnd, NULL) != 0 || pthread_detach(t) != 0)
			failed = 1;
		else
			created++;
	}
	EXPECT_EQ(created, PD_TOTAL);
	EXPECT_TRUE(pt_waitFor(&gPdEnded, created));
}
