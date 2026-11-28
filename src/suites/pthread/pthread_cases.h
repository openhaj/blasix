/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pthread_cases.h
 * @brief Shared helpers of the <pthread.h> units.
 * @Created: 2026/11/28 17:45:51 by Moutig
 * @Updated: 2026/11/28 18:03:13 by Moutig
 *
 * The threads of a case talk through plain int flags, read and written
 * with the compiler's atomic builtins (no libc, no data race):
 *
 *   - pt_get, pt_set, pt_add: load, store, add to a flag;
 *   - pt_waitFor: wait until a flag holds a value, with a deadline
 *     (PT_WAIT_NS), so that a broken libc fails the case instead of
 *     hanging it;
 *   - pt_fork, PT_FORK: run a function in the child of the libc's fork()
 *     and give its exit status, for what ends a process (pthread_exit()
 *     from main, the last thread exiting). The libc's fork(), not a raw
 *     one: the child creates threads, so the libc must know it is a new
 *     process. Checks in the child do not count: the child gives its
 *     result as its exit status;
 *   - pt_shared: memory still shared with such a child;
 *   - pt_stackMin, pt_pageSize: {PTHREAD_STACK_MIN} (<limits.h>, or
 *     sysconf() when the header leaves it out) and {PAGESIZE};
 *   - pt_runWith: create a thread with an attributes object, wait until
 *     it has run (a flag, so that it also works for a detached thread)
 *     and join it when it is joinable.
 *
 * The time and the waits are raw syscalls (lc_now, lc_sleep).
 */

#ifndef PTHREAD_CASES_H
# define PTHREAD_CASES_H

# include "framework/lc.h"
# include "framework/sys.h"
# include <limits.h>
# include <pthread.h>
# include <unistd.h>

/** Longest wait for another thread (a working libc takes microseconds). */
# define PT_WAIT_NS	(5 * LC_NS_PER_SEC)

static inline int	pt_get(const int *p)
{
	return (__atomic_load_n(p, __ATOMIC_SEQ_CST));
}

static inline void	pt_set(int *p, int v)
{
	__atomic_store_n(p, v, __ATOMIC_SEQ_CST);
}

/* @return The new value. */
static inline int	pt_add(int *p, int v)
{
	return (__atomic_add_fetch(p, v, __ATOMIC_SEQ_CST));
}

/*
 * Wait until *p == want, at most PT_WAIT_NS.
 *
 * @return 1 when it does, 0 on timeout.
 */
static inline int	pt_waitFor(const int *p, int want)
{
	long long	end = lc_now() + PT_WAIT_NS;

	while (pt_get(p) != want)
	{
		if (lc_now() > end)
			return (0);
		lc_sleep(100000);
	}
	return (1);
}

/*
 * Run body(arg) in the child of the libc's fork(); the child ends with
 * the value body returns (lc_exitNow: no atexit handler), unless body
 * ends the process itself.
 *
 * @return The child's exit status (0 to 255), 256 + the signal that
 *         killed it, -2 when fork() or the wait failed.
 */
static inline int	pt_fork(int (*body)(void *), void *arg)
{
	long	pid;
	int		st = 0;

	pid = (long)fork();
	if (pid < 0)
		return (-2);
	if (pid == 0)
		lc_exitNow(body(arg));
	if (lc_sysWait4(pid, &st, 0) != pid)
		return (-2);
	if ((st & 0x7f) == 0)
		return ((st >> 8) & 0xff);
	return (256 + (st & 0x7f));
}

/* The exit status of body(arg) in a child of fork(), or stop the case. */
# define PT_FORK(st, body, arg)													\
	int	st = pt_fork((body), (arg));											\
	ASSERT_NE(st, -2)

/* @return Zeroed memory shared with the children of fork(), or NULL. */
static inline void	*pt_shared(unsigned long len)
{
	return (lc_sysMmapShared(len));
}

/* @return {PTHREAD_STACK_MIN}. */
static inline unsigned long	pt_stackMin(void)
{
# ifdef PTHREAD_STACK_MIN
	return ((unsigned long)PTHREAD_STACK_MIN);
# else
	return ((unsigned long)sysconf(_SC_THREAD_STACK_MIN));
# endif
}

/* @return {PAGESIZE}. */
static inline unsigned long	pt_pageSize(void)
{
	return ((unsigned long)sysconf(_SC_PAGESIZE));
}

/* What pt_runWith gives its thread and gets back. */
typedef struct pt_run
{
	void	*(*fn)(void *);	/* Run by the thread (may be NULL). */
	void	*arg;			/* Its argument. */
	void	*ret;			/* What it returned. */
	int		done;			/* Set once fn has returned. */
}	pt_run;

static inline void	*pt_runBody(void *arg)
{
	pt_run	*r = arg;

	r->ret = r->fn ? r->fn(r->arg) : NULL;
	pt_set(&r->done, 1);
	return (r->ret);
}

/*
 * Run fn(arg) in a thread created with @p attr; wait until it has run and,
 * when @p join, join it.
 *
 * @return pthread_create()'s result, or -1 when the thread did not run
 *         in time or the join failed.
 */
static inline int	pt_runWith(const pthread_attr_t *attr, int join, pt_run *r)
{
	pthread_t	t;
	int			e;

	r->done = 0;
	r->ret = NULL;
	e = pthread_create(&t, attr, pt_runBody, r);
	if (e != 0)
		return (e);
	if (!pt_waitFor(&r->done, 1))
		return (-1);
	if (join && pthread_join(t, NULL) != 0)
		return (-1);
	return (0);
}

#endif
