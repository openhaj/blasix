/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file gettid.c
 * @brief Tests for gettid() (<unistd.h>, extension).
 * @Created: 2026/11/28 11:05:25 by Moutig
 * @Updated: 2026/11/28 11:05:40 by Moutig
 *
 * gettid() returns the caller's thread ID (Linux); it always succeeds. In
 * the main thread, it equals the process ID; another thread has its own;
 * the child of fork() has the child's. In no standard (EXT=1).
 * Reference: Linux gettid(2), glibc 2.30. The references are the raw
 * gettid(2) and getpid(2).
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "Linux gettid(2)")
{
	EXPECT_TYPE(&gettid, pid_t (*)(void));
}

TEST_OPT(same_as_the_kernel, .ref = "Linux gettid(2)")
{
	EXPECT_EQ((long)gettid(), lc_sysGettid());
}

/* The main thread: the thread ID is the process ID. */
TEST_OPT(main_thread_is_the_process_id, .ref = "Linux gettid(2)")
{
	EXPECT_EQ((long)gettid(), lc_sysGetpid());
}

/* In the child of fork(): the child's thread ID, its process ID. */
TEST_OPT(child_of_fork_has_its_own, .ref = "Linux gettid(2)")
{
	long	*seen = lc_sysMmapShared(2 * sizeof(long));
	pid_t	r;

	ASSERT_NOTNULL(seen);
	r = fork();
	if (r == 0)
	{
		seen[0] = (long)gettid();
		seen[1] = lc_sysGettid();
		lc_sysExit(0);
	}
	ASSERT_TRUE(r > 0);
	(void)lc_sysWait4(r, 0, 0);
	EXPECT_EQ(seen[0], seen[1]);
	EXPECT_EQ(seen[0], (long)r);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ((long)gettid(), (long)(gettid)());
}
