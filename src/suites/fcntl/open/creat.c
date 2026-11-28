/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file creat.c
 * @brief Tests for creat() (<fcntl.h>).
 * @Created: 2026/11/28 12:04:17 by Moutig
 * @Updated: 2026/11/28 12:11:10 by Moutig
 *
 * creat(path, mode) is equivalent to open(path, O_WRONLY | O_CREAT |
 * O_TRUNC, mode): a missing file is created with mode less the umask, an
 * existing one is truncated (its mode kept), and the descriptor is write
 * only, the lowest free, with FD_CLOEXEC clear.
 *
 * Clauses: POSIX: XSH creat().
 */
#include "fcntl/fcntl_cases.h"

TEST_OPT(prototype, .ref = "XSH creat()")
{
	EXPECT_TYPE(&creat, int (*)(const char *, mode_t));
}

/* A missing file: created, regular, empty, mode less the umask. */
TEST_OPT(creates_with_the_mode_less_the_umask, .ref = "XSH creat()")
{
	char	p[128];
	int		old = fc_umask(022);
	int		fd = creat(fc_path(p, sizeof(p), "cr"), 0666);
	fc_st	st;

	(void)fc_umask(old);
	ASSERT_GE(fd, 0);
	ASSERT_EQ(fc_stat(-1, p, 1, &st), 0);
	EXPECT_EQ(st.mode & FC_IFMT, FC_IFREG);
	EXPECT_EQ(st.mode & 07777, 0644);
	EXPECT_EQ(st.size, 0);
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
}

/* An existing file: truncated to 0, its mode kept. */
TEST_OPT(truncates_an_existing_file, .ref = "XSH creat()")
{
	char	p[128];
	int		fd;

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "ct"), 50), 0);
	fd = creat(p, 0777);
	ASSERT_GE(fd, 0);
	EXPECT_EQ(fc_size(fd), 0);
	EXPECT_EQ(fc_perm(p), 0600);
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
}

/* As open(O_WRONLY | O_CREAT | O_TRUNC): write only, no other status flag, the lowest free fd, FD_CLOEXEC clear. */
TEST_OPT(same_as_open_wronly_creat_trunc, .ref = "XSH creat()")
{
	char	p[128];
	char	q[128];
	char	c;
	int		want = fc_lowestFree();
	int		a = creat(fc_path(p, sizeof(p), "ca"), 0600);
	int		b = open(fc_path(q, sizeof(q), "cb"), O_WRONLY | O_CREAT | O_TRUNC, 0600);

	ASSERT_TRUE(a >= 0 && b >= 0);
	EXPECT_EQ(a, want);
	EXPECT_EQ(fc_flFlags(a), fc_flFlags(b));
	EXPECT_EQ(fc_flFlags(a) & 3, LC_O_WRONLY);
	EXPECT_EQ(fc_fdFlags(a) & FC_FD_CLOEXEC, 0);
	EXPECT_LT(lc_sysRead(a, &c, 1), 0);
	(void)lc_sysClose(a);
	(void)lc_sysClose(b);
	(void)lc_sysUnlink(p);
	(void)lc_sysUnlink(q);
}

/* A missing directory in the path: ENOENT; a directory: EISDIR. */
TEST_OPT(errors_of_open, .ref = "XSH creat()")
{
	char	p[128];
	char	q[160];
	int		n;

	(void)fc_path(p, sizeof(p), "ce");
	for (n = 0; p[n]; n++)
		q[n] = p[n];
	q[n] = '/';
	q[n + 1] = 'f';
	q[n + 2] = 0;
	errno = 0;
	EXPECT_EQ(creat(q, 0600), -1);
	EXPECT_EQ(errno, ENOENT);
	ASSERT_EQ(lc_sysMkdir(p, 0700), 0);
	errno = 0;
	EXPECT_EQ(creat(p, 0600), -1);
	EXPECT_EQ(errno, EISDIR);
	(void)lc_sysRmdir(p);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(creat("", 0600), (creat)("", 0600));
}
