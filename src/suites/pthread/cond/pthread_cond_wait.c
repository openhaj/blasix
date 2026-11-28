/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_cond_wait.c
 * @brief Tests for pthread_cond_wait() (<pthread.h>).
 * @Created: 2026/11/28 19:02:58 by Moutig
 * @Updated: 2026/11/28 19:03:24 by Moutig
 *
 * pthread_cond_wait(cond, mutex) atomically releases the mutex and blocks
 * on the condition variable; it returns 0 with the mutex owned by the
 * caller again. While it is blocked, another thread can lock the mutex;
 * a signal sent once the waiter is blocked (the mutex taken and released
 * by the signaler) is not lost. A waiter that loops on its predicate
 * returns once the predicate is set and the condition signaled. An
 * ERRORCHECK mutex the caller does not own: an error, EPERM from 2008.
 * Not tested: spurious wakeups (allowed), cancellation (the cancellation
 * unit), robust mutexes, the dynamic binding of two mutexes (undefined),
 * EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_cond_wait().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <time.h>

TEST_OPT(prototype, .ref = "XSH pthread_cond_wait()")
{
	EXPECT_TYPE(&pthread_cond_wait, int (*)(pthread_cond_t *, pthread_mutex_t *));
}

typedef struct pcw_state
{
	pthread_mutex_t	m;
	pthread_cond_t	c;
	int				waiting;
	int				go;
	int				ret;
	int				owned;
	int				done;
}	pcw_state;

/* Wait until go, with pthread_cond_wait() (a hang ends with the unit's timeout). */
static void	*pcwWaiter(void *arg)
{
	pcw_state	*s = arg;
	int			e = 0;

	if (pthread_mutex_lock(&s->m) != 0)
		return (NULL);
	pt_set(&s->waiting, 1);
	while (!pt_get(&s->go) && e == 0)
		e = pthread_cond_wait(&s->c, &s->m);
	s->ret = e;
	s->owned = pt_tryOther(&s->m) == EBUSY;
	(void)pthread_mutex_unlock(&s->m);
	pt_set(&s->done, 1);
	return (NULL);
}

static int	pcwStart(pcw_state *s, pthread_t *t)
{
	lc_fill(s, 0, sizeof(*s));
	if (pthread_mutex_init(&s->m, NULL) != 0 || pthread_cond_init(&s->c, NULL) != 0)
		return (-1);
	if (pthread_create(t, NULL, pcwWaiter, s) != 0 || !pt_waitFor(&s->waiting, 1))
		return (-1);
	return (0);
}

/* Blocked: the mutex is released (another thread can lock it). */
TEST_OPT(releases_the_mutex_while_blocked, .ref = "XSH pthread_cond_wait()")
{
	static pcw_state	s;
	pthread_t			t;

	ASSERT_EQ(pcwStart(&s, &t), 0);
	EXPECT_EQ(pthread_mutex_lock(&s.m), 0);
	EXPECT_EQ(pt_get(&s.done), 0);
	pt_set(&s.go, 1);
	EXPECT_EQ(pthread_cond_signal(&s.c), 0);
	EXPECT_EQ(pthread_mutex_unlock(&s.m), 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
}

/* Signaled: 0, the mutex owned by the caller on return. */
TEST_OPT(returns_owning_the_mutex, .ref = "XSH pthread_cond_wait()")
{
	static pcw_state	s;
	pthread_t			t;

	ASSERT_EQ(pcwStart(&s, &t), 0);
	ASSERT_EQ(pthread_mutex_lock(&s.m), 0);
	pt_set(&s.go, 1);
	EXPECT_EQ(pthread_cond_signal(&s.c), 0);
	ASSERT_EQ(pthread_mutex_unlock(&s.m), 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	EXPECT_EQ(s.ret, 0);
	EXPECT_EQ(s.owned, 1);
}

/* Signaled right after taking the mutex once the waiter set its flag: not lost. */
TEST_OPT(release_and_block_are_atomic, .ref = "XSH pthread_cond_wait()", .timeout = 30)
{
	static pcw_state	s;
	int					bad = 0;

	for (int i = 0; i < 200; i++)
	{
		pthread_t	t;

		if (pcwStart(&s, &t) != 0)
		{
			bad++;
			break ;
		}
		(void)pthread_mutex_lock(&s.m);
		pt_set(&s.go, 1);
		(void)pthread_cond_signal(&s.c);
		(void)pthread_mutex_unlock(&s.m);
		bad += !pt_waitFor(&s.done, 1);
		(void)pthread_join(t, NULL);
		(void)pthread_cond_destroy(&s.c);
		(void)pthread_mutex_destroy(&s.m);
	}
	EXPECT_EQ(bad, 0);
}

/* An ERRORCHECK mutex the caller does not own: an error, without blocking. */
TEST_OPT(errorcheck_not_owner_error, .ref = "XSH pthread_cond_wait()")
{
	pthread_mutex_t	m;
	pthread_cond_t	c = PTHREAD_COND_INITIALIZER;

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_ERRORCHECK), 0);
	EXPECT_NE(pthread_cond_wait(&c, &m), 0);
	(void)pthread_mutex_destroy(&m);
}

/* That error is EPERM (2008). */
TEST_OPT(errorcheck_not_owner_eperm, .since = "POSIX.1-2008", .ref = "XSH pthread_cond_wait()")
{
	pthread_mutex_t	m;
	pthread_cond_t	c = PTHREAD_COND_INITIALIZER;

	ASSERT_EQ(pt_mutexOfType(&m, PTHREAD_MUTEX_ERRORCHECK), 0);
	EXPECT_EQ(pthread_cond_wait(&c, &m), EPERM);
	(void)pthread_mutex_destroy(&m);
}
