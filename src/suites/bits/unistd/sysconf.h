/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file sysconf.h
 * @brief Cases of the sysconf() names of <unistd.h> (_SC_*).
 * @Created: 2026/11/28 09:36:00 by Moutig
 * @Updated: 2026/11/28 09:37:33 by Moutig
 *
 * The _SC_* symbolic constants name what sysconf() reports (XBD
 * <unistd.h>, "Constants for Functions"): each one must name something
 * of its own, so its value differs from every other _SC_* name the
 * header defines; _SC_PAGE_SIZE is a synonym of _SC_PAGESIZE (XSH
 * sysconf(): {PAGE_SIZE} and {PAGESIZE} are one variable) and may share
 * its value. What sysconf() answers is tested with sysconf().
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_UNISTD_SYSCONF_H
# define BITS_UNISTD_SYSCONF_H

# include "framework/lc.h"

/* The _SC_* names the header defines, to compare them. */
static const long	bitsSysconfNames[] = {
#  ifdef _SC_2_CHAR_TERM
	(long)_SC_2_CHAR_TERM,
#  endif
#  ifdef _SC_2_C_BIND
	(long)_SC_2_C_BIND,
#  endif
#  ifdef _SC_2_C_DEV
	(long)_SC_2_C_DEV,
#  endif
#  ifdef _SC_2_FORT_DEV
	(long)_SC_2_FORT_DEV,
#  endif
#  ifdef _SC_2_FORT_RUN
	(long)_SC_2_FORT_RUN,
#  endif
#  ifdef _SC_2_LOCALEDEF
	(long)_SC_2_LOCALEDEF,
#  endif
#  ifdef _SC_2_PBS
	(long)_SC_2_PBS,
#  endif
#  ifdef _SC_2_PBS_ACCOUNTING
	(long)_SC_2_PBS_ACCOUNTING,
#  endif
#  ifdef _SC_2_PBS_CHECKPOINT
	(long)_SC_2_PBS_CHECKPOINT,
#  endif
#  ifdef _SC_2_PBS_LOCATE
	(long)_SC_2_PBS_LOCATE,
#  endif
#  ifdef _SC_2_PBS_MESSAGE
	(long)_SC_2_PBS_MESSAGE,
#  endif
#  ifdef _SC_2_PBS_TRACK
	(long)_SC_2_PBS_TRACK,
#  endif
#  ifdef _SC_2_SW_DEV
	(long)_SC_2_SW_DEV,
#  endif
#  ifdef _SC_2_UPE
	(long)_SC_2_UPE,
#  endif
#  ifdef _SC_2_VERSION
	(long)_SC_2_VERSION,
#  endif
#  ifdef _SC_ADVISORY_INFO
	(long)_SC_ADVISORY_INFO,
#  endif
#  ifdef _SC_AIO_LISTIO_MAX
	(long)_SC_AIO_LISTIO_MAX,
#  endif
#  ifdef _SC_AIO_MAX
	(long)_SC_AIO_MAX,
#  endif
#  ifdef _SC_AIO_PRIO_DELTA_MAX
	(long)_SC_AIO_PRIO_DELTA_MAX,
#  endif
#  ifdef _SC_ARG_MAX
	(long)_SC_ARG_MAX,
#  endif
#  ifdef _SC_ASYNCHRONOUS_IO
	(long)_SC_ASYNCHRONOUS_IO,
#  endif
#  ifdef _SC_ATEXIT_MAX
	(long)_SC_ATEXIT_MAX,
#  endif
#  ifdef _SC_BARRIERS
	(long)_SC_BARRIERS,
#  endif
#  ifdef _SC_BC_BASE_MAX
	(long)_SC_BC_BASE_MAX,
#  endif
#  ifdef _SC_BC_DIM_MAX
	(long)_SC_BC_DIM_MAX,
#  endif
#  ifdef _SC_BC_SCALE_MAX
	(long)_SC_BC_SCALE_MAX,
#  endif
#  ifdef _SC_BC_STRING_MAX
	(long)_SC_BC_STRING_MAX,
#  endif
#  ifdef _SC_CHILD_MAX
	(long)_SC_CHILD_MAX,
#  endif
#  ifdef _SC_CLK_TCK
	(long)_SC_CLK_TCK,
#  endif
#  ifdef _SC_CLOCK_SELECTION
	(long)_SC_CLOCK_SELECTION,
#  endif
#  ifdef _SC_COLL_WEIGHTS_MAX
	(long)_SC_COLL_WEIGHTS_MAX,
#  endif
#  ifdef _SC_CPUTIME
	(long)_SC_CPUTIME,
#  endif
#  ifdef _SC_DELAYTIMER_MAX
	(long)_SC_DELAYTIMER_MAX,
#  endif
#  ifdef _SC_DEVICE_CONTROL
	(long)_SC_DEVICE_CONTROL,
#  endif
#  ifdef _SC_EXPR_NEST_MAX
	(long)_SC_EXPR_NEST_MAX,
#  endif
#  ifdef _SC_FSYNC
	(long)_SC_FSYNC,
#  endif
#  ifdef _SC_GETGR_R_SIZE_MAX
	(long)_SC_GETGR_R_SIZE_MAX,
#  endif
#  ifdef _SC_GETPW_R_SIZE_MAX
	(long)_SC_GETPW_R_SIZE_MAX,
#  endif
#  ifdef _SC_HOST_NAME_MAX
	(long)_SC_HOST_NAME_MAX,
#  endif
#  ifdef _SC_IOV_MAX
	(long)_SC_IOV_MAX,
#  endif
#  ifdef _SC_IPV6
	(long)_SC_IPV6,
#  endif
#  ifdef _SC_JOB_CONTROL
	(long)_SC_JOB_CONTROL,
#  endif
#  ifdef _SC_LINE_MAX
	(long)_SC_LINE_MAX,
#  endif
#  ifdef _SC_LOGIN_NAME_MAX
	(long)_SC_LOGIN_NAME_MAX,
#  endif
#  ifdef _SC_MAPPED_FILES
	(long)_SC_MAPPED_FILES,
#  endif
#  ifdef _SC_MEMLOCK
	(long)_SC_MEMLOCK,
#  endif
#  ifdef _SC_MEMLOCK_RANGE
	(long)_SC_MEMLOCK_RANGE,
#  endif
#  ifdef _SC_MEMORY_PROTECTION
	(long)_SC_MEMORY_PROTECTION,
#  endif
#  ifdef _SC_MESSAGE_PASSING
	(long)_SC_MESSAGE_PASSING,
#  endif
#  ifdef _SC_MONOTONIC_CLOCK
	(long)_SC_MONOTONIC_CLOCK,
#  endif
#  ifdef _SC_MQ_OPEN_MAX
	(long)_SC_MQ_OPEN_MAX,
#  endif
#  ifdef _SC_MQ_PRIO_MAX
	(long)_SC_MQ_PRIO_MAX,
#  endif
#  ifdef _SC_NGROUPS_MAX
	(long)_SC_NGROUPS_MAX,
#  endif
#  ifdef _SC_NPROCESSORS_CONF
	(long)_SC_NPROCESSORS_CONF,
#  endif
#  ifdef _SC_NPROCESSORS_ONLN
	(long)_SC_NPROCESSORS_ONLN,
#  endif
#  ifdef _SC_NSIG
	(long)_SC_NSIG,
#  endif
#  ifdef _SC_OPEN_MAX
	(long)_SC_OPEN_MAX,
#  endif
#  ifdef _SC_PAGESIZE
	(long)_SC_PAGESIZE,
#  endif
#  ifdef _SC_PRIORITIZED_IO
	(long)_SC_PRIORITIZED_IO,
#  endif
#  ifdef _SC_PRIORITY_SCHEDULING
	(long)_SC_PRIORITY_SCHEDULING,
#  endif
#  ifdef _SC_RAW_SOCKETS
	(long)_SC_RAW_SOCKETS,
#  endif
#  ifdef _SC_READER_WRITER_LOCKS
	(long)_SC_READER_WRITER_LOCKS,
#  endif
#  ifdef _SC_REALTIME_SIGNALS
	(long)_SC_REALTIME_SIGNALS,
#  endif
#  ifdef _SC_REGEXP
	(long)_SC_REGEXP,
#  endif
#  ifdef _SC_RE_DUP_MAX
	(long)_SC_RE_DUP_MAX,
#  endif
#  ifdef _SC_RTSIG_MAX
	(long)_SC_RTSIG_MAX,
#  endif
#  ifdef _SC_SAVED_IDS
	(long)_SC_SAVED_IDS,
#  endif
#  ifdef _SC_SEMAPHORES
	(long)_SC_SEMAPHORES,
#  endif
#  ifdef _SC_SEM_NSEMS_MAX
	(long)_SC_SEM_NSEMS_MAX,
#  endif
#  ifdef _SC_SEM_VALUE_MAX
	(long)_SC_SEM_VALUE_MAX,
#  endif
#  ifdef _SC_SHARED_MEMORY_OBJECTS
	(long)_SC_SHARED_MEMORY_OBJECTS,
#  endif
#  ifdef _SC_SHELL
	(long)_SC_SHELL,
#  endif
#  ifdef _SC_SIGQUEUE_MAX
	(long)_SC_SIGQUEUE_MAX,
#  endif
#  ifdef _SC_SPAWN
	(long)_SC_SPAWN,
#  endif
#  ifdef _SC_SPIN_LOCKS
	(long)_SC_SPIN_LOCKS,
#  endif
#  ifdef _SC_SPORADIC_SERVER
	(long)_SC_SPORADIC_SERVER,
#  endif
#  ifdef _SC_SS_REPL_MAX
	(long)_SC_SS_REPL_MAX,
#  endif
#  ifdef _SC_STREAM_MAX
	(long)_SC_STREAM_MAX,
#  endif
#  ifdef _SC_SYMLOOP_MAX
	(long)_SC_SYMLOOP_MAX,
#  endif
#  ifdef _SC_SYNCHRONIZED_IO
	(long)_SC_SYNCHRONIZED_IO,
#  endif
#  ifdef _SC_THREADS
	(long)_SC_THREADS,
#  endif
#  ifdef _SC_THREAD_ATTR_STACKADDR
	(long)_SC_THREAD_ATTR_STACKADDR,
#  endif
#  ifdef _SC_THREAD_ATTR_STACKSIZE
	(long)_SC_THREAD_ATTR_STACKSIZE,
#  endif
#  ifdef _SC_THREAD_CPUTIME
	(long)_SC_THREAD_CPUTIME,
#  endif
#  ifdef _SC_THREAD_DESTRUCTOR_ITERATIONS
	(long)_SC_THREAD_DESTRUCTOR_ITERATIONS,
#  endif
#  ifdef _SC_THREAD_KEYS_MAX
	(long)_SC_THREAD_KEYS_MAX,
#  endif
#  ifdef _SC_THREAD_PRIORITY_SCHEDULING
	(long)_SC_THREAD_PRIORITY_SCHEDULING,
#  endif
#  ifdef _SC_THREAD_PRIO_INHERIT
	(long)_SC_THREAD_PRIO_INHERIT,
#  endif
#  ifdef _SC_THREAD_PRIO_PROTECT
	(long)_SC_THREAD_PRIO_PROTECT,
#  endif
#  ifdef _SC_THREAD_PROCESS_SHARED
	(long)_SC_THREAD_PROCESS_SHARED,
#  endif
#  ifdef _SC_THREAD_ROBUST_PRIO_INHERIT
	(long)_SC_THREAD_ROBUST_PRIO_INHERIT,
#  endif
#  ifdef _SC_THREAD_ROBUST_PRIO_PROTECT
	(long)_SC_THREAD_ROBUST_PRIO_PROTECT,
#  endif
#  ifdef _SC_THREAD_SAFE_FUNCTIONS
	(long)_SC_THREAD_SAFE_FUNCTIONS,
#  endif
#  ifdef _SC_THREAD_SPORADIC_SERVER
	(long)_SC_THREAD_SPORADIC_SERVER,
#  endif
#  ifdef _SC_THREAD_STACK_MIN
	(long)_SC_THREAD_STACK_MIN,
#  endif
#  ifdef _SC_THREAD_THREADS_MAX
	(long)_SC_THREAD_THREADS_MAX,
#  endif
#  ifdef _SC_TIMEOUTS
	(long)_SC_TIMEOUTS,
#  endif
#  ifdef _SC_TIMERS
	(long)_SC_TIMERS,
#  endif
#  ifdef _SC_TIMER_MAX
	(long)_SC_TIMER_MAX,
#  endif
#  ifdef _SC_TRACE
	(long)_SC_TRACE,
#  endif
#  ifdef _SC_TRACE_EVENT_FILTER
	(long)_SC_TRACE_EVENT_FILTER,
#  endif
#  ifdef _SC_TRACE_EVENT_NAME_MAX
	(long)_SC_TRACE_EVENT_NAME_MAX,
#  endif
#  ifdef _SC_TRACE_INHERIT
	(long)_SC_TRACE_INHERIT,
#  endif
#  ifdef _SC_TRACE_LOG
	(long)_SC_TRACE_LOG,
#  endif
#  ifdef _SC_TRACE_NAME_MAX
	(long)_SC_TRACE_NAME_MAX,
#  endif
#  ifdef _SC_TRACE_SYS_MAX
	(long)_SC_TRACE_SYS_MAX,
#  endif
#  ifdef _SC_TRACE_USER_EVENT_MAX
	(long)_SC_TRACE_USER_EVENT_MAX,
#  endif
#  ifdef _SC_TTY_NAME_MAX
	(long)_SC_TTY_NAME_MAX,
#  endif
#  ifdef _SC_TYPED_MEMORY_OBJECTS
	(long)_SC_TYPED_MEMORY_OBJECTS,
#  endif
#  ifdef _SC_TZNAME_MAX
	(long)_SC_TZNAME_MAX,
#  endif
#  ifdef _SC_V6_ILP32_OFF32
	(long)_SC_V6_ILP32_OFF32,
#  endif
#  ifdef _SC_V6_ILP32_OFFBIG
	(long)_SC_V6_ILP32_OFFBIG,
#  endif
#  ifdef _SC_V6_LP64_OFF64
	(long)_SC_V6_LP64_OFF64,
#  endif
#  ifdef _SC_V6_LPBIG_OFFBIG
	(long)_SC_V6_LPBIG_OFFBIG,
#  endif
#  ifdef _SC_V7_ILP32_OFF32
	(long)_SC_V7_ILP32_OFF32,
#  endif
#  ifdef _SC_V7_ILP32_OFFBIG
	(long)_SC_V7_ILP32_OFFBIG,
#  endif
#  ifdef _SC_V7_LP64_OFF64
	(long)_SC_V7_LP64_OFF64,
#  endif
#  ifdef _SC_V7_LPBIG_OFFBIG
	(long)_SC_V7_LPBIG_OFFBIG,
#  endif
#  ifdef _SC_V8_ILP32_OFF32
	(long)_SC_V8_ILP32_OFF32,
#  endif
#  ifdef _SC_V8_ILP32_OFFBIG
	(long)_SC_V8_ILP32_OFFBIG,
#  endif
#  ifdef _SC_V8_LP64_OFF64
	(long)_SC_V8_LP64_OFF64,
#  endif
#  ifdef _SC_V8_LPBIG_OFFBIG
	(long)_SC_V8_LPBIG_OFFBIG,
#  endif
#  ifdef _SC_VERSION
	(long)_SC_VERSION,
#  endif
#  ifdef _SC_XOPEN_CRYPT
	(long)_SC_XOPEN_CRYPT,
#  endif
#  ifdef _SC_XOPEN_ENH_I18N
	(long)_SC_XOPEN_ENH_I18N,
#  endif
#  ifdef _SC_XOPEN_LEGACY
	(long)_SC_XOPEN_LEGACY,
#  endif
#  ifdef _SC_XOPEN_REALTIME
	(long)_SC_XOPEN_REALTIME,
#  endif
#  ifdef _SC_XOPEN_REALTIME_THREADS
	(long)_SC_XOPEN_REALTIME_THREADS,
#  endif
#  ifdef _SC_XOPEN_SHM
	(long)_SC_XOPEN_SHM,
#  endif
#  ifdef _SC_XOPEN_STREAMS
	(long)_SC_XOPEN_STREAMS,
#  endif
#  ifdef _SC_XOPEN_UNIX
	(long)_SC_XOPEN_UNIX,
#  endif
#  ifdef _SC_XOPEN_UUCP
	(long)_SC_XOPEN_UUCP,
#  endif
#  ifdef _SC_XOPEN_VERSION
	(long)_SC_XOPEN_VERSION,
#  endif
	-12345678L
};

/* How many of the _SC_* names (but _SC_PAGE_SIZE) have the value v? */
static inline int	bitsSysconfCount(long v)
{
	int	k = 0;

	for (int i = 0; i < (int)(sizeof(bitsSysconfNames) / sizeof(bitsSysconfNames[0])); i++)
		k += bitsSysconfNames[i] == v;
	return (k);
}

/* Does v appear exactly once among them? */
static inline int	bitsSysconfUnique(long v)
{
	return (bitsSysconfCount(v) == 1);
}

# if defined(LC_REQ__SC_2_CHAR_TERM) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_CHAR_TERM__unique, "_SC_2_CHAR_TERM", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_CHAR_TERM));
}
# endif

# if defined(LC_REQ__SC_2_C_BIND) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_C_BIND__unique, "_SC_2_C_BIND", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_C_BIND));
}
# endif

# if defined(LC_REQ__SC_2_C_DEV) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_C_DEV__unique, "_SC_2_C_DEV", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_C_DEV));
}
# endif

# if defined(LC_REQ__SC_2_FORT_DEV) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_FORT_DEV__unique, "_SC_2_FORT_DEV", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_FORT_DEV));
}
# endif

# if defined(LC_REQ__SC_2_FORT_RUN) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_FORT_RUN__unique, "_SC_2_FORT_RUN", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_FORT_RUN));
}
# endif

# if defined(LC_REQ__SC_2_LOCALEDEF) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_LOCALEDEF__unique, "_SC_2_LOCALEDEF", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_LOCALEDEF));
}
# endif

# if defined(LC_REQ__SC_2_PBS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_PBS__unique, "_SC_2_PBS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_PBS));
}
# endif

# if defined(LC_REQ__SC_2_PBS_ACCOUNTING) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_PBS_ACCOUNTING__unique, "_SC_2_PBS_ACCOUNTING", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_PBS_ACCOUNTING));
}
# endif

# if defined(LC_REQ__SC_2_PBS_CHECKPOINT) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_PBS_CHECKPOINT__unique, "_SC_2_PBS_CHECKPOINT", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_PBS_CHECKPOINT));
}
# endif

# if defined(LC_REQ__SC_2_PBS_LOCATE) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_PBS_LOCATE__unique, "_SC_2_PBS_LOCATE", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_PBS_LOCATE));
}
# endif

# if defined(LC_REQ__SC_2_PBS_MESSAGE) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_PBS_MESSAGE__unique, "_SC_2_PBS_MESSAGE", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_PBS_MESSAGE));
}
# endif

# if defined(LC_REQ__SC_2_PBS_TRACK) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_PBS_TRACK__unique, "_SC_2_PBS_TRACK", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_PBS_TRACK));
}
# endif

# if defined(LC_REQ__SC_2_SW_DEV) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_SW_DEV__unique, "_SC_2_SW_DEV", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_SW_DEV));
}
# endif

# if defined(LC_REQ__SC_2_UPE) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_UPE__unique, "_SC_2_UPE", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_UPE));
}
# endif

# if defined(LC_REQ__SC_2_VERSION) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_2_VERSION__unique, "_SC_2_VERSION", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_2_VERSION));
}
# endif

# if defined(LC_REQ__SC_ADVISORY_INFO) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_ADVISORY_INFO__unique, "_SC_ADVISORY_INFO", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_ADVISORY_INFO));
}
# endif

# if defined(LC_REQ__SC_AIO_LISTIO_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_AIO_LISTIO_MAX__unique, "_SC_AIO_LISTIO_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_AIO_LISTIO_MAX));
}
# endif

# if defined(LC_REQ__SC_AIO_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_AIO_MAX__unique, "_SC_AIO_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_AIO_MAX));
}
# endif

# if defined(LC_REQ__SC_AIO_PRIO_DELTA_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_AIO_PRIO_DELTA_MAX__unique, "_SC_AIO_PRIO_DELTA_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_AIO_PRIO_DELTA_MAX));
}
# endif

# if defined(LC_REQ__SC_ARG_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_ARG_MAX__unique, "_SC_ARG_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_ARG_MAX));
}
# endif

# if defined(LC_REQ__SC_ASYNCHRONOUS_IO) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_ASYNCHRONOUS_IO__unique, "_SC_ASYNCHRONOUS_IO", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_ASYNCHRONOUS_IO));
}
# endif

# if defined(LC_REQ__SC_ATEXIT_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_ATEXIT_MAX__unique, "_SC_ATEXIT_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_ATEXIT_MAX));
}
# endif

# if defined(LC_REQ__SC_BARRIERS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_BARRIERS__unique, "_SC_BARRIERS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_BARRIERS));
}
# endif

# if defined(LC_REQ__SC_BC_BASE_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_BC_BASE_MAX__unique, "_SC_BC_BASE_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_BC_BASE_MAX));
}
# endif

# if defined(LC_REQ__SC_BC_DIM_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_BC_DIM_MAX__unique, "_SC_BC_DIM_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_BC_DIM_MAX));
}
# endif

# if defined(LC_REQ__SC_BC_SCALE_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_BC_SCALE_MAX__unique, "_SC_BC_SCALE_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_BC_SCALE_MAX));
}
# endif

# if defined(LC_REQ__SC_BC_STRING_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_BC_STRING_MAX__unique, "_SC_BC_STRING_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_BC_STRING_MAX));
}
# endif

# if defined(LC_REQ__SC_CHILD_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_CHILD_MAX__unique, "_SC_CHILD_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_CHILD_MAX));
}
# endif

# if defined(LC_REQ__SC_CLK_TCK) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_CLK_TCK__unique, "_SC_CLK_TCK", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_CLK_TCK));
}
# endif

# if defined(LC_REQ__SC_CLOCK_SELECTION) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_CLOCK_SELECTION__unique, "_SC_CLOCK_SELECTION", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_CLOCK_SELECTION));
}
# endif

# if defined(LC_REQ__SC_COLL_WEIGHTS_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_COLL_WEIGHTS_MAX__unique, "_SC_COLL_WEIGHTS_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_COLL_WEIGHTS_MAX));
}
# endif

# if defined(LC_REQ__SC_CPUTIME) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_CPUTIME__unique, "_SC_CPUTIME", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_CPUTIME));
}
# endif

# if defined(LC_REQ__SC_DELAYTIMER_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_DELAYTIMER_MAX__unique, "_SC_DELAYTIMER_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_DELAYTIMER_MAX));
}
# endif

# if defined(LC_REQ__SC_DEVICE_CONTROL) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_DEVICE_CONTROL__unique, "_SC_DEVICE_CONTROL", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_DEVICE_CONTROL));
}
# endif

# if defined(LC_REQ__SC_EXPR_NEST_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_EXPR_NEST_MAX__unique, "_SC_EXPR_NEST_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_EXPR_NEST_MAX));
}
# endif

# if defined(LC_REQ__SC_FSYNC) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_FSYNC__unique, "_SC_FSYNC", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_FSYNC));
}
# endif

# if defined(LC_REQ__SC_GETGR_R_SIZE_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_GETGR_R_SIZE_MAX__unique, "_SC_GETGR_R_SIZE_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_GETGR_R_SIZE_MAX));
}
# endif

# if defined(LC_REQ__SC_GETPW_R_SIZE_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_GETPW_R_SIZE_MAX__unique, "_SC_GETPW_R_SIZE_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_GETPW_R_SIZE_MAX));
}
# endif

# if defined(LC_REQ__SC_HOST_NAME_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_HOST_NAME_MAX__unique, "_SC_HOST_NAME_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_HOST_NAME_MAX));
}
# endif

# if defined(LC_REQ__SC_IOV_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_IOV_MAX__unique, "_SC_IOV_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_IOV_MAX));
}
# endif

# if defined(LC_REQ__SC_IPV6) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_IPV6__unique, "_SC_IPV6", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_IPV6));
}
# endif

# if defined(LC_REQ__SC_JOB_CONTROL) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_JOB_CONTROL__unique, "_SC_JOB_CONTROL", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_JOB_CONTROL));
}
# endif

# if defined(LC_REQ__SC_LINE_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_LINE_MAX__unique, "_SC_LINE_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_LINE_MAX));
}
# endif

# if defined(LC_REQ__SC_LOGIN_NAME_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_LOGIN_NAME_MAX__unique, "_SC_LOGIN_NAME_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_LOGIN_NAME_MAX));
}
# endif

# if defined(LC_REQ__SC_MAPPED_FILES) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_MAPPED_FILES__unique, "_SC_MAPPED_FILES", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_MAPPED_FILES));
}
# endif

# if defined(LC_REQ__SC_MEMLOCK) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_MEMLOCK__unique, "_SC_MEMLOCK", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_MEMLOCK));
}
# endif

# if defined(LC_REQ__SC_MEMLOCK_RANGE) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_MEMLOCK_RANGE__unique, "_SC_MEMLOCK_RANGE", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_MEMLOCK_RANGE));
}
# endif

# if defined(LC_REQ__SC_MEMORY_PROTECTION) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_MEMORY_PROTECTION__unique, "_SC_MEMORY_PROTECTION", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_MEMORY_PROTECTION));
}
# endif

# if defined(LC_REQ__SC_MESSAGE_PASSING) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_MESSAGE_PASSING__unique, "_SC_MESSAGE_PASSING", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_MESSAGE_PASSING));
}
# endif

# if defined(LC_REQ__SC_MONOTONIC_CLOCK) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_MONOTONIC_CLOCK__unique, "_SC_MONOTONIC_CLOCK", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_MONOTONIC_CLOCK));
}
# endif

# if defined(LC_REQ__SC_MQ_OPEN_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_MQ_OPEN_MAX__unique, "_SC_MQ_OPEN_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_MQ_OPEN_MAX));
}
# endif

# if defined(LC_REQ__SC_MQ_PRIO_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_MQ_PRIO_MAX__unique, "_SC_MQ_PRIO_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_MQ_PRIO_MAX));
}
# endif

# if defined(LC_REQ__SC_NGROUPS_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_NGROUPS_MAX__unique, "_SC_NGROUPS_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_NGROUPS_MAX));
}
# endif

# if defined(LC_REQ__SC_NPROCESSORS_CONF) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_NPROCESSORS_CONF__unique, "_SC_NPROCESSORS_CONF", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_NPROCESSORS_CONF));
}
# endif

# if defined(LC_REQ__SC_NPROCESSORS_ONLN) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_NPROCESSORS_ONLN__unique, "_SC_NPROCESSORS_ONLN", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_NPROCESSORS_ONLN));
}
# endif

# if defined(LC_REQ__SC_NSIG) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_NSIG__unique, "_SC_NSIG", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_NSIG));
}
# endif

# if defined(LC_REQ__SC_OPEN_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_OPEN_MAX__unique, "_SC_OPEN_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_OPEN_MAX));
}
# endif

# if defined(LC_REQ__SC_PAGESIZE) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_PAGESIZE__unique, "_SC_PAGESIZE", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_PAGESIZE));
}
# endif

# if defined(LC_REQ__SC_PAGE_SIZE) || defined(LC_DEFS_ALL)
/* {PAGE_SIZE} is a synonym of {PAGESIZE}: the same name, or a name of its own. */
TEST_SYM(_SC_PAGE_SIZE__unique, "_SC_PAGE_SIZE", same_as_SC_PAGESIZE_or_distinct, .ref = "XSH sysconf()")
{
#  ifdef _SC_PAGESIZE
	if ((long)_SC_PAGE_SIZE == (long)_SC_PAGESIZE)
		EXPECT_TRUE(1);
	else
#  endif
		EXPECT_EQ(bitsSysconfCount((long)_SC_PAGE_SIZE), 0);
}
# endif

# if defined(LC_REQ__SC_PRIORITIZED_IO) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_PRIORITIZED_IO__unique, "_SC_PRIORITIZED_IO", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_PRIORITIZED_IO));
}
# endif

# if defined(LC_REQ__SC_PRIORITY_SCHEDULING) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_PRIORITY_SCHEDULING__unique, "_SC_PRIORITY_SCHEDULING", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_PRIORITY_SCHEDULING));
}
# endif

# if defined(LC_REQ__SC_RAW_SOCKETS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_RAW_SOCKETS__unique, "_SC_RAW_SOCKETS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_RAW_SOCKETS));
}
# endif

# if defined(LC_REQ__SC_READER_WRITER_LOCKS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_READER_WRITER_LOCKS__unique, "_SC_READER_WRITER_LOCKS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_READER_WRITER_LOCKS));
}
# endif

# if defined(LC_REQ__SC_REALTIME_SIGNALS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_REALTIME_SIGNALS__unique, "_SC_REALTIME_SIGNALS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_REALTIME_SIGNALS));
}
# endif

# if defined(LC_REQ__SC_REGEXP) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_REGEXP__unique, "_SC_REGEXP", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_REGEXP));
}
# endif

# if defined(LC_REQ__SC_RE_DUP_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_RE_DUP_MAX__unique, "_SC_RE_DUP_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_RE_DUP_MAX));
}
# endif

# if defined(LC_REQ__SC_RTSIG_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_RTSIG_MAX__unique, "_SC_RTSIG_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_RTSIG_MAX));
}
# endif

# if defined(LC_REQ__SC_SAVED_IDS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SAVED_IDS__unique, "_SC_SAVED_IDS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SAVED_IDS));
}
# endif

# if defined(LC_REQ__SC_SEMAPHORES) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SEMAPHORES__unique, "_SC_SEMAPHORES", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SEMAPHORES));
}
# endif

# if defined(LC_REQ__SC_SEM_NSEMS_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SEM_NSEMS_MAX__unique, "_SC_SEM_NSEMS_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SEM_NSEMS_MAX));
}
# endif

# if defined(LC_REQ__SC_SEM_VALUE_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SEM_VALUE_MAX__unique, "_SC_SEM_VALUE_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SEM_VALUE_MAX));
}
# endif

# if defined(LC_REQ__SC_SHARED_MEMORY_OBJECTS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SHARED_MEMORY_OBJECTS__unique, "_SC_SHARED_MEMORY_OBJECTS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SHARED_MEMORY_OBJECTS));
}
# endif

# if defined(LC_REQ__SC_SHELL) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SHELL__unique, "_SC_SHELL", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SHELL));
}
# endif

# if defined(LC_REQ__SC_SIGQUEUE_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SIGQUEUE_MAX__unique, "_SC_SIGQUEUE_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SIGQUEUE_MAX));
}
# endif

# if defined(LC_REQ__SC_SPAWN) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SPAWN__unique, "_SC_SPAWN", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SPAWN));
}
# endif

# if defined(LC_REQ__SC_SPIN_LOCKS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SPIN_LOCKS__unique, "_SC_SPIN_LOCKS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SPIN_LOCKS));
}
# endif

# if defined(LC_REQ__SC_SPORADIC_SERVER) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SPORADIC_SERVER__unique, "_SC_SPORADIC_SERVER", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SPORADIC_SERVER));
}
# endif

# if defined(LC_REQ__SC_SS_REPL_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SS_REPL_MAX__unique, "_SC_SS_REPL_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SS_REPL_MAX));
}
# endif

# if defined(LC_REQ__SC_STREAM_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_STREAM_MAX__unique, "_SC_STREAM_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_STREAM_MAX));
}
# endif

# if defined(LC_REQ__SC_SYMLOOP_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SYMLOOP_MAX__unique, "_SC_SYMLOOP_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SYMLOOP_MAX));
}
# endif

# if defined(LC_REQ__SC_SYNCHRONIZED_IO) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_SYNCHRONIZED_IO__unique, "_SC_SYNCHRONIZED_IO", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_SYNCHRONIZED_IO));
}
# endif

# if defined(LC_REQ__SC_THREADS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREADS__unique, "_SC_THREADS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREADS));
}
# endif

# if defined(LC_REQ__SC_THREAD_ATTR_STACKADDR) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_ATTR_STACKADDR__unique, "_SC_THREAD_ATTR_STACKADDR", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_ATTR_STACKADDR));
}
# endif

# if defined(LC_REQ__SC_THREAD_ATTR_STACKSIZE) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_ATTR_STACKSIZE__unique, "_SC_THREAD_ATTR_STACKSIZE", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_ATTR_STACKSIZE));
}
# endif

# if defined(LC_REQ__SC_THREAD_CPUTIME) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_CPUTIME__unique, "_SC_THREAD_CPUTIME", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_CPUTIME));
}
# endif

# if defined(LC_REQ__SC_THREAD_DESTRUCTOR_ITERATIONS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_DESTRUCTOR_ITERATIONS__unique, "_SC_THREAD_DESTRUCTOR_ITERATIONS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_DESTRUCTOR_ITERATIONS));
}
# endif

# if defined(LC_REQ__SC_THREAD_KEYS_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_KEYS_MAX__unique, "_SC_THREAD_KEYS_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_KEYS_MAX));
}
# endif

# if defined(LC_REQ__SC_THREAD_PRIORITY_SCHEDULING) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_PRIORITY_SCHEDULING__unique, "_SC_THREAD_PRIORITY_SCHEDULING", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_PRIORITY_SCHEDULING));
}
# endif

# if defined(LC_REQ__SC_THREAD_PRIO_INHERIT) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_PRIO_INHERIT__unique, "_SC_THREAD_PRIO_INHERIT", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_PRIO_INHERIT));
}
# endif

# if defined(LC_REQ__SC_THREAD_PRIO_PROTECT) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_PRIO_PROTECT__unique, "_SC_THREAD_PRIO_PROTECT", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_PRIO_PROTECT));
}
# endif

# if defined(LC_REQ__SC_THREAD_PROCESS_SHARED) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_PROCESS_SHARED__unique, "_SC_THREAD_PROCESS_SHARED", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_PROCESS_SHARED));
}
# endif

# if defined(LC_REQ__SC_THREAD_ROBUST_PRIO_INHERIT) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_ROBUST_PRIO_INHERIT__unique, "_SC_THREAD_ROBUST_PRIO_INHERIT", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_ROBUST_PRIO_INHERIT));
}
# endif

# if defined(LC_REQ__SC_THREAD_ROBUST_PRIO_PROTECT) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_ROBUST_PRIO_PROTECT__unique, "_SC_THREAD_ROBUST_PRIO_PROTECT", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_ROBUST_PRIO_PROTECT));
}
# endif

# if defined(LC_REQ__SC_THREAD_SAFE_FUNCTIONS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_SAFE_FUNCTIONS__unique, "_SC_THREAD_SAFE_FUNCTIONS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_SAFE_FUNCTIONS));
}
# endif

# if defined(LC_REQ__SC_THREAD_SPORADIC_SERVER) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_SPORADIC_SERVER__unique, "_SC_THREAD_SPORADIC_SERVER", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_SPORADIC_SERVER));
}
# endif

# if defined(LC_REQ__SC_THREAD_STACK_MIN) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_STACK_MIN__unique, "_SC_THREAD_STACK_MIN", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_STACK_MIN));
}
# endif

# if defined(LC_REQ__SC_THREAD_THREADS_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_THREAD_THREADS_MAX__unique, "_SC_THREAD_THREADS_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_THREAD_THREADS_MAX));
}
# endif

# if defined(LC_REQ__SC_TIMEOUTS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TIMEOUTS__unique, "_SC_TIMEOUTS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TIMEOUTS));
}
# endif

# if defined(LC_REQ__SC_TIMERS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TIMERS__unique, "_SC_TIMERS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TIMERS));
}
# endif

# if defined(LC_REQ__SC_TIMER_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TIMER_MAX__unique, "_SC_TIMER_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TIMER_MAX));
}
# endif

# if defined(LC_REQ__SC_TRACE) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TRACE__unique, "_SC_TRACE", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TRACE));
}
# endif

# if defined(LC_REQ__SC_TRACE_EVENT_FILTER) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TRACE_EVENT_FILTER__unique, "_SC_TRACE_EVENT_FILTER", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TRACE_EVENT_FILTER));
}
# endif

# if defined(LC_REQ__SC_TRACE_EVENT_NAME_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TRACE_EVENT_NAME_MAX__unique, "_SC_TRACE_EVENT_NAME_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TRACE_EVENT_NAME_MAX));
}
# endif

# if defined(LC_REQ__SC_TRACE_INHERIT) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TRACE_INHERIT__unique, "_SC_TRACE_INHERIT", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TRACE_INHERIT));
}
# endif

# if defined(LC_REQ__SC_TRACE_LOG) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TRACE_LOG__unique, "_SC_TRACE_LOG", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TRACE_LOG));
}
# endif

# if defined(LC_REQ__SC_TRACE_NAME_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TRACE_NAME_MAX__unique, "_SC_TRACE_NAME_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TRACE_NAME_MAX));
}
# endif

# if defined(LC_REQ__SC_TRACE_SYS_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TRACE_SYS_MAX__unique, "_SC_TRACE_SYS_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TRACE_SYS_MAX));
}
# endif

# if defined(LC_REQ__SC_TRACE_USER_EVENT_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TRACE_USER_EVENT_MAX__unique, "_SC_TRACE_USER_EVENT_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TRACE_USER_EVENT_MAX));
}
# endif

# if defined(LC_REQ__SC_TTY_NAME_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TTY_NAME_MAX__unique, "_SC_TTY_NAME_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TTY_NAME_MAX));
}
# endif

# if defined(LC_REQ__SC_TYPED_MEMORY_OBJECTS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TYPED_MEMORY_OBJECTS__unique, "_SC_TYPED_MEMORY_OBJECTS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TYPED_MEMORY_OBJECTS));
}
# endif

# if defined(LC_REQ__SC_TZNAME_MAX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_TZNAME_MAX__unique, "_SC_TZNAME_MAX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_TZNAME_MAX));
}
# endif

# if defined(LC_REQ__SC_V6_ILP32_OFF32) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V6_ILP32_OFF32__unique, "_SC_V6_ILP32_OFF32", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V6_ILP32_OFF32));
}
# endif

# if defined(LC_REQ__SC_V6_ILP32_OFFBIG) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V6_ILP32_OFFBIG__unique, "_SC_V6_ILP32_OFFBIG", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V6_ILP32_OFFBIG));
}
# endif

# if defined(LC_REQ__SC_V6_LP64_OFF64) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V6_LP64_OFF64__unique, "_SC_V6_LP64_OFF64", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V6_LP64_OFF64));
}
# endif

# if defined(LC_REQ__SC_V6_LPBIG_OFFBIG) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V6_LPBIG_OFFBIG__unique, "_SC_V6_LPBIG_OFFBIG", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V6_LPBIG_OFFBIG));
}
# endif

# if defined(LC_REQ__SC_V7_ILP32_OFF32) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V7_ILP32_OFF32__unique, "_SC_V7_ILP32_OFF32", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V7_ILP32_OFF32));
}
# endif

# if defined(LC_REQ__SC_V7_ILP32_OFFBIG) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V7_ILP32_OFFBIG__unique, "_SC_V7_ILP32_OFFBIG", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V7_ILP32_OFFBIG));
}
# endif

# if defined(LC_REQ__SC_V7_LP64_OFF64) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V7_LP64_OFF64__unique, "_SC_V7_LP64_OFF64", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V7_LP64_OFF64));
}
# endif

# if defined(LC_REQ__SC_V7_LPBIG_OFFBIG) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V7_LPBIG_OFFBIG__unique, "_SC_V7_LPBIG_OFFBIG", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V7_LPBIG_OFFBIG));
}
# endif

# if defined(LC_REQ__SC_V8_ILP32_OFF32) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V8_ILP32_OFF32__unique, "_SC_V8_ILP32_OFF32", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V8_ILP32_OFF32));
}
# endif

# if defined(LC_REQ__SC_V8_ILP32_OFFBIG) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V8_ILP32_OFFBIG__unique, "_SC_V8_ILP32_OFFBIG", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V8_ILP32_OFFBIG));
}
# endif

# if defined(LC_REQ__SC_V8_LP64_OFF64) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V8_LP64_OFF64__unique, "_SC_V8_LP64_OFF64", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V8_LP64_OFF64));
}
# endif

# if defined(LC_REQ__SC_V8_LPBIG_OFFBIG) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_V8_LPBIG_OFFBIG__unique, "_SC_V8_LPBIG_OFFBIG", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_V8_LPBIG_OFFBIG));
}
# endif

# if defined(LC_REQ__SC_VERSION) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_VERSION__unique, "_SC_VERSION", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_VERSION));
}
# endif

# if defined(LC_REQ__SC_XOPEN_CRYPT) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_XOPEN_CRYPT__unique, "_SC_XOPEN_CRYPT", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_XOPEN_CRYPT));
}
# endif

# if defined(LC_REQ__SC_XOPEN_ENH_I18N) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_XOPEN_ENH_I18N__unique, "_SC_XOPEN_ENH_I18N", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_XOPEN_ENH_I18N));
}
# endif

# if defined(LC_REQ__SC_XOPEN_LEGACY) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_XOPEN_LEGACY__unique, "_SC_XOPEN_LEGACY", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_XOPEN_LEGACY));
}
# endif

# if defined(LC_REQ__SC_XOPEN_REALTIME) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_XOPEN_REALTIME__unique, "_SC_XOPEN_REALTIME", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_XOPEN_REALTIME));
}
# endif

# if defined(LC_REQ__SC_XOPEN_REALTIME_THREADS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_XOPEN_REALTIME_THREADS__unique, "_SC_XOPEN_REALTIME_THREADS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_XOPEN_REALTIME_THREADS));
}
# endif

# if defined(LC_REQ__SC_XOPEN_SHM) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_XOPEN_SHM__unique, "_SC_XOPEN_SHM", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_XOPEN_SHM));
}
# endif

# if defined(LC_REQ__SC_XOPEN_STREAMS) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_XOPEN_STREAMS__unique, "_SC_XOPEN_STREAMS", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_XOPEN_STREAMS));
}
# endif

# if defined(LC_REQ__SC_XOPEN_UNIX) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_XOPEN_UNIX__unique, "_SC_XOPEN_UNIX", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_XOPEN_UNIX));
}
# endif

# if defined(LC_REQ__SC_XOPEN_UUCP) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_XOPEN_UUCP__unique, "_SC_XOPEN_UUCP", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_XOPEN_UUCP));
}
# endif

# if defined(LC_REQ__SC_XOPEN_VERSION) || defined(LC_DEFS_ALL)
TEST_SYM(_SC_XOPEN_VERSION__unique, "_SC_XOPEN_VERSION", distinct_from_the_other_sc_names, .ref = "XBD <unistd.h> (sysconf())")
{
	EXPECT_TRUE(bitsSysconfUnique((long)_SC_XOPEN_VERSION));
}
# endif


#endif
