/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutex_lock.c
 * @brief Tests for pthread_mutex_lock() (<pthread.h>).
 * @Created: 2026/11/28 18:35:50 by Moutig
 * @Updated: 2026/11/28 18:36:42 by Moutig
 *
 * pthread_mutex_lock(mutex) locks the mutex, the caller as owner, and
 * returns 0; when another thread owns it, the caller blocks until it is
 * released. Threads that increment a counter under the lock lose no
 * increment. A relock: ERRORCHECK gives an error (EDEADLK from 2008);
 * RECURSIVE succeeds and counts, the mutex being free once unlocked as
 * many times; NORMAL deadlocks (checked in a child of fork(), which is
 * then ended). Not tested: DEFAULT relock (undefined), EAGAIN (recursion
 * limit unknown), the signal handler case, robust and PRIO_PROTECT
 * mutexes (their units), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutex_lock(), XSH pthread_mutexattr_settype().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutex_lock()")
{
	EXPECT_TYPE(&pthread_mutex_lock, int (*)(pthread_mutex_t *));
}

/* 0, and the caller owns the mutex: another thread gets EBUSY. */
TEST_OPT(locks_with_the_caller_as_owner, .ref = "XSH pthread_mutex_lock()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pthread_mutex_init(&m, NULL), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), EBUSY);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

typedef struct pml_wait
{
	pthread_mutex_t	*m;
	int				started;
	int				got;
}	pml_wait;

static void	*pmlLocker(void *arg)
{
	pml_wait	*w = arg;

	pt_set(&w->started, 1);
	if (pthread_mutex_lock(w->m) == 0)
	{
		pt_set(&w->got, 1);
		(void)pthread_mutex_unlock(w->m);
	}
	return (NULL);
}

/* Owned by another thread: the caller blocks until it is released. */
TEST_OPT(blocks_until_released, .ref = "XSH pthread_mutex_lock()")
{
	static pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;
	static pml_wait			w;
	pthread_t				t;

	lc_fill(&w, 0, sizeof(w));
	w.m = &m;
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	ASSERT_EQ(pthread_create(&t, NULL, pmlLocker, &w), 0);
	EXPECT_TRUE(pt_waitFor(&w.started, 1));
	lc_sleep(50 * 1000000LL);
	EXPECT_EQ(pt_get(&w.got), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_TRUE(pt_waitFor(&w.got, 1));
	ASSERT_EQ(pthread_join(t, NULL), 0);
}

# define PML_THREADS	4
# define PML_ROUNDS		20000

static pthread_mutex_t	gPmlCount = PTHREAD_MUTEX_INITIALIZER;
static volatile long	gPmlValue;

static void	*pmlIncrement(void *arg)
{
	(void)arg;
	for (int i = 0; i < PML_ROUNDS; i++)
	{
		if (pthread_mutex_lock(&gPmlCount) != 0)
			return (NULL);
		gPmlValue = gPmlValue + 1;
		(void)pthread_mutex_unlock(&gPmlCount);
	}
	return (NULL);
}

/* Mutual exclusion: 4 threads x 20000 increments under the lock, none lost. */
TEST_OPT(mutual_exclusion, .ref = "XSH pthread_mutex_lock()", .timeout = 60)
{
	pthread_t	t[PML_THREADS];
	int			made = 0;

	gPmlValue = 0;
	while (made < PML_THREADS && pthread_create(&t[made], NULL, pmlIncrement, NULL) == 0)
		made++;
	for (int i = 0; i < made; i++)
		(void)pthread_join(t[i], NULL);
	EXPECT_EQ(made, PML_THREADS);
	EXPECT_EQ(gPmlValue, (long)PML_THREADS * PML_ROUNDS);
}

/* ERRORCHECK, relocked by its owner: an error, and still locked once. */
TEST_OPT(errorcheck_relock_is_an_error, .ref = "XSH pthread_mutexattr_settype()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_ERRORCHECK), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_NE(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* ERRORCHECK relock: the error is EDEADLK (2008). */
TEST_OPT(errorcheck_relock_edeadlk, .since = "POSIX.1-2008", .ref = "XSH pthread_mutex_lock()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_ERRORCHECK), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), EDEADLK);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* RECURSIVE: 3 locks need 3 unlocks before another thread can lock it. */
TEST_OPT(recursive_lock_count, .ref = "XSH pthread_mutex_lock()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_RECURSIVE), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), EBUSY);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

static int	*gPmlRelocked;

static void	*pmlRelock(void *arg)
{
	pthread_mutex_t	*m = arg;

	if (pthread_mutex_lock(m) == 0)
	{
		(void)pthread_mutex_lock(m);
		pt_set(gPmlRelocked, 1);
	}
	return (NULL);
}

static int	pmlNormalChild(void *arg)
{
	pthread_mutex_t	m;
	pthread_t		t;

	(void)arg;
	if (pt_mutexOfType(&m, PTHREAD_MUTEX_NORMAL) != 0)
		return (2);
	if (pthread_create(&t, NULL, pmlRelock, &m) != 0)
		return (3);
	lc_sleep(100 * 1000000LL);
	/* Still blocked: the process ends here (lc_exitNow). */
	return (pt_get(gPmlRelocked) ? 4 : 0);
}

/* NORMAL, relocked by its owner: deadlock, the second lock never returns. */
TEST_OPT(normal_relock_deadlocks, .ref = "XSH pthread_mutex_lock()")
{
	gPmlRelocked = pt_shared(sizeof(int));
	ASSERT_NOTNULL(gPmlRelocked);
	PT_FORK(st, pmlNormalChild, NULL);
	EXPECT_EQ(st, 0);
}
