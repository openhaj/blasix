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
 * @Updated: 2026/11/28 18:59:41 by Moutig
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
 *     and join it when it is joinable;
 *   - pt_mutexOfType: initialize a mutex of a given type;
 *   - pt_holdStart, pt_holdEnd: another thread locks a mutex and keeps it
 *     until told to unlock it; pt_tryOther: pthread_mutex_trylock() from
 *     another thread (unlocked again when it succeeds);
 *   - pt_absReal, pt_absMono: an absolute time n nanoseconds from now on
 *     CLOCK_REALTIME or CLOCK_MONOTONIC, for the timed functions;
 *   - pt_cv, pt_cvInit, pt_cvWaiter, pt_cvBlocked: a mutex and a condition
 *     variable, threads that each wait once on it (with a deadline, so a
 *     broken libc cannot hang the case) and count how many were woken,
 *     and a way to know they are all blocked in the wait;
 *   - pt_nsOf: a struct timespec in nanoseconds.
 *
 * The time and the waits are raw syscalls (lc_now, lc_sleep).
 */

#ifndef PTHREAD_CASES_H
# define PTHREAD_CASES_H

# include "framework/lc.h"
# include "framework/sys.h"
# include <errno.h>
# include <limits.h>
# include <pthread.h>
# include <time.h>
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

/* Initialize *m as a mutex of type @p type. @return 0, or an error number. */
static inline int	pt_mutexOfType(pthread_mutex_t *m, int type)
{
	pthread_mutexattr_t	a;
	int					e;

	e = pthread_mutexattr_init(&a);
	if (e != 0)
		return (e);
	e = pthread_mutexattr_settype(&a, type);
	if (e == 0)
		e = pthread_mutex_init(m, &a);
	(void)pthread_mutexattr_destroy(&a);
	return (e);
}

/* Another thread that owns a mutex until pt_holdEnd. */
typedef struct pt_hold
{
	pthread_mutex_t	*m;
	pthread_t		t;
	int				held;		/* Set once the lock call has returned. */
	int				release;	/* Set by pt_holdEnd. */
	int				ret;		/* What pthread_mutex_lock() returned. */
}	pt_hold;

static inline void	*pt_holdBody(void *arg)
{
	pt_hold	*h = arg;

	h->ret = pthread_mutex_lock(h->m);
	pt_set(&h->held, 1);
	(void)pt_waitFor(&h->release, 1);
	if (h->ret == 0)
		(void)pthread_mutex_unlock(h->m);
	return (NULL);
}

/*
 * Start a thread that locks *m and keeps it.
 *
 * @return 0 once it owns the mutex, its lock error, or -1.
 */
static inline int	pt_holdStart(pt_hold *h, pthread_mutex_t *m)
{
	lc_fill(h, 0, sizeof(*h));
	h->m = m;
	if (pthread_create(&h->t, NULL, pt_holdBody, h) != 0)
		return (-1);
	if (!pt_waitFor(&h->held, 1))
		return (-1);
	return (h->ret);
}

/* Let the holder unlock and end; join it. */
static inline void	pt_holdEnd(pt_hold *h)
{
	pt_set(&h->release, 1);
	(void)pthread_join(h->t, NULL);
}

/* What pt_tryOther hands its thread. */
typedef struct pt_try
{
	pthread_mutex_t	*m;
	int				e;
}	pt_try;

static inline void	*pt_tryBody(void *arg)
{
	pt_try	*a = arg;

	a->e = pthread_mutex_trylock(a->m);
	if (a->e == 0)
		(void)pthread_mutex_unlock(a->m);
	return (NULL);
}

/* @return What pthread_mutex_trylock(m) gives in another thread, or -1. */
static inline int	pt_tryOther(pthread_mutex_t *m)
{
	pt_try		a;
	pthread_t	t;

	a.m = m;
	a.e = -1;
	if (pthread_create(&t, NULL, pt_tryBody, &a) != 0 || pthread_join(t, NULL) != 0)
		return (-1);
	return (a.e);
}

static inline struct timespec	pt_absFrom(long long now, long long ns)
{
	struct timespec	ts;
	long long		at = now + ns;

	ts.tv_sec = (time_t)(at / LC_NS_PER_SEC);
	ts.tv_nsec = (long)(at % LC_NS_PER_SEC);
	return (ts);
}

/* @return CLOCK_REALTIME now + ns (ns may be negative: the past). */
static inline struct timespec	pt_absReal(long long ns)
{
	return (pt_absFrom(lc_nowReal(), ns));
}

/* @return CLOCK_MONOTONIC now + ns. */
static inline struct timespec	pt_absMono(long long ns)
{
	return (pt_absFrom(lc_now(), ns));
}

/* @return ts in nanoseconds. */
static inline long long	pt_nsOf(const struct timespec *ts)
{
	return ((long long)ts->tv_sec * LC_NS_PER_SEC + ts->tv_nsec);
}

/* A condition variable, its mutex and what its waiters saw. */
typedef struct pt_cv
{
	pthread_mutex_t	m;
	pthread_cond_t	c;
	long long		waitNs;		/* How long each waiter waits at most. */
	int				waiting;	/* Waiters that reached the wait (under m). */
	int				woken;		/* Waits that returned 0. */
	int				owned;		/* Of those, the ones that owned m on return. */
	int				done;		/* Waiters that have finished. */
}	pt_cv;

/* @return 0, or an error number. */
static inline int	pt_cvInit(pt_cv *v, long long waitNs)
{
	int	e;

	lc_fill(v, 0, sizeof(*v));
	v->waitNs = waitNs;
	e = pthread_mutex_init(&v->m, NULL);
	if (e == 0)
		e = pthread_cond_init(&v->c, NULL);
	return (e);
}

/*
 * One wait on v->c (pthread_cond_timedwait(), deadline v->waitNs on
 * CLOCK_REALTIME): counts it in woken when it returns 0, and in owned when
 * the waiter then owns the mutex (another thread's trylock is EBUSY).
 */
static inline void	*pt_cvWaiter(void *arg)
{
	pt_cv			*v = arg;
	struct timespec	at = pt_absReal(v->waitNs);

	if (pthread_mutex_lock(&v->m) != 0)
		return (NULL);
	(void)pt_add(&v->waiting, 1);
	if (pthread_cond_timedwait(&v->c, &v->m, &at) == 0)
	{
		(void)pt_add(&v->woken, 1);
		if (pt_tryOther(&v->m) == EBUSY)
			(void)pt_add(&v->owned, 1);
	}
	(void)pthread_mutex_unlock(&v->m);
	(void)pt_add(&v->done, 1);
	return (NULL);
}

/*
 * Wait until n waiters have reached the wait, then lock and unlock the
 * mutex: each released it by blocking, so all of them are blocked.
 *
 * @return 1, or 0 on timeout.
 */
static inline int	pt_cvBlocked(pt_cv *v, int n)
{
	if (!pt_waitFor(&v->waiting, n))
		return (0);
	if (pthread_mutex_lock(&v->m) != 0)
		return (0);
	(void)pthread_mutex_unlock(&v->m);
	return (1);
}

#endif
