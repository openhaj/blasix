/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_condattr_setpshared.c
 * @brief Tests for pthread_condattr_setpshared() (<pthread.h>).
 * @Created: 2026/11/28 19:00:37 by Moutig
 * @Updated: 2026/11/28 19:00:54 by Moutig
 *
 * pthread_condattr_setpshared(attr, pshared) sets the process-shared
 * attribute and returns 0 (TSH). With PTHREAD_PROCESS_SHARED, a condition
 * variable (and its mutex) in memory shared with a child of fork() works
 * across the processes: the child waits on it, a thread of the parent
 * signals it. Not tested: an invalid value (EINVAL may fail), copies and
 * other mappings (XSH 2.9.9), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_condattr_setpshared().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <time.h>

TEST_OPT(prototype, .ref = "XSH pthread_condattr_setpshared()")
{
	EXPECT_TYPE(&pthread_condattr_setpshared, int (*)(pthread_condattr_t *, int));
}

/* Both values: 0. */
TEST_OPT(both_values_accepted, .ref = "XSH pthread_condattr_setpshared()")
{
	pthread_condattr_t	a;

	ASSERT_EQ(pthread_condattr_init(&a), 0);
	EXPECT_EQ(pthread_condattr_setpshared(&a, PTHREAD_PROCESS_SHARED), 0);
	EXPECT_EQ(pthread_condattr_setpshared(&a, PTHREAD_PROCESS_PRIVATE), 0);
	(void)pthread_condattr_destroy(&a);
}

/* In shared memory: the mutex, the condition and the predicate. */
typedef struct pcs_shared
{
	pthread_mutex_t	m;
	pthread_cond_t	c;
	int				ready;
	int				go;
}	pcs_shared;

static int	pcsChildWaits(void *arg)
{
	pcs_shared		*s = arg;
	struct timespec	at = pt_absReal(PT_WAIT_NS);
	int				e = 0;

	if (pthread_mutex_lock(&s->m) != 0)
		return (2);
	pt_set(&s->ready, 1);
	while (!pt_get(&s->go) && e == 0)
		e = pthread_cond_timedwait(&s->c, &s->m, &at);
	(void)pthread_mutex_unlock(&s->m);
	return (pt_get(&s->go) ? 0 : 3);
}

static void	*pcsParentSignals(void *arg)
{
	pcs_shared	*s = arg;

	if (!pt_waitFor(&s->ready, 1) || pthread_mutex_lock(&s->m) != 0)
		return (NULL);
	pt_set(&s->go, 1);
	(void)pthread_cond_signal(&s->c);
	(void)pthread_mutex_unlock(&s->m);
	return (NULL);
}

/* Shared: a child of fork() waiting on it is woken by the parent. */
TEST_OPT(shared_cond_works_across_processes, .ref = "XSH pthread_condattr_setpshared()")
{
	pthread_condattr_t	ca;
	pthread_mutexattr_t	ma;
	pcs_shared			*s = pt_shared(sizeof(*s));
	pthread_t			t;

	ASSERT_NOTNULL(s);
	ASSERT_EQ(pthread_condattr_init(&ca), 0);
	ASSERT_EQ(pthread_condattr_setpshared(&ca, PTHREAD_PROCESS_SHARED), 0);
	ASSERT_EQ(pthread_mutexattr_init(&ma), 0);
	ASSERT_EQ(pthread_mutexattr_setpshared(&ma, PTHREAD_PROCESS_SHARED), 0);
	ASSERT_EQ(pthread_cond_init(&s->c, &ca), 0);
	ASSERT_EQ(pthread_mutex_init(&s->m, &ma), 0);
	(void)pthread_condattr_destroy(&ca);
	(void)pthread_mutexattr_destroy(&ma);
	ASSERT_EQ(pthread_create(&t, NULL, pcsParentSignals, s), 0);
	{
		PT_FORK(st, pcsChildWaits, s);
		EXPECT_EQ(st, 0);
	}
	ASSERT_EQ(pthread_join(t, NULL), 0);
}
