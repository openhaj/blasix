/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file status.h
 * @brief Cases of the status macros WIFEXITED ... WCOREDUMP, for every header that defines them.
 * @Created: 2026/11/25 12:50:14 by Moutig
 * @Updated: 2026/11/25 12:50:20 by Moutig
 *
 * The macros of XBD <sys/wait.h> "for analysis of process status
 * values", applied to the status of a child that exited, was killed by
 * a signal, or was stopped by one. <stdlib.h> defines them as described
 * there (XSI in POSIX.1-2001, CX from 2008; WCOREDUMP from POSIX.1-2024).
 *
 * The statuses come from raw fork(2) and wait4(2) (framework/sys.h): the
 * libc under test only decodes them. The signal numbers and the wait4
 * option are Linux's (the same on x86_64 and aarch64): SIGKILL 9, SIGSTOP
 * 19, WUNTRACED 2. SIGKILL's default action is to terminate without
 * "additional actions" (XSH 2.4.3: T, not A), so WCOREDUMP is false.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * ( -DLC_REQ_<macro>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_WAIT_STATUS_H
# define BITS_WAIT_STATUS_H

# include "framework/lc.h"

# if defined(LC_REQ_WIFEXITED) \
	|| defined(LC_REQ_WEXITSTATUS) \
	|| defined(LC_REQ_WIFSIGNALED) \
	|| defined(LC_REQ_WTERMSIG) \
	|| defined(LC_REQ_WIFSTOPPED) \
	|| defined(LC_REQ_WSTOPSIG) \
	|| defined(LC_REQ_WCOREDUMP) \
	|| defined(LC_DEFS_ALL)

#  include "framework/sys.h"

#  define BITS_SIGSTOP		19
#  define BITS_KWUNTRACED	2

/** How the child of bitsStatus() ends. */
enum e_bitsEnd
{
	BITS_EXIT_5,
	BITS_EXIT_0,
	BITS_EXIT_255,
	BITS_KILLED,
	BITS_STOPPED
};

/*
 * Status of a child that ends as @p end says (a stopped child is then
 * killed and reaped).
 *
 * @return The raw status, or -1 if the child could not be run.
 */
static int	bitsStatus(enum e_bitsEnd end)
{
	long	pid = lc_sysFork();
	int		st = -1;

	if (pid < 0)
		return (-1);
	if (pid == 0)
	{
		if (end == BITS_KILLED)
			(void)lc_sysKill(lc_sysGetpid(), LC_SIGKILL);
		if (end == BITS_STOPPED)
			(void)lc_sysKill(lc_sysGetpid(), BITS_SIGSTOP);
		lc_sysExit(end == BITS_EXIT_5 ? 5 : end == BITS_EXIT_255 ? 255 : 0);
	}
	if (lc_sysWait4(pid, &st, end == BITS_STOPPED ? BITS_KWUNTRACED : 0) != pid)
		return (-1);
	if (end == BITS_STOPPED)
	{
		(void)lc_sysKill(pid, LC_SIGKILL);
		(void)lc_sysWait4(pid, 0, 0);
	}
	return (st);
}

/* The three statuses of a test, or SKIP when a child cannot be run. */
#  define BITS_STATUSES(ex, ki, so)										\
	int	ex = bitsStatus(BITS_EXIT_5);									\
	int	ki = bitsStatus(BITS_KILLED);									\
	int	so = bitsStatus(BITS_STOPPED);									\
	if (ex == -1 || ki == -1 || so == -1)								\
		SKIP("no child process")

# endif

# if defined(LC_REQ_WIFEXITED) || defined(LC_DEFS_ALL)
TEST_SYM(WIFEXITED__exited, "WIFEXITED", true_only_for_a_normal_exit, .ref = "XBD <sys/wait.h>")
{
	BITS_STATUSES(ex, ki, so);
	EXPECT_NE(WIFEXITED(ex), 0);
	EXPECT_EQ(WIFEXITED(ki), 0);
	EXPECT_EQ(WIFEXITED(so), 0);
}
# endif

# if defined(LC_REQ_WEXITSTATUS) || defined(LC_DEFS_ALL)
/* The low-order 8 bits of the status passed to _exit (XSH wait()). */
TEST_SYM(WEXITSTATUS__value, "WEXITSTATUS", gives_the_exit_status, .ref = "XSH wait()")
{
	int	a = bitsStatus(BITS_EXIT_5);
	int	b = bitsStatus(BITS_EXIT_0);
	int	c = bitsStatus(BITS_EXIT_255);

	if (a == -1 || b == -1 || c == -1)
		SKIP("no child process");
	EXPECT_EQ(WEXITSTATUS(a), 5);
	EXPECT_EQ(WEXITSTATUS(b), 0);
	EXPECT_EQ(WEXITSTATUS(c), 255);
}
# endif

# if defined(LC_REQ_WIFSIGNALED) || defined(LC_DEFS_ALL)
TEST_SYM(WIFSIGNALED__killed, "WIFSIGNALED", true_only_for_an_uncaught_signal, .ref = "XBD <sys/wait.h>")
{
	BITS_STATUSES(ex, ki, so);
	EXPECT_EQ(WIFSIGNALED(ex), 0);
	EXPECT_NE(WIFSIGNALED(ki), 0);
	EXPECT_EQ(WIFSIGNALED(so), 0);
}
# endif

# if defined(LC_REQ_WTERMSIG) || defined(LC_DEFS_ALL)
TEST_SYM(WTERMSIG__value, "WTERMSIG", gives_the_terminating_signal, .ref = "XBD <sys/wait.h>")
{
	int	ki = bitsStatus(BITS_KILLED);

	if (ki == -1)
		SKIP("no child process");
	EXPECT_EQ(WTERMSIG(ki), LC_SIGKILL);
}
# endif

# if defined(LC_REQ_WIFSTOPPED) || defined(LC_DEFS_ALL)
TEST_SYM(WIFSTOPPED__stopped, "WIFSTOPPED", true_only_for_a_stopped_child, .ref = "XBD <sys/wait.h>")
{
	BITS_STATUSES(ex, ki, so);
	EXPECT_EQ(WIFSTOPPED(ex), 0);
	EXPECT_EQ(WIFSTOPPED(ki), 0);
	EXPECT_NE(WIFSTOPPED(so), 0);
}
# endif

# if defined(LC_REQ_WSTOPSIG) || defined(LC_DEFS_ALL)
TEST_SYM(WSTOPSIG__value, "WSTOPSIG", gives_the_stopping_signal, .ref = "XBD <sys/wait.h>")
{
	int	so = bitsStatus(BITS_STOPPED);

	if (so == -1)
		SKIP("no child process");
	EXPECT_EQ(WSTOPSIG(so), BITS_SIGSTOP);
}
# endif

# if defined(LC_REQ_WCOREDUMP) || defined(LC_DEFS_ALL)
TEST_SYM(WCOREDUMP__sigkill, "WCOREDUMP", false_when_no_core_image_is_attempted,
	.ref = "XBD <sys/wait.h> (Issue 8)")
{
	int	ki = bitsStatus(BITS_KILLED);

	if (ki == -1)
		SKIP("no child process");
	EXPECT_EQ(WCOREDUMP(ki), 0);
}
# endif

#endif
