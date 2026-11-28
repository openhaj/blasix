/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_equal.c
 * @brief Tests for pthread_equal() (<pthread.h>).
 * @Created: 2026/11/28 17:48:49 by Moutig
 * @Updated: 2026/11/28 17:49:18 by Moutig
 *
 * pthread_equal(t1, t2) returns non-zero when the two thread IDs are
 * equal, zero otherwise. Undefined for an ID that is not valid, so only
 * IDs of existing threads are compared, and, from 2024, PTHREAD_NULL,
 * which never equals the ID of an existing thread (XBD <pthread.h>).
 * Without PTHREAD_NULL the 2024 cases are not implemented.
 *
 * Clauses: POSIX: XSH pthread_equal(), XBD <pthread.h>.
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_equal()")
{
	EXPECT_TYPE(&pthread_equal, int (*)(pthread_t, pthread_t));
}

/* An ID equals itself. */
TEST_OPT(equal_to_itself, .ref = "XSH pthread_equal()")
{
	pthread_t	a = pthread_self();

	EXPECT_NE(pthread_equal(a, a), 0);
}

/* A copy of an ID equals it. */
TEST_OPT(copy_is_equal, .ref = "XSH pthread_equal()")
{
	pthread_t	a = pthread_self();
	pthread_t	b;

	b = a;
	EXPECT_NE(pthread_equal(a, b), 0);
	EXPECT_NE(pthread_equal(b, a), 0);
}

typedef struct pq_meet
{
	int	go;
}	pq_meet;

static void	*pqWaitGo(void *arg)
{
	(void)pt_waitFor(&((pq_meet *)arg)->go, 1);
	return (NULL);
}

/* The IDs of two existing threads: zero, in both orders. */
TEST_OPT(different_threads_are_not_equal, .ref = "XSH pthread_equal()")
{
	static pq_meet	m;
	pthread_t		t;

	pt_set(&m.go, 0);
	ASSERT_EQ(pthread_create(&t, NULL, pqWaitGo, &m), 0);
	EXPECT_EQ(pthread_equal(t, pthread_self()), 0);
	EXPECT_EQ(pthread_equal(pthread_self(), t), 0);
	pt_set(&m.go, 1);
	ASSERT_EQ(pthread_join(t, NULL), 0);
}

/* PTHREAD_NULL equals PTHREAD_NULL (2024). */
TEST_OPT(null_equals_null, .since = "POSIX.1-2024", .ref = "XSH pthread_equal()")
{
#ifdef PTHREAD_NULL
	static pthread_t	n = PTHREAD_NULL;
	pthread_t			m = n;

	EXPECT_NE(pthread_equal(n, m), 0);
#else
	NOT_IMPLEMENTED("PTHREAD_NULL");
#endif
}

/* PTHREAD_NULL never equals the ID of an existing thread (2024). */
TEST_OPT(null_is_no_existing_thread, .since = "POSIX.1-2024", .ref = "XBD <pthread.h>")
{
#ifdef PTHREAD_NULL
	static pthread_t	n = PTHREAD_NULL;
	static pq_meet		m;
	pthread_t			t;

	EXPECT_EQ(pthread_equal(pthread_self(), n), 0);
	pt_set(&m.go, 0);
	ASSERT_EQ(pthread_create(&t, NULL, pqWaitGo, &m), 0);
	EXPECT_EQ(pthread_equal(t, n), 0);
	EXPECT_EQ(pthread_equal(n, t), 0);
	pt_set(&m.go, 1);
	ASSERT_EQ(pthread_join(t, NULL), 0);
#else
	NOT_IMPLEMENTED("PTHREAD_NULL");
#endif
}
