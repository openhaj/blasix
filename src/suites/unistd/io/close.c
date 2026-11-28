/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file close.c
 * @brief Tests for close() (<unistd.h>).
 * @Created: 2026/11/28 09:54:09 by Moutig
 * @Updated: 2026/11/28 09:56:25 by Moutig
 *
 * close(fildes) deallocates the descriptor: it is no longer valid and
 * becomes available to the functions that allocate descriptors; when the
 * last descriptor of a pipe's write end is closed, the reader sees the end
 * of the file. 0 on success; -1 and errno: EBADF for a descriptor that is
 * not valid (POSIX.1-2024: not open), never EAGAIN nor EWOULDBLOCK (2024).
 *
 * Clauses: POSIX: XSH close().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH close()")
{
	EXPECT_TYPE(&close, int (*)(int));
}

/* 0, and the descriptor is no longer valid (raw fcntl: EBADF). */
TEST_OPT(closes_the_descriptor, .ref = "XSH close()")
{
	int	fd = ui_file("cl", 4, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	EXPECT_EQ(close(fd), 0);
	EXPECT_EQ(lc_sysFcntl(fd, LC_F_GETFD, 0), -EBADF);
}

/* The number is free again: the lowest free descriptor from it is it. */
TEST_OPT(descriptor_becomes_available, .ref = "XSH close()")
{
	int		fd = ui_file("cl", 4, LC_O_RDONLY);
	long	again;

	ASSERT_NE(fd, -1);
	ASSERT_EQ(close(fd), 0);
	again = lc_sysFcntl(0, LC_F_DUPFD_CLOEXEC, fd);
	EXPECT_EQ(again, fd);
	if (again >= 0)
		(void)lc_sysClose((int)again);
}

TEST_OPT(closed_twice_gives_ebadf, .ref = "XSH close() ERRORS")
{
	int	fd = ui_file("cl", 4, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	EXPECT_EQ(close(fd), 0);
	errno = 0;
	EXPECT_EQ(close(fd), -1);
	EXPECT_EQ(errno, EBADF);
}

TEST_OPT(invalid_descriptor_gives_ebadf, .ref = "XSH close() ERRORS")
{
	static const int	bad[] = {-1, -1000, 4095, 100000};

	for (int i = 0; i < 4; i++)
	{
		errno = 0;
		EXPECT_EQ(close(bad[i]), -1);
		EXPECT_EQ(errno, EBADF);
	}
}

/* The write end of a pipe closed: the reader gets what was in it, then the end of file. */
TEST_OPT(closing_the_write_end_ends_the_pipe, .ref = "XSH close()")
{
	int				p[2];
	unsigned char	buf[8];

	ASSERT_EQ(lc_sysPipe(p, LC_O_CLOEXEC), 0);
	ASSERT_EQ(lc_sysWrite(p[1], "ok", 2), 2);
	EXPECT_EQ(close(p[1]), 0);
	EXPECT_EQ(lc_sysRead(p[0], buf, 8), 2);
	EXPECT_EQ(lc_sysRead(p[0], buf, 8), 0);
	(void)lc_sysClose(p[0]);
}

/* Another descriptor of the same file stays valid and keeps the file. */
TEST_OPT(other_descriptors_stay_open, .ref = "XSH close()")
{
	unsigned char	buf[5];
	int				fd = ui_file("cl", 5, LC_O_RDONLY);
	long			dup;

	ASSERT_NE(fd, -1);
	dup = lc_sysFcntl(fd, LC_F_DUPFD_CLOEXEC, 0);
	ASSERT_TRUE(dup >= 0);
	EXPECT_EQ(close(fd), 0);
	EXPECT_EQ(lc_sysRead((int)dup, buf, 5), 5);
	EXPECT_TRUE(ui_matches(buf, 5, 0));
	(void)lc_sysClose((int)dup);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	int	a = ui_file("cl", 1, LC_O_RDONLY);
	int	b = ui_file("cl", 1, LC_O_RDONLY);

	ASSERT_NE(a, -1);
	ASSERT_NE(b, -1);
	EXPECT_EQ(close(a), 0);
	EXPECT_EQ((close)(b), 0);
}
