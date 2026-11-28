/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_mutex_getprioceiling.c
 * @brief Tests for pthread_mutex_getprioceiling() (<pthread.h>).
 * @Created: 2026/11/28 18:58:09 by Moutig
 * @Updated: 2026/11/28 18:58:27 by Moutig
 *
 * pthread_mutex_getprioceiling(mutex, prioceiling) stores the current
 * priority ceiling of a PTHREAD_PRIO_PROTECT mutex and returns 0 (TPP;
 * RPP): the ceiling of its attributes object, here the lowest and the
 * highest SCHED_FIFO priorities. A PTHREAD_PRIO_NONE mutex: EINVAL (2008).
 * EPERM without the privilege skips the case. Not tested: EINTR (never
 * returned).
 *
 * Clauses: POSIX: XSH pthread_mutex_getprioceiling().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <sched.h>

TEST_OPT(prototype, .ref = "XSH pthread_mutex_getprioceiling()")
{
	EXPECT_TYPE(&pthread_mutex_getprioceiling, int (*)(const pthread_mutex_t *, int *));
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

/* The ceiling the mutex was made with: the SCHED_FIFO bounds. */
TEST_OPT(gives_the_initial_ceiling, .ref = "XSH pthread_mutex_getprioceiling()")
{
	int	ceil[2];

	ceil[0] = sched_get_priority_min(SCHED_FIFO);
	ceil[1] = sched_get_priority_max(SCHED_FIFO);
	for (int i = 0; i < 2; i++)
	{
		pthread_mutex_t	m;
		int				v = -12345;

		PROTECT_MUTEX(&m, ceil[i], PTHREAD_MUTEX_DEFAULT);
		EXPECT_EQ(pthread_mutex_getprioceiling(&m, &v), 0);
		EXPECT_EQ(v, ceil[i]);
		(void)pthread_mutex_destroy(&m);
	}
}

/* A PTHREAD_PRIO_NONE mutex: EINVAL. */
TEST_OPT(prio_none_einval, .since = "POSIX.1-2008", .ref = "XSH pthread_mutex_getprioceiling()")
{
	pthread_mutex_t	m;
	int				v = -12345;

	ASSERT_EQ(pthread_mutex_init(&m, NULL), 0);
	EXPECT_EQ(pthread_mutex_getprioceiling(&m, &v), EINVAL);
	(void)pthread_mutex_destroy(&m);
}
