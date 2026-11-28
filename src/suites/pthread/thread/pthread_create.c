/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_create.c
 * @brief Tests for pthread_create() (<pthread.h>).
 * @Created: 2026/11/28 17:46:25 by Moutig
 * @Updated: 2026/11/28 17:47:01 by Moutig
 *
 * pthread_create(thread, attr, start_routine, arg) creates a thread of the
 * process (default attributes when attr is NULL: joinable), stores its ID
 * in *thread and returns 0. The thread runs start_routine(arg), at the
 * same time as its creator; returning from start_routine is an implicit
 * pthread_exit() with the value returned. The signal mask is inherited.
 * EAGAIN (no resources) and EPERM (scheduling) cannot be caused safely;
 * EINTR is never returned, which cannot be forced either. That the
 * stored ID is the thread's own is in pthread_self.c; the attributes
 * have their units.
 *
 * Clauses: POSIX: XSH pthread_create().
 */
#include "pthread/pthread_cases.h"

#include <signal.h>

TEST_OPT(prototype, .ref = "XSH pthread_create()")
{
	EXPECT_TYPE(&pthread_create,
		int (*)(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *));
}

/* What a thread saw of itself. */
typedef struct pc_rec
{
	int		ran;
	void	*arg;
	long	pid;
	long	tid;
}	pc_rec;

static void	*pcRecord(void *arg)
{
	pc_rec	*r = arg;

	r->arg = arg;
	r->pid = lc_sysGetpid();
	r->tid = lc_sysGettid();
	pt_set(&r->ran, 1);
	return (NULL);
}

/* 0, and start_routine runs with arg as its sole argument. */
TEST_OPT(runs_start_routine_with_arg, .ref = "XSH pthread_create()")
{
	pthread_t	t;
	pc_rec		r;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_create(&t, NULL, pcRecord, &r), 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	EXPECT_EQ(pt_get(&r.ran), 1);
	EXPECT_PTREQ(r.arg, &r);
}

/* A thread within the process: same process ID, another thread. */
TEST_OPT(new_thread_of_the_process, .ref = "XSH pthread_create()")
{
	pthread_t	t;
	pc_rec		r;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_create(&t, NULL, pcRecord, &r), 0);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	EXPECT_EQ(r.pid, lc_sysGetpid());
	EXPECT_NE(r.tid, lc_sysGettid());
}

static void	*pcReturn(void *arg)
{
	return ((char *)arg + 1);
}

/* Returning from start_routine: as pthread_exit() with the value returned. */
TEST_OPT(return_value_is_the_exit_status, .ref = "XSH pthread_create()")
{
	static char	buf[4];
	pthread_t	t;
	void		*v = NULL;

	ASSERT_EQ(pthread_create(&t, NULL, pcReturn, buf), 0);
	ASSERT_EQ(pthread_join(t, &v), 0);
	EXPECT_PTREQ(v, buf + 1);
}

/* attr NULL: the default attributes, so the thread is joinable. */
TEST_OPT(null_attr_gives_a_joinable_thread, .ref = "XSH pthread_create()")
{
	pthread_t	t;
	pc_rec		r;

	lc_fill(&r, 0, sizeof(r));
	ASSERT_EQ(pthread_create(&t, NULL, pcRecord, &r), 0);
	EXPECT_EQ(pthread_join(t, NULL), 0);
}

typedef struct pc_meet
{
	int	started;
	int	go;
	int	done;
}	pc_meet;

static void	*pcMeet(void *arg)
{
	pc_meet	*m = arg;

	pt_set(&m->started, 1);
	if (pt_waitFor(&m->go, 1))
		pt_set(&m->done, 1);
	return (NULL);
}

/* The new thread runs while its creator does: each waits for the other. */
TEST_OPT(runs_concurrently_with_the_creator, .ref = "XSH pthread_create()")
{
	pthread_t	t;
	pc_meet		m;

	lc_fill(&m, 0, sizeof(m));
	ASSERT_EQ(pthread_create(&t, NULL, pcMeet, &m), 0);
	EXPECT_TRUE(pt_waitFor(&m.started, 1));
	pt_set(&m.go, 1);
	ASSERT_EQ(pthread_join(t, NULL), 0);
	EXPECT_EQ(pt_get(&m.done), 1);
}

# define PC_N	8

typedef struct pc_slot
{
	int	in;
	int	out;
}	pc_slot;

static void	*pcTriple(void *arg)
{
	pc_slot	*s = arg;

	s->out = 3 * s->in;
	return (s);
}

/* Several threads at once, each with its own argument and exit status. */
TEST_OPT(several_threads, .ref = "XSH pthread_create()")
{
	pthread_t	t[PC_N];
	pc_slot		s[PC_N];
	int			made = 0;
	int			bad = 0;

	for (int i = 0; i < PC_N; i++)
	{
		s[i].in = i + 1;
		s[i].out = 0;
	}
	while (made < PC_N && pthread_create(&t[made], NULL, pcTriple, &s[made]) == 0)
		made++;
	for (int i = 0; i < made; i++)
	{
		void	*v = NULL;

		bad += pthread_join(t[i], &v) != 0 || v != &s[i] || s[i].out != 3 * (i + 1);
	}
	EXPECT_EQ(made, PC_N);
	EXPECT_EQ(bad, 0);
}

static void	*pcRaiseSelf(void *arg)
{
	(void)arg;
	/* Blocked (inherited mask): stays pending, dropped when the thread ends. */
	(void)lc_sysTgkill(lc_sysGetpid(), lc_sysGettid(), SIGUSR1);
	return (NULL);
}

static int	pcMaskChild(void *arg)
{
	pthread_t	t;

	(void)arg;
	if (lc_sysBlockSignal(SIGUSR1) != 0)
		return (2);
	if (pthread_create(&t, NULL, pcRaiseSelf, NULL) != 0)
		return (3);
	if (pthread_join(t, NULL) != 0)
		return (4);
	return (0);
}

/*
 * The signal mask is inherited: SIGUSR1, blocked by the creator, does not
 * kill the process when the new thread sends it to itself.
 */
TEST_OPT(inherits_the_signal_mask, .ref = "XSH pthread_create()")
{
	PT_FORK(st, pcMaskChild, NULL);
	EXPECT_EQ(st, 0);
}
