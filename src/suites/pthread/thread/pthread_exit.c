/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_exit.c
 * @brief Tests for pthread_exit() (<pthread.h>).
 * @Created: 2026/11/28 17:46:36 by Moutig
 * @Updated: 2026/11/28 17:47:26 by Moutig
 *
 * pthread_exit(value_ptr) ends the calling thread, from any depth of
 * calls, and does not return; value_ptr goes to a join. The cleanup
 * handlers still pushed run in reverse order, then the destructors of
 * the thread-specific data. Process resources (a locked mutex) stay; no
 * atexit() handler runs. From main(), the other threads go on; after the
 * last thread the process exits with status 0, as if by exit(0), so the
 * atexit() handlers run then. The cases that end a process run in a child
 * of fork() (pt_fork).
 *
 * Clauses: POSIX: XSH pthread_exit().
 */
#include "pthread/pthread_cases.h"

#include <errno.h>
#include <stdlib.h>

TEST_OPT(prototype, .ref = "XSH pthread_exit()")
{
	EXPECT_TYPE(&pthread_exit, void (*)(void *));
}

static int	gPeObject;

static void	peDeep(int depth, int *after)
{
	if (depth > 0)
		peDeep(depth - 1, after);
	else
		pthread_exit(&gPeObject);
	pt_set(after, 1);
}

static void	*peFromDeep(void *arg)
{
	peDeep(3, arg);
	pt_set(arg, 1);
	return (NULL);
}

/* From nested calls: the thread ends, its value goes to the join. */
TEST_OPT(ends_the_thread_with_its_value, .ref = "XSH pthread_exit()")
{
	static int	after;
	pthread_t	t;
	void		*v = NULL;

	ASSERT_EQ(pthread_create(&t, NULL, peFromDeep, &after), 0);
	ASSERT_EQ(pthread_join(t, &v), 0);
	EXPECT_PTREQ(v, &gPeObject);
}

/* It does not return: nothing after the call runs. */
TEST_OPT(does_not_return, .ref = "XSH pthread_exit()")
{
	static int	after;
	pthread_t	t;

	pt_set(&after, 0);
	ASSERT_EQ(pthread_create(&t, NULL, peFromDeep, &after), 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	EXPECT_EQ(pt_get(&after), 0);
}

/* The order in which the handlers and destructors ran. */
typedef struct pe_order
{
	int	n;
	int	seen[4];
}	pe_order;

static pe_order	gPeOrder;

static void	peNote(void *arg)
{
	int	i = pt_add(&gPeOrder.n, 1) - 1;

	if (i < 4)
		gPeOrder.seen[i] = (int)(long)arg;
}

static void	*peCleanups(void *arg)
{
	(void)arg;
	pthread_cleanup_push(peNote, (void *)1L);
	pthread_cleanup_push(peNote, (void *)2L);
	pthread_cleanup_push(peNote, (void *)3L);
	pthread_exit(NULL);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	return (NULL);
}

/* The cleanup handlers still pushed run, the last pushed first. */
TEST_OPT(cleanup_handlers_in_reverse_order, .ref = "XSH pthread_exit()")
{
	pthread_t	t;

	lc_fill(&gPeOrder, 0, sizeof(gPeOrder));
	ASSERT_EQ(pthread_create(&t, NULL, peCleanups, NULL), 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	ASSERT_EQ(pt_get(&gPeOrder.n), 3);
	EXPECT_EQ(gPeOrder.seen[0], 3);
	EXPECT_EQ(gPeOrder.seen[1], 2);
	EXPECT_EQ(gPeOrder.seen[2], 1);
}

static pthread_key_t	gPeKey;

static void	*peKeyAndCleanup(void *arg)
{
	(void)arg;
	(void)pthread_setspecific(gPeKey, (void *)20L);
	pthread_cleanup_push(peNote, (void *)10L);
	pthread_exit(NULL);
	pthread_cleanup_pop(0);
	return (NULL);
}

/* The destructors of the thread-specific data run after the handlers. */
TEST_OPT(destructors_after_cleanup_handlers, .ref = "XSH pthread_exit()")
{
	pthread_t	t;

	lc_fill(&gPeOrder, 0, sizeof(gPeOrder));
	ASSERT_EQ(pthread_key_create(&gPeKey, peNote), 0);
	ASSERT_EQ(pthread_create(&t, NULL, peKeyAndCleanup, NULL), 0);
	EXPECT_EQ(pthread_join(t, NULL), 0);
	(void)pthread_key_delete(gPeKey);
	ASSERT_EQ(pt_get(&gPeOrder.n), 2);
	EXPECT_EQ(gPeOrder.seen[0], 10);
	EXPECT_EQ(gPeOrder.seen[1], 20);
}

static pthread_mutex_t	gPeMutex = PTHREAD_MUTEX_INITIALIZER;

static void	*peLockAndExit(void *arg)
{
	pt_set(arg, pthread_mutex_lock(&gPeMutex) == 0);
	pthread_exit(NULL);
	return (NULL);
}

/* Process resources are not released: a mutex it locked stays locked. */
TEST_OPT(keeps_process_resources, .ref = "XSH pthread_exit()")
{
	static int	locked;
	pthread_t	t;

	ASSERT_EQ(pthread_create(&t, NULL, peLockAndExit, &locked), 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	ASSERT_EQ(pt_get(&locked), 1);
	EXPECT_EQ(pthread_mutex_trylock(&gPeMutex), EBUSY);
}

/* What the child processes report through shared memory. */
typedef struct pe_shared
{
	int	atexitRan;
	int	otherRan;
}	pe_shared;

static pe_shared	*gPeShared;

static void	peAtexit(void)
{
	pt_set(&gPeShared->atexitRan, 1);
}

static void	*peReturnNull(void *arg)
{
	(void)arg;
	pthread_exit(NULL);
	return (NULL);
}

static int	peNoAtexitChild(void *arg)
{
	pthread_t	t;

	(void)arg;
	if (atexit(peAtexit) != 0)
		return (2);
	if (pthread_create(&t, NULL, peReturnNull, NULL) != 0)
		return (3);
	if (pthread_join(t, NULL) != 0)
		return (4);
	/* Ends with lc_exitNow: only pthread_exit() could have run it. */
	return (pt_get(&gPeShared->atexitRan) ? 5 : 0);
}

/* No process-level cleanup: the atexit() handlers do not run. */
TEST_OPT(runs_no_atexit_handler, .ref = "XSH pthread_exit()")
{
	gPeShared = pt_shared(sizeof(*gPeShared));
	ASSERT_NOTNULL(gPeShared);
	PT_FORK(st, peNoAtexitChild, NULL);
	EXPECT_EQ(st, 0);
}

static void	*peLateOther(void *arg)
{
	(void)arg;
	lc_sleep(50 * 1000000LL);
	pt_set(&gPeShared->otherRan, 1);
	return (NULL);
}

static int	peMainExitsChild(void *arg)
{
	pthread_t	t;

	(void)arg;
	if (atexit(peAtexit) != 0)
		return (2);
	if (pthread_create(&t, NULL, peLateOther, NULL) != 0)
		return (3);
	pthread_exit(NULL);
	return (4);
}

/* From main(): the other threads go on, the process does not end yet. */
TEST_OPT(main_thread_exit_lets_others_run, .ref = "XSH pthread_exit()")
{
	gPeShared = pt_shared(sizeof(*gPeShared));
	ASSERT_NOTNULL(gPeShared);
	PT_FORK(st, peMainExitsChild, NULL);
	EXPECT_EQ(pt_get(&gPeShared->otherRan), 1);
}

/* After the last thread: exit status 0, as if by exit(0) (atexit runs). */
TEST_OPT(last_thread_exits_the_process_with_zero, .ref = "XSH pthread_exit()")
{
	gPeShared = pt_shared(sizeof(*gPeShared));
	ASSERT_NOTNULL(gPeShared);
	PT_FORK(st, peMainExitsChild, NULL);
	EXPECT_EQ(st, 0);
	EXPECT_EQ(pt_get(&gPeShared->atexitRan), 1);
}
