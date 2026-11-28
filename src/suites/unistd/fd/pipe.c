/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pipe.c
 * @brief Tests for pipe() (<unistd.h>).
 * @Created: 2026/11/28 10:09:50 by Moutig
 * @Updated: 2026/11/28 10:11:22 by Moutig
 *
 * pipe(fildes) creates a pipe: fildes[0] reads what fildes[1] writes,
 * first in, first out; the descriptors are allocated as every new
 * descriptor (the lowest available, XSH 2.6 / 2.14); FD_CLOEXEC (and
 * POSIX.1-2024 FD_CLOFORK) clear on both, O_NONBLOCK clear on both. 0 on
 * success; -1, errno, nothing allocated and fildes unmodified on failure:
 * EMFILE when the process has no descriptor left.
 *
 * Clauses: POSIX: XSH pipe().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH pipe()")
{
	EXPECT_TYPE(&pipe, int (*)(int [2]));
}

TEST_OPT(read_end_gets_what_the_write_end_got, .ref = "XSH pipe()")
{
	int				p[2] = {-1, -1};
	unsigned char	b[16];

	ASSERT_EQ(pipe(p), 0);
	ASSERT_TRUE(p[0] >= 0 && p[1] >= 0 && p[0] != p[1]);
	ASSERT_EQ(lc_sysWrite(p[1], "first", 5), 5);
	ASSERT_EQ(lc_sysWrite(p[1], "second", 6), 6);
	EXPECT_EQ(lc_sysRead(p[0], b, 16), 11);
	EXPECT_MEMEQ(b, "firstsecond", 11);
	(void)lc_sysClose(p[0]);
	(void)lc_sysClose(p[1]);
}

/* No writer left: the reader gets the end of file. */
TEST_OPT(end_of_file_once_the_writer_is_closed, .ref = "XSH pipe()")
{
	int				p[2];
	unsigned char	b[4];

	ASSERT_EQ(pipe(p), 0);
	(void)lc_sysClose(p[1]);
	EXPECT_EQ(lc_sysRead(p[0], b, 4), 0);
	(void)lc_sysClose(p[0]);
}

/* The two lowest free descriptors (2.6, File Descriptor Allocation). */
TEST_OPT(takes_the_lowest_free_descriptors, .ref = "XSH 2.14 / 2.6")
{
	long	a = lc_sysFcntl(0, LC_F_DUPFD_CLOEXEC, 0);
	long	b = lc_sysFcntl(0, LC_F_DUPFD_CLOEXEC, 0);
	int		p[2];

	ASSERT_TRUE(a >= 0 && b > a);
	(void)lc_sysClose((int)a);
	(void)lc_sysClose((int)b);
	ASSERT_EQ(pipe(p), 0);
	EXPECT_TRUE((p[0] == a && p[1] == b) || (p[0] == b && p[1] == a));
	(void)lc_sysClose(p[0]);
	(void)lc_sysClose(p[1]);
}

/* FD_CLOEXEC clear on both descriptors, O_NONBLOCK clear on both ends. */
TEST_OPT(flags_are_clear, .ref = "XSH pipe()")
{
	int	p[2];

	ASSERT_EQ(pipe(p), 0);
	EXPECT_EQ(lc_sysFcntl(p[0], LC_F_GETFD, 0), 0);
	EXPECT_EQ(lc_sysFcntl(p[1], LC_F_GETFD, 0), 0);
	EXPECT_EQ(lc_sysFcntl(p[0], LC_F_GETFL, 0) & LC_O_NONBLOCK, 0);
	EXPECT_EQ(lc_sysFcntl(p[1], LC_F_GETFL, 0) & LC_O_NONBLOCK, 0);
	(void)lc_sysClose(p[0]);
	(void)lc_sysClose(p[1]);
}

/* Large data goes through in order (a writer child, the parent reads). */
static void	writeLots(void *arg)
{
	static unsigned char	b[65536];
	int						fd = *(int *)arg;

	for (unsigned long k = 0; k < 16; k++)
	{
		for (unsigned long i = 0; i < sizeof(b); i++)
			b[i] = ui_pattern(k * sizeof(b) + i);
		if (lc_sysWrite(fd, b, sizeof(b)) != (long)sizeof(b))
			lc_exitNow(1);
	}
	lc_exitNow(0);
}

TEST_OPT(large_data_in_order, .ref = "XSH pipe()")
{
	static unsigned char	b[4096];
	int						p[2];
	unsigned long			total = 0;
	long					r;
	int						ok = 1;

	ASSERT_EQ(pipe(p), 0);
	{
		long	pid = lc_sysFork();

		ASSERT_TRUE(pid >= 0);
		if (pid == 0)
		{
			(void)lc_sysClose(p[0]);
			writeLots(&p[1]);
		}
		(void)lc_sysClose(p[1]);
		while ((r = lc_sysRead(p[0], b, sizeof(b))) > 0)
		{
			ok = ok && ui_matches(b, (unsigned long)r, total);
			total += (unsigned long)r;
		}
		(void)lc_sysWait4(pid, 0, 0);
	}
	EXPECT_UEQ(total, 16UL * 65536);
	EXPECT_TRUE(ok);
	(void)lc_sysClose(p[0]);
}

/* No descriptor left: -1, EMFILE, fildes untouched. */
TEST_OPT(no_descriptor_left_gives_emfile, .ref = "XSH pipe() ERRORS")
{
	static int	held[70000];
	int			n = 0;
	int			p[2] = {-7, -7};
	long		fd;

	while (n < 70000 && (fd = lc_sysFcntl(0, LC_F_DUPFD_CLOEXEC, 0)) >= 0)
		held[n++] = (int)fd;
	if (n == 70000)
	{
		for (int i = 0; i < n; i++)
			(void)lc_sysClose(held[i]);
		SKIP("more than 70000 descriptors available");
	}
	errno = 0;
	EXPECT_EQ(pipe(p), -1);
	EXPECT_EQ(errno, EMFILE);
	EXPECT_TRUE(p[0] == -7 && p[1] == -7);
	/* One descriptor free: still not enough for the two of a pipe. */
	(void)lc_sysClose(held[--n]);
	errno = 0;
	EXPECT_EQ(pipe(p), -1);
	EXPECT_EQ(errno, EMFILE);
	EXPECT_EQ(lc_sysFcntl(held[n], LC_F_GETFD, 0), -EBADF);
	for (int i = 0; i < n; i++)
		(void)lc_sysClose(held[i]);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	int	a[2];
	int	b[2];

	ASSERT_EQ(pipe(a), 0);
	ASSERT_EQ((pipe)(b), 0);
	for (int i = 0; i < 2; i++)
	{
		(void)lc_sysClose(a[i]);
		(void)lc_sysClose(b[i]);
	}
}
