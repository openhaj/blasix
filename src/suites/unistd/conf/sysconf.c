/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file sysconf.c
 * @brief Tests for sysconf() (<unistd.h>).
 * @Created: 2026/11/28 10:55:22 by Moutig
 * @Updated: 2026/11/28 10:57:45 by Moutig
 *
 * sysconf(name) returns the current value of a configurable system limit
 * or option. For a limit described in <limits.h> as a maximum or a
 * minimum that has no limit: -1 without changing errno; otherwise the
 * value, no more restrictive than what <limits.h> promises (at least the
 * _POSIX_* / _POSIX2_* / _XOPEN_* minimum of the edition). The value
 * does not change during the process (but _SC_NPROCESSORS_ONLN). An
 * invalid name: -1 and EINVAL. An option whose constant is greater than
 * 0 at compilation is always supported at run time (XBD 2.1.6).
 *
 * The limits and their minimum values were read from XSH sysconf() and
 * XBD <limits.h> ("Minimum Values") of each edition; the page size is the
 * kernel's (AT_PAGESZ of /proc/self/auxv, without the libc).
 *
 * Clauses: POSIX: XSH sysconf(); XBD <limits.h>, 2.1.6.
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH sysconf()")
{
	EXPECT_TYPE(&sysconf, long (*)(int));
}

/** A limit: its sysconf() name, its variable, its minimum (-1: none). */
typedef struct s_limit
{
	const char	*var;
	int			name;
	long		min;
}	t_limit;

/* The limits of sysconf() the header names, with their minimum in the edition under test. */
static const t_limit	g_limits[] = {
#if _POSIX_C_SOURCE < 200809L
# ifdef _SC_AIO_LISTIO_MAX
	{"AIO_LISTIO_MAX", _SC_AIO_LISTIO_MAX, 2},
# endif
# ifdef _SC_AIO_MAX
	{"AIO_MAX", _SC_AIO_MAX, 1},
# endif
# ifdef _SC_AIO_PRIO_DELTA_MAX
	{"AIO_PRIO_DELTA_MAX", _SC_AIO_PRIO_DELTA_MAX, -1},
# endif
# ifdef _SC_ARG_MAX
	{"ARG_MAX", _SC_ARG_MAX, 4096},
# endif
# ifdef _SC_ATEXIT_MAX
	{"ATEXIT_MAX", _SC_ATEXIT_MAX, 32},
# endif
# ifdef _SC_BC_BASE_MAX
	{"BC_BASE_MAX", _SC_BC_BASE_MAX, 99},
# endif
# ifdef _SC_BC_DIM_MAX
	{"BC_DIM_MAX", _SC_BC_DIM_MAX, 2048},
# endif
# ifdef _SC_BC_SCALE_MAX
	{"BC_SCALE_MAX", _SC_BC_SCALE_MAX, 99},
# endif
# ifdef _SC_BC_STRING_MAX
	{"BC_STRING_MAX", _SC_BC_STRING_MAX, 1000},
# endif
# ifdef _SC_CHILD_MAX
	{"CHILD_MAX", _SC_CHILD_MAX, 25},
# endif
# ifdef _SC_COLL_WEIGHTS_MAX
	{"COLL_WEIGHTS_MAX", _SC_COLL_WEIGHTS_MAX, 2},
# endif
# ifdef _SC_DELAYTIMER_MAX
	{"DELAYTIMER_MAX", _SC_DELAYTIMER_MAX, 32},
# endif
# ifdef _SC_EXPR_NEST_MAX
	{"EXPR_NEST_MAX", _SC_EXPR_NEST_MAX, 32},
# endif
# ifdef _SC_HOST_NAME_MAX
	{"HOST_NAME_MAX", _SC_HOST_NAME_MAX, 255},
# endif
# ifdef _SC_IOV_MAX
	{"IOV_MAX", _SC_IOV_MAX, 16},
# endif
# ifdef _SC_LINE_MAX
	{"LINE_MAX", _SC_LINE_MAX, 2048},
# endif
# ifdef _SC_LOGIN_NAME_MAX
	{"LOGIN_NAME_MAX", _SC_LOGIN_NAME_MAX, 9},
# endif
# ifdef _SC_MQ_OPEN_MAX
	{"MQ_OPEN_MAX", _SC_MQ_OPEN_MAX, 8},
# endif
# ifdef _SC_MQ_PRIO_MAX
	{"MQ_PRIO_MAX", _SC_MQ_PRIO_MAX, 32},
# endif
# ifdef _SC_NGROUPS_MAX
	{"NGROUPS_MAX", _SC_NGROUPS_MAX, 8},
# endif
# ifdef _SC_OPEN_MAX
	{"OPEN_MAX", _SC_OPEN_MAX, 20},
# endif
# ifdef _SC_PAGESIZE
	{"PAGESIZE", _SC_PAGESIZE, 1},
# endif
# ifdef _SC_PAGE_SIZE
	{"PAGE_SIZE", _SC_PAGE_SIZE, 1},
# endif
# ifdef _SC_RE_DUP_MAX
	{"RE_DUP_MAX", _SC_RE_DUP_MAX, -1},
# endif
# ifdef _SC_RTSIG_MAX
	{"RTSIG_MAX", _SC_RTSIG_MAX, 8},
# endif
# ifdef _SC_SEM_NSEMS_MAX
	{"SEM_NSEMS_MAX", _SC_SEM_NSEMS_MAX, 256},
# endif
# ifdef _SC_SEM_VALUE_MAX
	{"SEM_VALUE_MAX", _SC_SEM_VALUE_MAX, 32767},
# endif
# ifdef _SC_SIGQUEUE_MAX
	{"SIGQUEUE_MAX", _SC_SIGQUEUE_MAX, 32},
# endif
# ifdef _SC_STREAM_MAX
	{"STREAM_MAX", _SC_STREAM_MAX, 8},
# endif
# ifdef _SC_SYMLOOP_MAX
	{"SYMLOOP_MAX", _SC_SYMLOOP_MAX, 8},
# endif
# ifdef _SC_THREAD_DESTRUCTOR_ITERATIONS
	{"PTHREAD_DESTRUCTOR_ITERATIONS", _SC_THREAD_DESTRUCTOR_ITERATIONS, 4},
# endif
# ifdef _SC_THREAD_KEYS_MAX
	{"PTHREAD_KEYS_MAX", _SC_THREAD_KEYS_MAX, 128},
# endif
# ifdef _SC_THREAD_STACK_MIN
	{"PTHREAD_STACK_MIN", _SC_THREAD_STACK_MIN, -1},
# endif
# ifdef _SC_THREAD_THREADS_MAX
	{"PTHREAD_THREADS_MAX", _SC_THREAD_THREADS_MAX, 64},
# endif
# ifdef _SC_TIMER_MAX
	{"TIMER_MAX", _SC_TIMER_MAX, 32},
# endif
# ifdef _SC_TTY_NAME_MAX
	{"TTY_NAME_MAX", _SC_TTY_NAME_MAX, 9},
# endif
# ifdef _SC_TZNAME_MAX
	{"TZNAME_MAX", _SC_TZNAME_MAX, 6},
# endif
#elif _POSIX_C_SOURCE >= 200809L && _POSIX_C_SOURCE < 202405L
# ifdef _SC_AIO_LISTIO_MAX
	{"AIO_LISTIO_MAX", _SC_AIO_LISTIO_MAX, 2},
# endif
# ifdef _SC_AIO_MAX
	{"AIO_MAX", _SC_AIO_MAX, 1},
# endif
# ifdef _SC_AIO_PRIO_DELTA_MAX
	{"AIO_PRIO_DELTA_MAX", _SC_AIO_PRIO_DELTA_MAX, -1},
# endif
# ifdef _SC_ARG_MAX
	{"ARG_MAX", _SC_ARG_MAX, 4096},
# endif
# ifdef _SC_ATEXIT_MAX
	{"ATEXIT_MAX", _SC_ATEXIT_MAX, 32},
# endif
# ifdef _SC_BC_BASE_MAX
	{"BC_BASE_MAX", _SC_BC_BASE_MAX, 99},
# endif
# ifdef _SC_BC_DIM_MAX
	{"BC_DIM_MAX", _SC_BC_DIM_MAX, 2048},
# endif
# ifdef _SC_BC_SCALE_MAX
	{"BC_SCALE_MAX", _SC_BC_SCALE_MAX, 99},
# endif
# ifdef _SC_BC_STRING_MAX
	{"BC_STRING_MAX", _SC_BC_STRING_MAX, 1000},
# endif
# ifdef _SC_CHILD_MAX
	{"CHILD_MAX", _SC_CHILD_MAX, 25},
# endif
# ifdef _SC_COLL_WEIGHTS_MAX
	{"COLL_WEIGHTS_MAX", _SC_COLL_WEIGHTS_MAX, 2},
# endif
# ifdef _SC_DELAYTIMER_MAX
	{"DELAYTIMER_MAX", _SC_DELAYTIMER_MAX, 32},
# endif
# ifdef _SC_EXPR_NEST_MAX
	{"EXPR_NEST_MAX", _SC_EXPR_NEST_MAX, 32},
# endif
# ifdef _SC_HOST_NAME_MAX
	{"HOST_NAME_MAX", _SC_HOST_NAME_MAX, 255},
# endif
# ifdef _SC_IOV_MAX
	{"IOV_MAX", _SC_IOV_MAX, 16},
# endif
# ifdef _SC_LINE_MAX
	{"LINE_MAX", _SC_LINE_MAX, 2048},
# endif
# ifdef _SC_LOGIN_NAME_MAX
	{"LOGIN_NAME_MAX", _SC_LOGIN_NAME_MAX, 9},
# endif
# ifdef _SC_MQ_OPEN_MAX
	{"MQ_OPEN_MAX", _SC_MQ_OPEN_MAX, 8},
# endif
# ifdef _SC_MQ_PRIO_MAX
	{"MQ_PRIO_MAX", _SC_MQ_PRIO_MAX, 32},
# endif
# ifdef _SC_NGROUPS_MAX
	{"NGROUPS_MAX", _SC_NGROUPS_MAX, 8},
# endif
# ifdef _SC_OPEN_MAX
	{"OPEN_MAX", _SC_OPEN_MAX, 20},
# endif
# ifdef _SC_PAGESIZE
	{"PAGESIZE", _SC_PAGESIZE, 1},
# endif
# ifdef _SC_PAGE_SIZE
	{"PAGE_SIZE", _SC_PAGE_SIZE, 1},
# endif
# ifdef _SC_RE_DUP_MAX
	{"RE_DUP_MAX", _SC_RE_DUP_MAX, -1},
# endif
# ifdef _SC_RTSIG_MAX
	{"RTSIG_MAX", _SC_RTSIG_MAX, 8},
# endif
# ifdef _SC_SEM_NSEMS_MAX
	{"SEM_NSEMS_MAX", _SC_SEM_NSEMS_MAX, 256},
# endif
# ifdef _SC_SEM_VALUE_MAX
	{"SEM_VALUE_MAX", _SC_SEM_VALUE_MAX, 32767},
# endif
# ifdef _SC_SIGQUEUE_MAX
	{"SIGQUEUE_MAX", _SC_SIGQUEUE_MAX, 32},
# endif
# ifdef _SC_STREAM_MAX
	{"STREAM_MAX", _SC_STREAM_MAX, 8},
# endif
# ifdef _SC_SYMLOOP_MAX
	{"SYMLOOP_MAX", _SC_SYMLOOP_MAX, 8},
# endif
# ifdef _SC_THREAD_DESTRUCTOR_ITERATIONS
	{"PTHREAD_DESTRUCTOR_ITERATIONS", _SC_THREAD_DESTRUCTOR_ITERATIONS, 4},
# endif
# ifdef _SC_THREAD_KEYS_MAX
	{"PTHREAD_KEYS_MAX", _SC_THREAD_KEYS_MAX, 128},
# endif
# ifdef _SC_THREAD_STACK_MIN
	{"PTHREAD_STACK_MIN", _SC_THREAD_STACK_MIN, -1},
# endif
# ifdef _SC_THREAD_THREADS_MAX
	{"PTHREAD_THREADS_MAX", _SC_THREAD_THREADS_MAX, 64},
# endif
# ifdef _SC_TIMER_MAX
	{"TIMER_MAX", _SC_TIMER_MAX, 32},
# endif
# ifdef _SC_TTY_NAME_MAX
	{"TTY_NAME_MAX", _SC_TTY_NAME_MAX, 9},
# endif
# ifdef _SC_TZNAME_MAX
	{"TZNAME_MAX", _SC_TZNAME_MAX, 6},
# endif
#elif _POSIX_C_SOURCE >= 202405L
# ifdef _SC_AIO_LISTIO_MAX
	{"AIO_LISTIO_MAX", _SC_AIO_LISTIO_MAX, 2},
# endif
# ifdef _SC_AIO_MAX
	{"AIO_MAX", _SC_AIO_MAX, 1},
# endif
# ifdef _SC_AIO_PRIO_DELTA_MAX
	{"AIO_PRIO_DELTA_MAX", _SC_AIO_PRIO_DELTA_MAX, -1},
# endif
# ifdef _SC_ARG_MAX
	{"ARG_MAX", _SC_ARG_MAX, 4096},
# endif
# ifdef _SC_ATEXIT_MAX
	{"ATEXIT_MAX", _SC_ATEXIT_MAX, 32},
# endif
# ifdef _SC_BC_BASE_MAX
	{"BC_BASE_MAX", _SC_BC_BASE_MAX, 99},
# endif
# ifdef _SC_BC_DIM_MAX
	{"BC_DIM_MAX", _SC_BC_DIM_MAX, 2048},
# endif
# ifdef _SC_BC_SCALE_MAX
	{"BC_SCALE_MAX", _SC_BC_SCALE_MAX, 99},
# endif
# ifdef _SC_BC_STRING_MAX
	{"BC_STRING_MAX", _SC_BC_STRING_MAX, 1000},
# endif
# ifdef _SC_CHILD_MAX
	{"CHILD_MAX", _SC_CHILD_MAX, 25},
# endif
# ifdef _SC_COLL_WEIGHTS_MAX
	{"COLL_WEIGHTS_MAX", _SC_COLL_WEIGHTS_MAX, 2},
# endif
# ifdef _SC_DELAYTIMER_MAX
	{"DELAYTIMER_MAX", _SC_DELAYTIMER_MAX, 32},
# endif
# ifdef _SC_EXPR_NEST_MAX
	{"EXPR_NEST_MAX", _SC_EXPR_NEST_MAX, 32},
# endif
# ifdef _SC_HOST_NAME_MAX
	{"HOST_NAME_MAX", _SC_HOST_NAME_MAX, 255},
# endif
# ifdef _SC_IOV_MAX
	{"IOV_MAX", _SC_IOV_MAX, 16},
# endif
# ifdef _SC_LINE_MAX
	{"LINE_MAX", _SC_LINE_MAX, 2048},
# endif
# ifdef _SC_LOGIN_NAME_MAX
	{"LOGIN_NAME_MAX", _SC_LOGIN_NAME_MAX, 9},
# endif
# ifdef _SC_MQ_OPEN_MAX
	{"MQ_OPEN_MAX", _SC_MQ_OPEN_MAX, 8},
# endif
# ifdef _SC_MQ_PRIO_MAX
	{"MQ_PRIO_MAX", _SC_MQ_PRIO_MAX, 32},
# endif
# ifdef _SC_NGROUPS_MAX
	{"NGROUPS_MAX", _SC_NGROUPS_MAX, 8},
# endif
# ifdef _SC_OPEN_MAX
	{"OPEN_MAX", _SC_OPEN_MAX, 20},
# endif
# ifdef _SC_PAGESIZE
	{"PAGESIZE", _SC_PAGESIZE, 1},
# endif
# ifdef _SC_PAGE_SIZE
	{"PAGE_SIZE", _SC_PAGE_SIZE, 1},
# endif
# ifdef _SC_RE_DUP_MAX
	{"RE_DUP_MAX", _SC_RE_DUP_MAX, 255},
# endif
# ifdef _SC_RTSIG_MAX
	{"RTSIG_MAX", _SC_RTSIG_MAX, 8},
# endif
# ifdef _SC_SEM_NSEMS_MAX
	{"SEM_NSEMS_MAX", _SC_SEM_NSEMS_MAX, 256},
# endif
# ifdef _SC_SEM_VALUE_MAX
	{"SEM_VALUE_MAX", _SC_SEM_VALUE_MAX, 32767},
# endif
# ifdef _SC_SIGQUEUE_MAX
	{"SIGQUEUE_MAX", _SC_SIGQUEUE_MAX, 32},
# endif
# ifdef _SC_STREAM_MAX
	{"STREAM_MAX", _SC_STREAM_MAX, 8},
# endif
# ifdef _SC_SYMLOOP_MAX
	{"SYMLOOP_MAX", _SC_SYMLOOP_MAX, 8},
# endif
# ifdef _SC_THREAD_DESTRUCTOR_ITERATIONS
	{"PTHREAD_DESTRUCTOR_ITERATIONS", _SC_THREAD_DESTRUCTOR_ITERATIONS, 4},
# endif
# ifdef _SC_THREAD_KEYS_MAX
	{"PTHREAD_KEYS_MAX", _SC_THREAD_KEYS_MAX, 128},
# endif
# ifdef _SC_THREAD_STACK_MIN
	{"PTHREAD_STACK_MIN", _SC_THREAD_STACK_MIN, -1},
# endif
# ifdef _SC_THREAD_THREADS_MAX
	{"PTHREAD_THREADS_MAX", _SC_THREAD_THREADS_MAX, 64},
# endif
# ifdef _SC_TIMER_MAX
	{"TIMER_MAX", _SC_TIMER_MAX, 32},
# endif
# ifdef _SC_TTY_NAME_MAX
	{"TTY_NAME_MAX", _SC_TTY_NAME_MAX, 9},
# endif
# ifdef _SC_TZNAME_MAX
	{"TZNAME_MAX", _SC_TZNAME_MAX, 6},
# endif
#endif
	{NULL, 0, 0}
};

/* Every limit: -1 (no limit, errno unchanged) or at least its minimum. */
TEST_OPT(limits_are_at_least_their_minimum, .ref = "XSH sysconf(); XBD <limits.h>")
{
	int	bad = 0;

	for (int i = 0; g_limits[i].var; i++)
	{
		long	v;

		errno = 4321;
		v = sysconf(g_limits[i].name);
		if (v == -1 ? errno != 4321 : (g_limits[i].min >= 0 ? v < g_limits[i].min : v < 0))
		{
			LOG(g_limits[i].var);
			bad++;
		}
	}
	EXPECT_EQ(bad, 0);
}

/* The values do not change during the process (_SC_NPROCESSORS_ONLN may). */
TEST_OPT(values_do_not_change, .ref = "XSH sysconf()")
{
	int	bad = 0;

	for (int i = 0; g_limits[i].var; i++)
		for (int k = 0; k < 3; k++)
			bad += sysconf(g_limits[i].name) != sysconf(g_limits[i].name);
	EXPECT_EQ(bad, 0);
}

/* {PAGESIZE} is the page size of the system (and {PAGE_SIZE}, XSI, the same). */
TEST_OPT(page_size_is_the_system_page_size, .ref = "XSH sysconf()")
{
	unsigned long	v[2];
	long			fd = lc_sysOpen("/proc/self/auxv", LC_O_RDONLY | LC_O_CLOEXEC, 0);
	long			page = 0;

	if (fd < 0)
		SKIP("no /proc/self/auxv");
	while (page == 0 && lc_sysRead((int)fd, v, sizeof(v)) == (long)sizeof(v) && v[0] != 0)
		if (v[0] == 6)
			page = (long)v[1];
	(void)lc_sysClose((int)fd);
	EXPECT_EQ(sysconf(_SC_PAGESIZE), page);
#ifdef _SC_PAGE_SIZE
	EXPECT_EQ(sysconf(_SC_PAGE_SIZE), page);
#endif
}

/* Clock ticks per second: positive. */
TEST_OPT(clock_ticks_are_positive, .ref = "XSH sysconf()")
{
	EXPECT_GT(sysconf(_SC_CLK_TCK), 0);
}

/* POSIX.1-2024: processors configured and online, at least one, online <= configured. */
TEST_OPT(processors_configured_and_online, .since = "POSIX.1-2024", .ref = "XSH sysconf() (Issue 8)")
{
#if defined(_SC_NPROCESSORS_CONF) && defined(_SC_NPROCESSORS_ONLN)
	long	conf = sysconf(_SC_NPROCESSORS_CONF);
	long	onln = sysconf(_SC_NPROCESSORS_ONLN);

	EXPECT_GE(conf, 1);
	EXPECT_GE(onln, 1);
	EXPECT_LE(onln, conf);
#else
	NOT_IMPLEMENTED("_SC_NPROCESSORS_CONF / _SC_NPROCESSORS_ONLN");
#endif
}

/*
 * An option whose constant is greater than 0 in <unistd.h> is always
 * supported at run time: sysconf() of it is greater than 0 too.
 */
TEST_OPT(options_supported_at_compilation_are_at_run_time, .ref = "XBD 2.1.6")
{
	int	bad = 0;

#define OPT_CHECK(c, n)	if ((c) > 0 && sysconf(n) <= 0) { LOG(#c); bad++; }
#if defined(_POSIX2_CHAR_TERM) && defined(_SC_2_CHAR_TERM)
	OPT_CHECK(_POSIX2_CHAR_TERM, _SC_2_CHAR_TERM)
#endif
#if defined(_POSIX2_C_BIND) && defined(_SC_2_C_BIND)
	OPT_CHECK(_POSIX2_C_BIND, _SC_2_C_BIND)
#endif
#if defined(_POSIX2_C_DEV) && defined(_SC_2_C_DEV)
	OPT_CHECK(_POSIX2_C_DEV, _SC_2_C_DEV)
#endif
#if defined(_POSIX2_FORT_DEV) && defined(_SC_2_FORT_DEV)
	OPT_CHECK(_POSIX2_FORT_DEV, _SC_2_FORT_DEV)
#endif
#if defined(_POSIX2_FORT_RUN) && defined(_SC_2_FORT_RUN)
	OPT_CHECK(_POSIX2_FORT_RUN, _SC_2_FORT_RUN)
#endif
#if defined(_POSIX2_LOCALEDEF) && defined(_SC_2_LOCALEDEF)
	OPT_CHECK(_POSIX2_LOCALEDEF, _SC_2_LOCALEDEF)
#endif
#if defined(_POSIX2_PBS) && defined(_SC_2_PBS)
	OPT_CHECK(_POSIX2_PBS, _SC_2_PBS)
#endif
#if defined(_POSIX2_PBS_ACCOUNTING) && defined(_SC_2_PBS_ACCOUNTING)
	OPT_CHECK(_POSIX2_PBS_ACCOUNTING, _SC_2_PBS_ACCOUNTING)
#endif
#if defined(_POSIX2_PBS_CHECKPOINT) && defined(_SC_2_PBS_CHECKPOINT)
	OPT_CHECK(_POSIX2_PBS_CHECKPOINT, _SC_2_PBS_CHECKPOINT)
#endif
#if defined(_POSIX2_PBS_LOCATE) && defined(_SC_2_PBS_LOCATE)
	OPT_CHECK(_POSIX2_PBS_LOCATE, _SC_2_PBS_LOCATE)
#endif
#if defined(_POSIX2_PBS_MESSAGE) && defined(_SC_2_PBS_MESSAGE)
	OPT_CHECK(_POSIX2_PBS_MESSAGE, _SC_2_PBS_MESSAGE)
#endif
#if defined(_POSIX2_PBS_TRACK) && defined(_SC_2_PBS_TRACK)
	OPT_CHECK(_POSIX2_PBS_TRACK, _SC_2_PBS_TRACK)
#endif
#if defined(_POSIX2_SW_DEV) && defined(_SC_2_SW_DEV)
	OPT_CHECK(_POSIX2_SW_DEV, _SC_2_SW_DEV)
#endif
#if defined(_POSIX2_UPE) && defined(_SC_2_UPE)
	OPT_CHECK(_POSIX2_UPE, _SC_2_UPE)
#endif
#if defined(_POSIX2_VERSION) && defined(_SC_2_VERSION)
	OPT_CHECK(_POSIX2_VERSION, _SC_2_VERSION)
#endif
#if defined(_POSIX_ADVISORY_INFO) && defined(_SC_ADVISORY_INFO)
	OPT_CHECK(_POSIX_ADVISORY_INFO, _SC_ADVISORY_INFO)
#endif
#if defined(_POSIX_ASYNCHRONOUS_IO) && defined(_SC_ASYNCHRONOUS_IO)
	OPT_CHECK(_POSIX_ASYNCHRONOUS_IO, _SC_ASYNCHRONOUS_IO)
#endif
#if defined(_POSIX_BARRIERS) && defined(_SC_BARRIERS)
	OPT_CHECK(_POSIX_BARRIERS, _SC_BARRIERS)
#endif
#if defined(_POSIX_CLOCK_SELECTION) && defined(_SC_CLOCK_SELECTION)
	OPT_CHECK(_POSIX_CLOCK_SELECTION, _SC_CLOCK_SELECTION)
#endif
#if defined(_POSIX_CPUTIME) && defined(_SC_CPUTIME)
	OPT_CHECK(_POSIX_CPUTIME, _SC_CPUTIME)
#endif
#if defined(_POSIX_DEVICE_CONTROL) && defined(_SC_DEVICE_CONTROL)
	OPT_CHECK(_POSIX_DEVICE_CONTROL, _SC_DEVICE_CONTROL)
#endif
#if defined(_POSIX_FSYNC) && defined(_SC_FSYNC)
	OPT_CHECK(_POSIX_FSYNC, _SC_FSYNC)
#endif
#if defined(_POSIX_IPV6) && defined(_SC_IPV6)
	OPT_CHECK(_POSIX_IPV6, _SC_IPV6)
#endif
#if defined(_POSIX_JOB_CONTROL) && defined(_SC_JOB_CONTROL)
	OPT_CHECK(_POSIX_JOB_CONTROL, _SC_JOB_CONTROL)
#endif
#if defined(_POSIX_MAPPED_FILES) && defined(_SC_MAPPED_FILES)
	OPT_CHECK(_POSIX_MAPPED_FILES, _SC_MAPPED_FILES)
#endif
#if defined(_POSIX_MEMLOCK) && defined(_SC_MEMLOCK)
	OPT_CHECK(_POSIX_MEMLOCK, _SC_MEMLOCK)
#endif
#if defined(_POSIX_MEMLOCK_RANGE) && defined(_SC_MEMLOCK_RANGE)
	OPT_CHECK(_POSIX_MEMLOCK_RANGE, _SC_MEMLOCK_RANGE)
#endif
#if defined(_POSIX_MEMORY_PROTECTION) && defined(_SC_MEMORY_PROTECTION)
	OPT_CHECK(_POSIX_MEMORY_PROTECTION, _SC_MEMORY_PROTECTION)
#endif
#if defined(_POSIX_MESSAGE_PASSING) && defined(_SC_MESSAGE_PASSING)
	OPT_CHECK(_POSIX_MESSAGE_PASSING, _SC_MESSAGE_PASSING)
#endif
#if defined(_POSIX_MONOTONIC_CLOCK) && defined(_SC_MONOTONIC_CLOCK)
	OPT_CHECK(_POSIX_MONOTONIC_CLOCK, _SC_MONOTONIC_CLOCK)
#endif
#if defined(_POSIX_PRIORITIZED_IO) && defined(_SC_PRIORITIZED_IO)
	OPT_CHECK(_POSIX_PRIORITIZED_IO, _SC_PRIORITIZED_IO)
#endif
#if defined(_POSIX_PRIORITY_SCHEDULING) && defined(_SC_PRIORITY_SCHEDULING)
	OPT_CHECK(_POSIX_PRIORITY_SCHEDULING, _SC_PRIORITY_SCHEDULING)
#endif
#if defined(_POSIX_RAW_SOCKETS) && defined(_SC_RAW_SOCKETS)
	OPT_CHECK(_POSIX_RAW_SOCKETS, _SC_RAW_SOCKETS)
#endif
#if defined(_POSIX_READER_WRITER_LOCKS) && defined(_SC_READER_WRITER_LOCKS)
	OPT_CHECK(_POSIX_READER_WRITER_LOCKS, _SC_READER_WRITER_LOCKS)
#endif
#if defined(_POSIX_REALTIME_SIGNALS) && defined(_SC_REALTIME_SIGNALS)
	OPT_CHECK(_POSIX_REALTIME_SIGNALS, _SC_REALTIME_SIGNALS)
#endif
#if defined(_POSIX_REGEXP) && defined(_SC_REGEXP)
	OPT_CHECK(_POSIX_REGEXP, _SC_REGEXP)
#endif
#if defined(_POSIX_SAVED_IDS) && defined(_SC_SAVED_IDS)
	OPT_CHECK(_POSIX_SAVED_IDS, _SC_SAVED_IDS)
#endif
#if defined(_POSIX_SEMAPHORES) && defined(_SC_SEMAPHORES)
	OPT_CHECK(_POSIX_SEMAPHORES, _SC_SEMAPHORES)
#endif
#if defined(_POSIX_SHARED_MEMORY_OBJECTS) && defined(_SC_SHARED_MEMORY_OBJECTS)
	OPT_CHECK(_POSIX_SHARED_MEMORY_OBJECTS, _SC_SHARED_MEMORY_OBJECTS)
#endif
#if defined(_POSIX_SHELL) && defined(_SC_SHELL)
	OPT_CHECK(_POSIX_SHELL, _SC_SHELL)
#endif
#if defined(_POSIX_SPAWN) && defined(_SC_SPAWN)
	OPT_CHECK(_POSIX_SPAWN, _SC_SPAWN)
#endif
#if defined(_POSIX_SPIN_LOCKS) && defined(_SC_SPIN_LOCKS)
	OPT_CHECK(_POSIX_SPIN_LOCKS, _SC_SPIN_LOCKS)
#endif
#if defined(_POSIX_SPORADIC_SERVER) && defined(_SC_SPORADIC_SERVER)
	OPT_CHECK(_POSIX_SPORADIC_SERVER, _SC_SPORADIC_SERVER)
#endif
#if defined(_POSIX_SS_REPL_MAX) && defined(_SC_SS_REPL_MAX)
	OPT_CHECK(_POSIX_SS_REPL_MAX, _SC_SS_REPL_MAX)
#endif
#if defined(_POSIX_SYNCHRONIZED_IO) && defined(_SC_SYNCHRONIZED_IO)
	OPT_CHECK(_POSIX_SYNCHRONIZED_IO, _SC_SYNCHRONIZED_IO)
#endif
#if defined(_POSIX_THREADS) && defined(_SC_THREADS)
	OPT_CHECK(_POSIX_THREADS, _SC_THREADS)
#endif
#if defined(_POSIX_THREAD_ATTR_STACKADDR) && defined(_SC_THREAD_ATTR_STACKADDR)
	OPT_CHECK(_POSIX_THREAD_ATTR_STACKADDR, _SC_THREAD_ATTR_STACKADDR)
#endif
#if defined(_POSIX_THREAD_ATTR_STACKSIZE) && defined(_SC_THREAD_ATTR_STACKSIZE)
	OPT_CHECK(_POSIX_THREAD_ATTR_STACKSIZE, _SC_THREAD_ATTR_STACKSIZE)
#endif
#if defined(_POSIX_THREAD_CPUTIME) && defined(_SC_THREAD_CPUTIME)
	OPT_CHECK(_POSIX_THREAD_CPUTIME, _SC_THREAD_CPUTIME)
#endif
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING) && defined(_SC_THREAD_PRIORITY_SCHEDULING)
	OPT_CHECK(_POSIX_THREAD_PRIORITY_SCHEDULING, _SC_THREAD_PRIORITY_SCHEDULING)
#endif
#if defined(_POSIX_THREAD_PRIO_INHERIT) && defined(_SC_THREAD_PRIO_INHERIT)
	OPT_CHECK(_POSIX_THREAD_PRIO_INHERIT, _SC_THREAD_PRIO_INHERIT)
#endif
#if defined(_POSIX_THREAD_PRIO_PROTECT) && defined(_SC_THREAD_PRIO_PROTECT)
	OPT_CHECK(_POSIX_THREAD_PRIO_PROTECT, _SC_THREAD_PRIO_PROTECT)
#endif
#if defined(_POSIX_THREAD_PROCESS_SHARED) && defined(_SC_THREAD_PROCESS_SHARED)
	OPT_CHECK(_POSIX_THREAD_PROCESS_SHARED, _SC_THREAD_PROCESS_SHARED)
#endif
#if defined(_POSIX_THREAD_ROBUST_PRIO_INHERIT) && defined(_SC_THREAD_ROBUST_PRIO_INHERIT)
	OPT_CHECK(_POSIX_THREAD_ROBUST_PRIO_INHERIT, _SC_THREAD_ROBUST_PRIO_INHERIT)
#endif
#if defined(_POSIX_THREAD_ROBUST_PRIO_PROTECT) && defined(_SC_THREAD_ROBUST_PRIO_PROTECT)
	OPT_CHECK(_POSIX_THREAD_ROBUST_PRIO_PROTECT, _SC_THREAD_ROBUST_PRIO_PROTECT)
#endif
#if defined(_POSIX_THREAD_SAFE_FUNCTIONS) && defined(_SC_THREAD_SAFE_FUNCTIONS)
	OPT_CHECK(_POSIX_THREAD_SAFE_FUNCTIONS, _SC_THREAD_SAFE_FUNCTIONS)
#endif
#if defined(_POSIX_THREAD_SPORADIC_SERVER) && defined(_SC_THREAD_SPORADIC_SERVER)
	OPT_CHECK(_POSIX_THREAD_SPORADIC_SERVER, _SC_THREAD_SPORADIC_SERVER)
#endif
#if defined(_POSIX_TIMEOUTS) && defined(_SC_TIMEOUTS)
	OPT_CHECK(_POSIX_TIMEOUTS, _SC_TIMEOUTS)
#endif
#if defined(_POSIX_TIMERS) && defined(_SC_TIMERS)
	OPT_CHECK(_POSIX_TIMERS, _SC_TIMERS)
#endif
#if defined(_POSIX_TRACE) && defined(_SC_TRACE)
	OPT_CHECK(_POSIX_TRACE, _SC_TRACE)
#endif
#if defined(_POSIX_TRACE_EVENT_FILTER) && defined(_SC_TRACE_EVENT_FILTER)
	OPT_CHECK(_POSIX_TRACE_EVENT_FILTER, _SC_TRACE_EVENT_FILTER)
#endif
#if defined(_POSIX_TRACE_EVENT_NAME_MAX) && defined(_SC_TRACE_EVENT_NAME_MAX)
	OPT_CHECK(_POSIX_TRACE_EVENT_NAME_MAX, _SC_TRACE_EVENT_NAME_MAX)
#endif
#if defined(_POSIX_TRACE_INHERIT) && defined(_SC_TRACE_INHERIT)
	OPT_CHECK(_POSIX_TRACE_INHERIT, _SC_TRACE_INHERIT)
#endif
#if defined(_POSIX_TRACE_LOG) && defined(_SC_TRACE_LOG)
	OPT_CHECK(_POSIX_TRACE_LOG, _SC_TRACE_LOG)
#endif
#if defined(_POSIX_TRACE_NAME_MAX) && defined(_SC_TRACE_NAME_MAX)
	OPT_CHECK(_POSIX_TRACE_NAME_MAX, _SC_TRACE_NAME_MAX)
#endif
#if defined(_POSIX_TRACE_SYS_MAX) && defined(_SC_TRACE_SYS_MAX)
	OPT_CHECK(_POSIX_TRACE_SYS_MAX, _SC_TRACE_SYS_MAX)
#endif
#if defined(_POSIX_TRACE_USER_EVENT_MAX) && defined(_SC_TRACE_USER_EVENT_MAX)
	OPT_CHECK(_POSIX_TRACE_USER_EVENT_MAX, _SC_TRACE_USER_EVENT_MAX)
#endif
#if defined(_POSIX_TYPED_MEMORY_OBJECTS) && defined(_SC_TYPED_MEMORY_OBJECTS)
	OPT_CHECK(_POSIX_TYPED_MEMORY_OBJECTS, _SC_TYPED_MEMORY_OBJECTS)
#endif
#if defined(_POSIX_V6_ILP32_OFF32) && defined(_SC_V6_ILP32_OFF32)
	OPT_CHECK(_POSIX_V6_ILP32_OFF32, _SC_V6_ILP32_OFF32)
#endif
#if defined(_POSIX_V6_ILP32_OFFBIG) && defined(_SC_V6_ILP32_OFFBIG)
	OPT_CHECK(_POSIX_V6_ILP32_OFFBIG, _SC_V6_ILP32_OFFBIG)
#endif
#if defined(_POSIX_V6_LP64_OFF64) && defined(_SC_V6_LP64_OFF64)
	OPT_CHECK(_POSIX_V6_LP64_OFF64, _SC_V6_LP64_OFF64)
#endif
#if defined(_POSIX_V6_LPBIG_OFFBIG) && defined(_SC_V6_LPBIG_OFFBIG)
	OPT_CHECK(_POSIX_V6_LPBIG_OFFBIG, _SC_V6_LPBIG_OFFBIG)
#endif
#if defined(_POSIX_V7_ILP32_OFF32) && defined(_SC_V7_ILP32_OFF32)
	OPT_CHECK(_POSIX_V7_ILP32_OFF32, _SC_V7_ILP32_OFF32)
#endif
#if defined(_POSIX_V7_ILP32_OFFBIG) && defined(_SC_V7_ILP32_OFFBIG)
	OPT_CHECK(_POSIX_V7_ILP32_OFFBIG, _SC_V7_ILP32_OFFBIG)
#endif
#if defined(_POSIX_V7_LP64_OFF64) && defined(_SC_V7_LP64_OFF64)
	OPT_CHECK(_POSIX_V7_LP64_OFF64, _SC_V7_LP64_OFF64)
#endif
#if defined(_POSIX_V7_LPBIG_OFFBIG) && defined(_SC_V7_LPBIG_OFFBIG)
	OPT_CHECK(_POSIX_V7_LPBIG_OFFBIG, _SC_V7_LPBIG_OFFBIG)
#endif
#if defined(_POSIX_V8_ILP32_OFF32) && defined(_SC_V8_ILP32_OFF32)
	OPT_CHECK(_POSIX_V8_ILP32_OFF32, _SC_V8_ILP32_OFF32)
#endif
#if defined(_POSIX_V8_ILP32_OFFBIG) && defined(_SC_V8_ILP32_OFFBIG)
	OPT_CHECK(_POSIX_V8_ILP32_OFFBIG, _SC_V8_ILP32_OFFBIG)
#endif
#if defined(_POSIX_V8_LP64_OFF64) && defined(_SC_V8_LP64_OFF64)
	OPT_CHECK(_POSIX_V8_LP64_OFF64, _SC_V8_LP64_OFF64)
#endif
#if defined(_POSIX_V8_LPBIG_OFFBIG) && defined(_SC_V8_LPBIG_OFFBIG)
	OPT_CHECK(_POSIX_V8_LPBIG_OFFBIG, _SC_V8_LPBIG_OFFBIG)
#endif
#if defined(_POSIX_VERSION) && defined(_SC_VERSION)
	OPT_CHECK(_POSIX_VERSION, _SC_VERSION)
#endif
#if defined(_XOPEN_CRYPT) && defined(_SC_XOPEN_CRYPT)
	OPT_CHECK(_XOPEN_CRYPT, _SC_XOPEN_CRYPT)
#endif
#if defined(_XOPEN_LEGACY) && defined(_SC_XOPEN_LEGACY)
	OPT_CHECK(_XOPEN_LEGACY, _SC_XOPEN_LEGACY)
#endif
#if defined(_XOPEN_REALTIME) && defined(_SC_XOPEN_REALTIME)
	OPT_CHECK(_XOPEN_REALTIME, _SC_XOPEN_REALTIME)
#endif
#if defined(_XOPEN_REALTIME_THREADS) && defined(_SC_XOPEN_REALTIME_THREADS)
	OPT_CHECK(_XOPEN_REALTIME_THREADS, _SC_XOPEN_REALTIME_THREADS)
#endif
#if defined(_XOPEN_SHM) && defined(_SC_XOPEN_SHM)
	OPT_CHECK(_XOPEN_SHM, _SC_XOPEN_SHM)
#endif
#if defined(_XOPEN_STREAMS) && defined(_SC_XOPEN_STREAMS)
	OPT_CHECK(_XOPEN_STREAMS, _SC_XOPEN_STREAMS)
#endif
#if defined(_XOPEN_UNIX) && defined(_SC_XOPEN_UNIX)
	OPT_CHECK(_XOPEN_UNIX, _SC_XOPEN_UNIX)
#endif
#if defined(_XOPEN_UUCP) && defined(_SC_XOPEN_UUCP)
	OPT_CHECK(_XOPEN_UUCP, _SC_XOPEN_UUCP)
#endif
#if defined(_XOPEN_VERSION) && defined(_SC_XOPEN_VERSION)
	OPT_CHECK(_XOPEN_VERSION, _SC_XOPEN_VERSION)
#endif
#undef OPT_CHECK
	EXPECT_EQ(bad, 0);
}

/* Not more restrictive than at compilation: _SC_VERSION at least _POSIX_VERSION. */
TEST_OPT(version_not_below_the_header, .ref = "XSH sysconf()")
{
	EXPECT_GE(sysconf(_SC_VERSION), _POSIX_VERSION);
}

/* A name that is none of the _SC_* values: -1 and EINVAL. */
TEST_OPT(invalid_name_gives_einval, .ref = "XSH sysconf() ERRORS")
{
	static const int	bad[] = {-1, -12345, 99999, 0x7fffffff};

	for (int i = 0; i < 4; i++)
	{
		errno = 0;
		EXPECT_EQ(sysconf(bad[i]), -1);
		EXPECT_EQ(errno, EINVAL);
	}
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(sysconf(_SC_PAGESIZE), (sysconf)(_SC_PAGESIZE));
}
