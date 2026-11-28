/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file getppid.c
 * @brief Tests for getppid() (<unistd.h>).
 * @Created: 2026/11/28 10:36:43 by Moutig
 * @Updated: 2026/11/28 10:36:54 by Moutig
 *
 * getppid() returns the parent process ID of the calling process; it always succeeds and no return value is
 * reserved to indicate an error. The reference is the raw syscall.
 *
 * Clauses: POSIX: XSH getppid().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH getppid()")
{
	EXPECT_TYPE(&getppid, pid_t (*)(void));
}

/* The value the kernel gives (the raw syscall). */
TEST_OPT(same_as_the_kernel, .ref = "XSH getppid()")
{
	EXPECT_EQ((long)getppid(), lc_sysGetppid());
}

/* Always successful: the same value on every call, errno never needed. */
TEST_OPT(always_successful_and_stable, .ref = "XSH getppid()")
{
	pid_t	first = getppid();
	int		bad = 0;

	for (int i = 0; i < 1000; i++)
		bad += getppid() != first;
	EXPECT_EQ(bad, 0);
}

/* In the child of fork(): the process ID of the parent. */
TEST_OPT(child_of_fork_gets_the_parent, .ref = "XSH getppid()")
{
	long	*seen = lc_sysMmapShared(sizeof(long));
	pid_t	r;

	ASSERT_NOTNULL(seen);
	r = fork();
	if (r == 0)
	{
		seen[0] = (long)getppid();
		lc_sysExit(0);
	}
	ASSERT_TRUE(r > 0);
	(void)lc_sysWait4(r, 0, 0);
	EXPECT_EQ(seen[0], lc_sysGetpid());
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ((long)getppid(), (long)(getppid)());
}
