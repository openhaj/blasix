/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file dup2.c
 * @brief Tests for dup2() (<unistd.h>).
 * @Created: 2026/11/28 10:09:41 by Moutig
 * @Updated: 2026/11/28 10:11:03 by Moutig
 *
 * dup2(fildes, fildes2) makes fildes2 refer to the open file description
 * of fildes (same offset, same flags) and returns fildes2; an open
 * fildes2 is closed first, unless it equals fildes, which is then
 * returned untouched. Afterwards FD_CLOEXEC (and POSIX.1-2024 FD_CLOFORK)
 * of fildes2 are cleared, unless fildes equals fildes2. Errors: EBADF
 * when fildes is not a valid open descriptor (fildes2 is then not closed)
 * or when fildes2 is negative or not below {OPEN_MAX}.
 *
 * Clauses: POSIX: XSH dup2().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH dup2()")
{
	EXPECT_TYPE(&dup2, int (*)(int, int));
}

/* A free number: returned, and it refers to the same file (shared offset). */
TEST_OPT(refers_to_the_same_open_file_description, .ref = "XSH dup2()")
{
	unsigned char	b[4];
	int				fd = ui_file("d2", 50, LC_O_RDONLY);
	long			free = lc_sysFcntl(0, LC_F_DUPFD_CLOEXEC, 100);

	ASSERT_NE(fd, -1);
	ASSERT_TRUE(free >= 100);
	(void)lc_sysClose((int)free);
	EXPECT_EQ(dup2(fd, (int)free), free);
	EXPECT_EQ(lc_sysLseek((int)free, 20, LC_SEEK_SET), 20);
	EXPECT_EQ(lc_sysLseek(fd, 0, LC_SEEK_CUR), 20);
	EXPECT_EQ(lc_sysRead(fd, b, 4), 4);
	EXPECT_TRUE(ui_matches(b, 4, 20));
	EXPECT_EQ(lc_sysLseek((int)free, 0, LC_SEEK_CUR), 24);
	(void)lc_sysClose((int)free);
	(void)lc_sysClose(fd);
}

/* An open fildes2 is closed first: it now reads the other file. */
TEST_OPT(open_fildes2_is_closed_first, .ref = "XSH dup2()")
{
	unsigned char	b[2];
	int				a = ui_file("d2", 10, LC_O_RDONLY);
	int				p[2];

	ASSERT_NE(a, -1);
	ASSERT_EQ(lc_sysPipe(p, LC_O_CLOEXEC), 0);
	EXPECT_EQ(dup2(a, p[1]), p[1]);
	EXPECT_EQ(lc_sysRead(p[1], b, 2), 2);
	EXPECT_TRUE(ui_matches(b, 2, 0));
	/* The pipe lost its only writer. */
	EXPECT_EQ(lc_sysRead(p[0], b, 2), 0);
	(void)lc_sysClose(p[0]);
	(void)lc_sysClose(p[1]);
	(void)lc_sysClose(a);
}

/* fildes2 == fildes, valid: returned, nothing closed, its FD_CLOEXEC kept. */
TEST_OPT(same_descriptor_is_returned_untouched, .ref = "XSH dup2()")
{
	int	fd = ui_file("d2", 10, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	ASSERT_EQ(lc_sysFcntl(fd, LC_F_GETFD, 0), 1);
	EXPECT_EQ(dup2(fd, fd), fd);
	EXPECT_EQ(lc_sysFcntl(fd, LC_F_GETFD, 0), 1);
	(void)lc_sysClose(fd);
}

/* After a dup2 to another number, FD_CLOEXEC of fildes2 is clear. */
TEST_OPT(close_on_exec_is_cleared, .ref = "XSH dup2()")
{
	int	fd = ui_file("d2", 10, LC_O_RDONLY);
	int	other = ui_file("d2", 10, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	ASSERT_NE(other, -1);
	ASSERT_EQ(lc_sysFcntl(other, LC_F_GETFD, 0), 1);
	EXPECT_EQ(dup2(fd, other), other);
	EXPECT_EQ(lc_sysFcntl(other, LC_F_GETFD, 0), 0);
	EXPECT_EQ(lc_sysFcntl(fd, LC_F_GETFD, 0), 1);
	(void)lc_sysClose(other);
	(void)lc_sysClose(fd);
}

/* fildes not valid: -1, EBADF, and fildes2 is not closed. */
TEST_OPT(invalid_fildes_gives_ebadf_and_keeps_fildes2, .ref = "XSH dup2() ERRORS")
{
	int	c = ui_file("d2", 4, LC_O_RDONLY);
	int	keep = ui_file("d2", 4, LC_O_RDONLY);

	ASSERT_NE(c, -1);
	ASSERT_NE(keep, -1);
	(void)lc_sysClose(c);
	errno = 0;
	EXPECT_EQ(dup2(c, keep), -1);
	EXPECT_EQ(errno, EBADF);
	EXPECT_EQ(lc_sysFcntl(keep, LC_F_GETFD, 0), 1);
	errno = 0;
	EXPECT_EQ(dup2(-1, keep), -1);
	EXPECT_EQ(errno, EBADF);
	errno = 0;
	EXPECT_EQ(dup2(c, c), -1);
	EXPECT_EQ(errno, EBADF);
	(void)lc_sysClose(keep);
}

/* fildes2 negative or not below {OPEN_MAX}: -1 and EBADF. */
TEST_OPT(out_of_range_fildes2_gives_ebadf, .ref = "XSH dup2() ERRORS")
{
	int	fd = ui_file("d2", 4, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	errno = 0;
	EXPECT_EQ(dup2(fd, -1), -1);
	EXPECT_EQ(errno, EBADF);
	errno = 0;
	EXPECT_EQ(dup2(fd, 0x7fffffff), -1);
	EXPECT_EQ(errno, EBADF);
	(void)lc_sysClose(fd);
}

/* The usual use: a pipe end onto a standard number, then back. */
TEST_OPT(redirects_a_standard_descriptor, .ref = "XSH dup2()")
{
	int				p[2];
	long			saved = lc_sysFcntl(STDOUT_FILENO, LC_F_DUPFD_CLOEXEC, 10);
	unsigned char	b[8];

	ASSERT_TRUE(saved >= 10);
	ASSERT_EQ(lc_sysPipe(p, LC_O_CLOEXEC), 0);
	EXPECT_EQ(dup2(p[1], STDOUT_FILENO), STDOUT_FILENO);
	EXPECT_EQ(lc_sysWrite(STDOUT_FILENO, "redir", 5), 5);
	EXPECT_EQ(dup2((int)saved, STDOUT_FILENO), STDOUT_FILENO);
	EXPECT_EQ(lc_sysRead(p[0], b, 8), 5);
	EXPECT_MEMEQ(b, "redir", 5);
	(void)lc_sysClose((int)saved);
	(void)lc_sysClose(p[0]);
	(void)lc_sysClose(p[1]);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	int		fd = ui_file("d2", 4, LC_O_RDONLY);
	long	n = lc_sysFcntl(0, LC_F_DUPFD_CLOEXEC, 200);

	ASSERT_NE(fd, -1);
	ASSERT_TRUE(n >= 200);
	(void)lc_sysClose((int)n);
	EXPECT_EQ((dup2)(fd, (int)n), n);
	(void)lc_sysClose((int)n);
	(void)lc_sysClose(fd);
}
