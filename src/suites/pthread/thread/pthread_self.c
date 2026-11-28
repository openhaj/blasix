/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_self.c
 * @brief Tests for pthread_self() (<pthread.h>).
 * @Created: 2026/11/28 17:50:26 by Moutig
 * @Updated: 2026/11/28 17:50:39 by Moutig
 *
 * pthread_self() returns the thread ID of the calling thread: the ID
 * pthread_create() stored for it, the same at each call, another one in
 * another thread. It always succeeds. IDs are compared with
 * pthread_equal(), and only while both threads exist (the ID of a thread
 * that has been joined is no longer valid).
 *
 * Clauses: POSIX: XSH pthread_self(), XSH pthread_create().
 */
#include "pthread/pthread_cases.h"

TEST_OPT(prototype, .ref = "XSH pthread_self()")
{
	EXPECT_TYPE(&pthread_self, pthread_t (*)(void));
}

typedef struct ps_rec
{
	pthread_t	self;
	int			ready;
	int			go;
}	ps_rec;

static void	*psRecord(void *arg)
{
	ps_rec	*r = arg;

	r->self = pthread_self();
	pt_set(&r->ready, 1);
	(void)pt_waitFor(&r->go, 1);
	return (NULL);
}

/* In a new thread: the ID pthread_create() stored for that thread. */
TEST_OPT(is_the_id_pthread_create_stored, .ref = "XSH pthread_create()")
{
	static ps_rec	r;
	pthread_t		t;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_create(&t, NULL, psRecord, &r), 0);
	if (pt_waitFor(&r.ready, 1))
		EXPECT_NE(pthread_equal(r.self, t), 0);
	else
		EXPECT_TRUE(0);
	pt_set(&r.go, 1);
	ASSERT_EQ(pthread_join(t, NULL), 0);
}

/* The same ID at each call in one thread. */
TEST_OPT(same_id_at_each_call, .ref = "XSH pthread_self()")
{
	pthread_t	a = pthread_self();
	pthread_t	b = pthread_self();

	EXPECT_NE(pthread_equal(a, b), 0);
}

/* Another thread, another ID. */
TEST_OPT(differs_between_threads, .ref = "XSH pthread_self()")
{
	static ps_rec	r;
	pthread_t		t;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_create(&t, NULL, psRecord, &r), 0);
	if (pt_waitFor(&r.ready, 1))
		EXPECT_EQ(pthread_equal(r.self, pthread_self()), 0);
	else
		EXPECT_TRUE(0);
	pt_set(&r.go, 1);
	ASSERT_EQ(pthread_join(t, NULL), 0);
}
