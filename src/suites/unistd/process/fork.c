/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file fork.c
 * @brief Tests for fork() (<unistd.h>).
 * @Created: 2026/11/28 10:12:54 by Moutig
 * @Updated: 2026/11/28 10:13:36 by Moutig
 *
 * fork() creates a child process, a copy of the caller except as listed:
 * a unique process ID, the caller as parent process ID, its own copy of
 * the descriptors, each referring to the same open file description as
 * the parent's. It returns 0 to the child and the child's process ID to
 * the parent; -1 and errno (EAGAIN, ENOMEM) when no process is created.
 * Modifications of private memory after fork() are seen only by the
 * process that made them.
 *
 * The child only uses raw syscalls and records what it saw in a shared
 * mapping; it ends with a raw exit; the parent waits with a raw wait4().
 *
 * Clauses: POSIX: XSH fork().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH fork()")
{
	EXPECT_TYPE(&fork, pid_t (*)(void));
}

/* What the child saw, shared with the parent. */
typedef struct s_seen
{
	long	ret;		/* what fork() returned in the child */
	long	pid;		/* the child's own process ID (raw getpid) */
	long	ppid;		/* the child's parent process ID (raw getppid) */
	int		copy;		/* the value of g_value in the child */
	int		done;
}	t_seen;

static int	g_value = 1;

/* Fork; in the child, record and exit with status st. The parent gets the pid. */
static long	forkAndRecord(t_seen *seen, int st)
{
	pid_t	r = fork();

	if (r == 0)
	{
		seen->ret = (long)r;
		seen->pid = lc_sysGetpid();
		seen->ppid = lc_sysGetppid();
		seen->copy = g_value;
		g_value = 2;
		seen->done = 1;
		lc_sysExit(st);
	}
	return ((long)r);
}

/* The child gets 0, the parent the child's process ID; the child's parent is the caller. */
TEST_OPT(return_values_and_process_ids, .ref = "XSH fork()")
{
	t_seen	*seen = lc_sysMmapShared(sizeof(t_seen));
	long	pid;
	int		st = -1;

	ASSERT_NOTNULL(seen);
	pid = forkAndRecord(seen, 0);
	ASSERT_TRUE(pid > 0);
	EXPECT_EQ(lc_sysWait4(pid, &st, 0), pid);
	EXPECT_EQ(st, 0);
	EXPECT_EQ(seen->done, 1);
	EXPECT_EQ(seen->ret, 0);
	EXPECT_EQ(seen->pid, pid);
	EXPECT_NE(seen->pid, lc_sysGetpid());
	EXPECT_EQ(seen->ppid, lc_sysGetpid());
}

/* The child has a copy of the memory: it sees the parent's data, its changes stay its own. */
TEST_OPT(child_has_a_copy_of_the_memory, .ref = "XSH fork()")
{
	t_seen	*seen = lc_sysMmapShared(sizeof(t_seen));
	long	pid;

	ASSERT_NOTNULL(seen);
	g_value = 1;
	pid = forkAndRecord(seen, 0);
	ASSERT_TRUE(pid > 0);
	(void)lc_sysWait4(pid, 0, 0);
	EXPECT_EQ(seen->copy, 1);
	EXPECT_EQ(g_value, 1);
}

/* Several children: as many distinct process IDs. */
TEST_OPT(each_child_has_its_own_process_id, .ref = "XSH fork()")
{
	t_seen	*seen = lc_sysMmapShared(8 * sizeof(t_seen));
	long	pid[8];
	int		bad = 0;

	ASSERT_NOTNULL(seen);
	for (int i = 0; i < 8; i++)
	{
		pid[i] = forkAndRecord(&seen[i], i);
		ASSERT_TRUE(pid[i] > 0);
	}
	for (int i = 0; i < 8; i++)
	{
		int	st = -1;

		if (lc_sysWait4(pid[i], &st, 0) != pid[i] || ((st >> 8) & 0xff) != i || seen[i].pid != pid[i])
			bad++;
		for (int j = 0; j < i; j++)
			bad += pid[i] == pid[j];
	}
	EXPECT_EQ(bad, 0);
}

/* The child's descriptors refer to the parent's open file descriptions: a shared offset. */
TEST_OPT(descriptors_share_the_open_file_description, .ref = "XSH fork()")
{
	int		fd = ui_file("fk", 100, LC_O_RDONLY);
	pid_t	r;

	ASSERT_NE(fd, -1);
	r = fork();
	if (r == 0)
	{
		(void)lc_sysLseek(fd, 64, LC_SEEK_SET);
		lc_sysExit(0);
	}
	ASSERT_TRUE(r > 0);
	(void)lc_sysWait4(r, 0, 0);
	EXPECT_EQ(lc_sysLseek(fd, 0, LC_SEEK_CUR), 64);
	(void)lc_sysClose(fd);
}

/* A pipe made before fork(): the child writes, the parent reads. */
TEST_OPT(child_writes_to_an_inherited_pipe, .ref = "XSH fork()")
{
	int				p[2];
	unsigned char	b[16];
	pid_t			r;

	ASSERT_EQ(lc_sysPipe(p, LC_O_CLOEXEC), 0);
	r = fork();
	if (r == 0)
	{
		(void)lc_sysClose(p[0]);
		(void)lc_sysWrite(p[1], "from child", 10);
		lc_sysExit(0);
	}
	ASSERT_TRUE(r > 0);
	(void)lc_sysClose(p[1]);
	EXPECT_EQ(lc_sysRead(p[0], b, 16), 10);
	EXPECT_MEMEQ(b, "from child", 10);
	EXPECT_EQ(lc_sysRead(p[0], b, 16), 0);
	(void)lc_sysWait4(r, 0, 0);
	(void)lc_sysClose(p[0]);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	pid_t	r = (fork)();

	if (r == 0)
		lc_sysExit(4);
	ASSERT_TRUE(r > 0);
	{
		int	st = -1;

		EXPECT_EQ(lc_sysWait4(r, &st, 0), r);
		EXPECT_EQ((st >> 8) & 0xff, 4);
	}
}
