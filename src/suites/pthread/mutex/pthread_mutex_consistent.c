/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutex_consistent.c
 * @brief Tests for pthread_mutex_consistent() (<pthread.h>).
 * @Created: 2026/11/28 18:46:02 by Moutig
 * @Updated: 2026/11/28 18:47:04 by Moutig
 *
 * pthread_mutex_consistent(mutex) marks the state a robust mutex protects
 * as consistent again and returns 0 (2008). When the owner of a robust
 * mutex ends holding it, the next thread to lock it gets EOWNERDEAD and
 * owns it. After pthread_mutex_consistent() and an unlock the mutex is
 * usable again; unlocked without it, every lock gives ENOTRECOVERABLE.
 * If the notified thread ends too, the next one is notified again. A
 * mutex that is not robust, or not inconsistent, is EINVAL. A robust
 * mutex unlocked by a thread that does not own it is EPERM.
 *
 * The death of the owning thread "may" be reported (XSH
 * pthread_mutex_lock(); pthread_mutex_consistent() says "shall"): the
 * cases that rely on it are skipped when lock returns 0. The death of the
 * owning process "shall" be: checked with a process-shared mutex (TSH)
 * locked by a child of fork(). Not tested: EAGAIN (robust resources),
 * EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_mutex_consistent(), XSH pthread_mutex_lock(), XSH pthread_mutexattr_getrobust().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutex_consistent()")
{
	EXPECT_TYPE(&pthread_mutex_consistent, int (*)(pthread_mutex_t *));
}

/* Initialize *m as a robust mutex, process-shared or not. */
static int	pmcRobust(pthread_mutex_t *m, int shared)
{
	pthread_mutexattr_t	a;
	int					e;

	e = pthread_mutexattr_init(&a);
	if (e != 0)
		return (e);
	e = pthread_mutexattr_setrobust(&a, PTHREAD_MUTEX_ROBUST);
	if (e == 0 && shared)
		e = pthread_mutexattr_setpshared(&a, PTHREAD_PROCESS_SHARED);
	if (e == 0)
		e = pthread_mutex_init(m, &a);
	(void)pthread_mutexattr_destroy(&a);
	return (e);
}

static void	*pmcLockAndEnd(void *arg)
{
	(void)pthread_mutex_lock(arg);
	return (NULL);
}

/* A thread locks *m and ends without unlocking it. @return 0, or -1. */
static int	pmcOwnerDies(pthread_mutex_t *m)
{
	pthread_t	t;

	if (pthread_create(&t, NULL, pmcLockAndEnd, m) != 0 || pthread_join(t, NULL) != 0)
		return (-1);
	return (0);
}

/* Lock a robust mutex whose owner died; skip the case when it is not reported. */
# define PMC_LOCK_DEAD(m)															\
	do																				\
	{																				\
		int	pmc_e;																	\
																					\
		ASSERT_EQ(pmcOwnerDies(m), 0);												\
		pmc_e = pthread_mutex_lock(m);												\
		if (pmc_e == 0)																\
		{																			\
			(void)pthread_mutex_unlock(m);											\
			SKIP("death of the owning thread not reported (may, XSH pthread_mutex_lock())"); \
		}																			\
		ASSERT_EQ(pmc_e, EOWNERDEAD);												\
	} while (0)

/* The owning thread ended: EOWNERDEAD, and the caller owns the mutex. */
TEST_OPT(dead_owner_thread_eownerdead, .ref = "XSH pthread_mutex_lock()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pmcRobust(&m, 0), 0);
	PMC_LOCK_DEAD(&m);
	EXPECT_EQ(pt_tryOther(&m), EBUSY);
	EXPECT_EQ(pthread_mutex_consistent(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* trylock reports it too: EOWNERDEAD, the caller owning the mutex. */
TEST_OPT(trylock_reports_the_dead_owner, .ref = "XSH pthread_mutex_lock()")
{
	pthread_mutex_t	m;
	int				e;

	ASSERT_EQ(pmcRobust(&m, 0), 0);
	ASSERT_EQ(pmcOwnerDies(&m), 0);
	e = pthread_mutex_trylock(&m);
	if (e == 0)
	{
		(void)pthread_mutex_unlock(&m);
		SKIP("death of the owning thread not reported (may, XSH pthread_mutex_lock())");
	}
	ASSERT_EQ(e, EOWNERDEAD);
	EXPECT_EQ(pthread_mutex_consistent(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* Marked consistent, then unlocked: the mutex works normally again. */
TEST_OPT(consistent_makes_it_usable_again, .ref = "XSH pthread_mutex_consistent()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pmcRobust(&m, 0), 0);
	PMC_LOCK_DEAD(&m);
	EXPECT_EQ(pthread_mutex_consistent(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	EXPECT_EQ(pt_tryOther(&m), 0);
	EXPECT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* Unlocked without consistent: every lock then gives ENOTRECOVERABLE. */
TEST_OPT(unlock_without_consistent_not_recoverable, .ref = "XSH pthread_mutexattr_getrobust()")
{
	pthread_mutex_t	m;
	pt_hold			h;

	ASSERT_EQ(pmcRobust(&m, 0), 0);
	PMC_LOCK_DEAD(&m);
	(void)pthread_mutex_unlock(&m);
	EXPECT_EQ(pthread_mutex_lock(&m), ENOTRECOVERABLE);
	EXPECT_EQ(pt_holdStart(&h, &m), ENOTRECOVERABLE);
	pt_holdEnd(&h);
	EXPECT_EQ(pthread_mutex_lock(&m), ENOTRECOVERABLE);
	EXPECT_EQ(pthread_mutex_destroy(&m), 0);
}

/*
 * trylock too: ENOTRECOVERABLE, and the mutex is not taken by it (another
 * thread's trylock also gives ENOTRECOVERABLE, not EBUSY). No blocking
 * lock afterwards: a libc that leaves it taken would hang the case.
 */
TEST_OPT(trylock_not_recoverable, .ref = "XSH pthread_mutexattr_getrobust()")
{
	pthread_mutex_t	m;

	ASSERT_EQ(pmcRobust(&m, 0), 0);
	PMC_LOCK_DEAD(&m);
	(void)pthread_mutex_unlock(&m);
	EXPECT_EQ(pthread_mutex_trylock(&m), ENOTRECOVERABLE);
	EXPECT_EQ(pt_tryOther(&m), ENOTRECOVERABLE);
}

static void	*pmcNotifiedThenEnd(void *arg)
{
	int	e = pthread_mutex_lock(arg);

	return ((void *)(long)e);
}

/* The notified thread ends before consistent or unlock: notified again. */
TEST_OPT(notified_owner_dies_notified_again, .ref = "XSH pthread_mutex_consistent()")
{
	pthread_mutex_t	m;
	pthread_t		t;
	void			*v = NULL;

	ASSERT_EQ(pmcRobust(&m, 0), 0);
	ASSERT_EQ(pmcOwnerDies(&m), 0);
	ASSERT_EQ(pthread_create(&t, NULL, pmcNotifiedThenEnd, &m), 0);
	ASSERT_EQ(pthread_join(t, &v), 0);
	if ((long)v == 0)
		SKIP("death of the owning thread not reported (may, XSH pthread_mutex_lock())");
	ASSERT_EQ((long)v, EOWNERDEAD);
	EXPECT_EQ(pthread_mutex_lock(&m), EOWNERDEAD);
	EXPECT_EQ(pthread_mutex_consistent(&m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), 0);
	(void)pthread_mutex_destroy(&m);
}

static int	pmcChildLocks(void *arg)
{
	return (pthread_mutex_lock(arg) == 0 ? 0 : 1);
}

/* TSH: the owning process ended holding it: EOWNERDEAD (shall). */
TEST_OPT(dead_owner_process_eownerdead, .opt = "TSH", .ref = "XSH pthread_mutexattr_getrobust()")
{
	pthread_mutex_t	*m = pt_shared(sizeof(*m));

	ASSERT_NOTNULL(m);
	ASSERT_EQ(pmcRobust(m, 1), 0);
	{
		PT_FORK(st, pmcChildLocks, m);
		ASSERT_EQ(st, 0);
	}
	EXPECT_EQ(pthread_mutex_lock(m), EOWNERDEAD);
	EXPECT_EQ(pthread_mutex_consistent(m), 0);
	EXPECT_EQ(pthread_mutex_unlock(m), 0);
	(void)pthread_mutex_destroy(m);
}

/* Not robust, or robust but consistent: EINVAL. */
TEST_OPT(not_inconsistent_einval, .ref = "XSH pthread_mutex_consistent()")
{
	pthread_mutex_t	plain;
	pthread_mutex_t	m;

	ASSERT_EQ(pthread_mutex_init(&plain, NULL), 0);
	ASSERT_EQ(pthread_mutex_lock(&plain), 0);
	EXPECT_EQ(pthread_mutex_consistent(&plain), EINVAL);
	(void)pthread_mutex_unlock(&plain);
	(void)pthread_mutex_destroy(&plain);
	ASSERT_EQ(pmcRobust(&m, 0), 0);
	ASSERT_EQ(pthread_mutex_lock(&m), 0);
	EXPECT_EQ(pthread_mutex_consistent(&m), EINVAL);
	(void)pthread_mutex_unlock(&m);
	(void)pthread_mutex_destroy(&m);
}

/* A robust mutex unlocked by a thread that does not own it: EPERM. */
TEST_OPT(robust_unlock_not_owner_eperm, .ref = "XSH pthread_mutex_unlock()")
{
	pthread_mutex_t	m;
	pt_hold			h;

	ASSERT_EQ(pmcRobust(&m, 0), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), EPERM);
	ASSERT_EQ(pt_holdStart(&h, &m), 0);
	EXPECT_EQ(pthread_mutex_unlock(&m), EPERM);
	pt_holdEnd(&h);
	(void)pthread_mutex_destroy(&m);
}
