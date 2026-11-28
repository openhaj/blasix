/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file isatty.c
 * @brief Tests for isatty() (<unistd.h>).
 * @Created: 2026/11/28 10:52:59 by Moutig
 * @Updated: 2026/11/28 10:53:43 by Moutig
 *
 * isatty(fildes) returns 1 if the open descriptor fildes is associated
 * with a terminal, 0 otherwise, and may set errno: ENOTTY (not a
 * terminal), EBADF (not a valid open descriptor).
 *
 * A terminal is the slave side of a pseudo-terminal, opened raw from
 * /dev/ptmx (TIOCSPTLCK, TIOCGPTN: the Linux ioctls); without one the
 * case is skipped.
 *
 * Clauses: POSIX: XSH isatty().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH isatty()")
{
	EXPECT_TYPE(&isatty, int (*)(int));
}

/* A regular file and a pipe: 0 (ENOTTY may be set). */
TEST_OPT(file_and_pipe_are_not_terminals, .ref = "XSH isatty()")
{
	int	fd = ui_file("tty", 4, LC_O_RDONLY);
	int	p[2];

	ASSERT_NE(fd, -1);
	ASSERT_EQ(lc_sysPipe(p, LC_O_CLOEXEC), 0);
	errno = 0;
	EXPECT_EQ(isatty(fd), 0);
	EXPECT_TRUE(errno == 0 || errno == ENOTTY);
	EXPECT_EQ(isatty(p[0]), 0);
	EXPECT_EQ(isatty(p[1]), 0);
	(void)lc_sysClose(fd);
	(void)lc_sysClose(p[0]);
	(void)lc_sysClose(p[1]);
}

/* A descriptor that is not open: 0 (EBADF may be set). */
TEST_OPT(closed_descriptor_is_not_a_terminal, .ref = "XSH isatty()")
{
	int	fd = ui_file("tty", 4, LC_O_RDONLY);

	ASSERT_NE(fd, -1);
	(void)lc_sysClose(fd);
	errno = 0;
	EXPECT_EQ(isatty(fd), 0);
	EXPECT_TRUE(errno == 0 || errno == EBADF);
	errno = 0;
	EXPECT_EQ(isatty(-1), 0);
	EXPECT_TRUE(errno == 0 || errno == EBADF);
}

/* Unlock the slave side of the pty master m (Linux ioctls, raw) and open it. */
static long	openPtySlave(int m)
{
	long	unlock = 0;
	unsigned	n = 0;
	char	path[32] = "/dev/pts/";
	int		i = 9;
	char	digits[12];
	int		k = 0;

	if (lc_sysIoctl(m, 0x40045431 /* TIOCSPTLCK */, &unlock) != 0
		|| lc_sysIoctl(m, 0x80045430 /* TIOCGPTN */, &n) != 0)
		return (-1);
	do
	{
		digits[k++] = (char)('0' + n % 10);
		n /= 10;
	} while (n);
	while (k)
		path[i++] = digits[--k];
	path[i] = '\0';
	return (lc_sysOpen(path, LC_O_RDWR | LC_O_CLOEXEC | 0400 /* O_NOCTTY */, 0));
}

/* The master and the slave side of a pseudo-terminal: 1. */
TEST_OPT(pseudo_terminal_is_a_terminal, .ref = "XSH isatty()")
{
	long	m = lc_sysOpen("/dev/ptmx", LC_O_RDWR | LC_O_CLOEXEC | 0400, 0);
	long	s;

	if (m < 0)
		SKIP("no /dev/ptmx");
	s = openPtySlave((int)m);
	if (s < 0)
	{
		(void)lc_sysClose((int)m);
		SKIP("cannot open the pseudo-terminal slave");
	}
	EXPECT_EQ(isatty((int)s), 1);
	EXPECT_EQ(isatty((int)m), 1);
	(void)lc_sysClose((int)s);
	(void)lc_sysClose((int)m);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(isatty(-1), (isatty)(-1));
}
