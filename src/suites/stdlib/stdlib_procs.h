/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file stdlib_procs.h
 * @brief Threads and fork() for the stress cases of the allocator units.
 * @Created: 2026/11/25 14:04:27 by Moutig
 * @Updated: 2026/11/25 14:04:33 by Moutig
 *
 * The memory management functions must work when several threads call
 * them (C11 7.22.3p2: one total order of the allocations and
 * deallocations; XSH 2.9.1: every function is thread-safe unless listed)
 * and in the child of fork() of a single-threaded process (XSH fork()).
 * These helpers run the cases; they are not under test:
 *
 *   - sl_threads: run a function in n POSIX threads and join them;
 *   - sl_fork: run a function in the child of the libc's fork() and give
 *     its exit status.
 *
 * <pthread.h> and <unistd.h> are optional for the libc under test
 * (__has_include): a libc without them makes the case NOT_IMPLEMENTED,
 * not the unit unbuildable. When the header is there, its functions are
 * referenced as usual: a weak reference would not pull them out of a
 * static libc archive. The wait is a raw wait4(2) (framework/sys.h).
 *
 * No aggregate initializer of automatic arrays here or in the cases that
 * use these helpers (lc_fill instead): with LTO, the memset() the
 * compiler adds is not found in a static libc archive already scanned.
 */

#ifndef STDLIB_PROCS_H
# define STDLIB_PROCS_H

# include "stdlib/stdlib_cases.h"
# include "framework/sys.h"

# if defined(__has_include)
#  if __has_include(<pthread.h>)
#   include <pthread.h>
#   define SL_HAVE_PTHREAD_H	1
#  endif
#  if __has_include(<unistd.h>)
#   include <unistd.h>
#   define SL_HAVE_UNISTD_H	1
#  endif
# endif

/** Most threads a case runs at once. */
# define SL_MAX_THREADS	16

# ifdef SL_HAVE_PTHREAD_H

/*
 * Run fn(&args[i * argSize]) in @p n threads (at most SL_MAX_THREADS), all
 * started before the first is joined.
 *
 * @return 0, -1 when the libc has no <pthread.h>, -2 when a thread
 *         could not be created (the others are joined).
 */
static inline int	sl_threads(int n, void *(*fn)(void *), void *args, size_t argSize)
{
	pthread_t	t[SL_MAX_THREADS];
	int			made = 0;

	while (made < n && made < SL_MAX_THREADS
		&& pthread_create(&t[made], NULL, fn, (char *)args + (size_t)made * argSize) == 0)
		made++;
	for (int i = 0; i < made; i++)
		(void)pthread_join(t[i], NULL);
	return (made == n ? 0 : -2);
}
# else
static inline int	sl_threads(int n, void *(*fn)(void *), void *args, size_t argSize)
{
	(void)n;
	(void)fn;
	(void)args;
	(void)argSize;
	return (-1);
}
# endif

/* Run the threads of a case, or stop it when the libc cannot. */
# define SL_THREADS(n, fn, args)												\
	do																			\
	{																			\
		int	sl_r = sl_threads((n), (fn), (args), sizeof((args)[0]));			\
																				\
		if (sl_r == -1)															\
			NOT_IMPLEMENTED("<pthread.h>");										\
		ASSERT_EQ(sl_r, 0);														\
	} while (0)

# ifdef SL_HAVE_UNISTD_H

/*
 * Run body(arg) in the child of the libc's fork(); the child ends with
 * the value body returns (lc_exitNow: no atexit handler, coverage kept).
 *
 * @return The child's exit status (0 to 255), 256 + the signal that
 *         killed it, -1 when the libc has no <unistd.h>, -2 when it failed.
 */
static inline int	sl_fork(int (*body)(void *), void *arg)
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
# else
static inline int	sl_fork(int (*body)(void *), void *arg)
{
	(void)body;
	(void)arg;
	return (-1);
}
# endif

/* The exit status of body(arg) in a child of fork(), or stop the case. */
# define SL_FORK(st, body, arg)													\
	int	st = sl_fork((body), (arg));											\
	if (st == -1)																\
		NOT_IMPLEMENTED("<unistd.h>");											\
	ASSERT_NE(st, -2)

#endif
