/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_attr_setstack.c
 * @brief Tests for pthread_attr_setstack() (<pthread.h>).
 * @Created: 2026/11/28 18:02:36 by Moutig
 * @Updated: 2026/11/28 18:05:13 by Moutig
 *
 * pthread_attr_setstack(attr, stackaddr, stacksize) sets the storage for
 * the stack of the threads created with attr: base stackaddr, size
 * stacksize, at least {PTHREAD_STACK_MIN}; it returns 0. A thread created
 * with it runs there: its locals are in [stackaddr, stackaddr + stacksize).
 * The storage is a page-aligned mapping (lc_sysMmap), readable and
 * writable. A size below {PTHREAD_STACK_MIN}: EINVAL. Not tested: the
 * alignment and EACCES errors (may fail), a size beyond the limit
 * (unknown), EINTR (never returned).
 *
 * Clauses: POSIX: XSH pthread_attr_setstack().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>

TEST_OPT(prototype, .ref = "XSH pthread_attr_setstack()")
{
	EXPECT_TYPE(&pthread_attr_setstack, int (*)(pthread_attr_t *, void *, size_t));
}

static void	*pstLocal(void *arg)
{
	volatile char	here = 1;

	*(char *volatile *)arg = (char *)&here;
	return ((void *)&here);
}

/* 0, and the thread runs on the storage given. */
TEST_OPT(thread_runs_on_the_given_stack, .ref = "XSH pthread_attr_setstack()")
{
	pthread_attr_t	a;
	pthread_t		t;
	size_t			size = pt_stackMin() + 64 * pt_pageSize();
	char			*mem = lc_sysMmap(size);
	char			*local = NULL;

	ASSERT_NOTNULL(mem);
	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_setstack(&a, mem, size), 0);
	ASSERT_EQ(pthread_create(&t, &a, pstLocal, &local), 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	EXPECT_TRUE(local >= mem && local < mem + size);
	(void)pthread_attr_destroy(&a);
}

/* A size below {PTHREAD_STACK_MIN}: EINVAL. */
TEST_OPT(below_stack_min_einval, .ref = "XSH pthread_attr_setstack()")
{
	pthread_attr_t	a;
	size_t			size = pt_stackMin();
	void			*mem = lc_sysMmap(size);

	ASSERT_NOTNULL(mem);
	ASSERT_EQ(pthread_attr_init(&a), 0);
	EXPECT_EQ(pthread_attr_setstack(&a, mem, size - 1), EINVAL);
	(void)pthread_attr_destroy(&a);
}
