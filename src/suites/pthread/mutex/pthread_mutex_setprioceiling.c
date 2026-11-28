/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutex_setprioceiling.c
 * @brief Tests for pthread_mutex_setprioceiling() (<pthread.h>).
 * @Created: 2026/11/28 18:58:17 by Moutig
 * @Updated: 2026/11/28 18:58:35 by Moutig
 *
 * pthread_mutex_setprioceiling(mutex, prioceiling, old_ceiling) locks the
 * mutex as pthread_mutex_lock() would, sets its priority ceiling, unlocks
 * it and returns 0, the previous ceiling in *old_ceiling (TPP; RPP). It
 * fails, leaving the ceiling as it was, with EDEADLK for an ERRORCHECK
 * mutex the caller owns (2008) and EINVAL for a PTHREAD_PRIO_NONE mutex
 * (2008). EPERM without the privilege skips the case. Not tested: EAGAIN,
 * robust mutexes, an out-of-range ceiling (EINVAL may fail), EINTR (never
 * returned).
 *
 * Clauses: POSIX: XSH pthread_mutex_setprioceiling().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <sched.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutex_setprioceiling()")
{
	EXPECT_TYPE(&pthread_mutex_setprioceiling, int (*)(pthread_mutex_t *, int, int *));
}

/*
 * Initialize *m with the PTHREAD_PRIO_PROTECT protocol, ceiling @p ceiling
 * and type @p type. @return 0, or an error number (EPERM: no privilege).
 */
static int	protectMutex(pthread_mutex_t *m, int ceiling, int type)
{
	pthread_mutexattr_t	a;
	int					e;

	e = pthread_mutexattr_init(&a);
	if (e != 0)
		return (e);
	e = pthread_mutexattr_setprotocol(&a, PTHREAD_PRIO_PROTECT);
	if (e == 0)
		e = pthread_mutexattr_setprioceiling(&a, ceiling);
	if (e == 0)
		e = pthread_mutexattr_settype(&a, type);
	if (e == 0)
		e = pthread_mutex_init(m, &a);
	(void)pthread_mutexattr_destroy(&a);
	return (e);
}

/* Make a PTHREAD_PRIO_PROTECT mutex, or skip the case on EPERM. */
# define PROTECT_MUTEX(m, ceiling, type)												\
	do																				\
	{																				\
		int	pm_e = protectMutex((m), (ceiling), (type));								\
																					\
		if (pm_e == EPERM)															\
			SKIP("no privilege for a PTHREAD_PRIO_PROTECT mutex (EPERM)");			\
		ASSERT_EQ(pm_e, 0);															\
	} while (0)

/* 0, the old ceiling in *old_ceiling, the new one in effect, the mutex unlocked. */
TEST_OPT(changes_and_gives_the_old_ceiling, .ref = "XSH pthread_mutex_setprioceiling()")
{
	pthread_mutex_t	m;
	int				lo = sched_get_priority_min(SCHED_FIFO);
	int				hi = sched_get_priority_max(SCHED_FIFO);
	int				old = -12345;
	int				now = -12345;
	int				e;

	PROTECT_MUTEX(&m, lo, PTHREAD_MUTEX_DEFAULT);
	e = pthread_mutex_setprioceiling(&m, hi, &old);
	if (e == EPERM)
	{
		(void)pthread_mutex_destroy(&m);
		SKIP("no privilege to lock a PTHREAD_PRIO_PROTECT mutex (EPERM)");
	}
	EXPECT_EQ(e, 0);
	EXPECT_EQ(old, lo);
	EXPECT_EQ(pthread_mutex_getprioceiling(&m, &now), 0);
	EXPECT_EQ(now, hi);
	EXPECT_NE(pt_tryOther(&m), EBUSY);
	(void)pthread_mutex_destroy(&m);
}

/* ERRORCHECK, owned by the caller: EDEADLK, and the ceiling is unchanged. */
TEST_OPT(errorcheck_owner_edeadlk, .since = "POSIX.1-2008",
	.ref = "XSH pthread_mutex_setprioceiling()")
{
	pthread_mutex_t	m;
	int				lo = sched_get_priority_min(SCHED_FIFO);
	int				hi = sched_get_priority_max(SCHED_FIFO);
	int				old = -12345;
	int				now = -12345;
	int				e;

	PROTECT_MUTEX(&m, lo, PTHREAD_MUTEX_ERRORCHECK);
	e = pthread_mutex_lock(&m);
	if (e == EPERM || e == EINVAL)
	{
		(void)pthread_mutex_destroy(&m);
		SKIP("cannot lock a PTHREAD_PRIO_PROTECT mutex at this priority");
	}
	ASSERT_EQ(e, 0);
	EXPECT_EQ(pthread_mutex_setprioceiling(&m, hi, &old), EDEADLK);
	EXPECT_EQ(pthread_mutex_getprioceiling(&m, &now), 0);
	EXPECT_EQ(now, lo);
	(void)pthread_mutex_unlock(&m);
	(void)pthread_mutex_destroy(&m);
}

/* A PTHREAD_PRIO_NONE mutex: EINVAL. */
TEST_OPT(prio_none_einval, .since = "POSIX.1-2008", .ref = "XSH pthread_mutex_setprioceiling()")
{
	pthread_mutex_t	m;
	int				old = -12345;

	ASSERT_EQ(pthread_mutex_init(&m, NULL), 0);
	EXPECT_EQ(pthread_mutex_setprioceiling(&m, sched_get_priority_min(SCHED_FIFO), &old),
		EINVAL);
	(void)pthread_mutex_destroy(&m);
}
