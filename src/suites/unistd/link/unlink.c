/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file unlink.c
 * @brief Tests for unlink() (<unistd.h>).
 * @Created: 2026/11/28 10:40:36 by Moutig
 * @Updated: 2026/11/28 10:41:52 by Moutig
 *
 * unlink(path) removes the directory entry path; a symbolic link is
 * removed itself, not what it names; a file still open stays readable
 * until it is closed. 0 on success; -1, errno and the file unchanged on
 * failure: ENOENT (missing, or an empty path), ENOTDIR (a file in the
 * prefix), EPERM (a directory, without the privilege or the support to
 * unlink it).
 *
 * The files are made and looked for with raw syscalls in /tmp.
 *
 * Clauses: POSIX: XSH unlink().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH unlink()")
{
	EXPECT_TYPE(&unlink, int (*)(const char *));
}

/* A new file at a temporary path (out: path). */
static int	makeFile(char *path, unsigned long size, const char *tag)
{
	long	fd;

	(void)lc_tmpPath(path, size, tag);
	fd = lc_sysOpen(path, LC_O_WRONLY | LC_O_CREAT | LC_O_TRUNC | LC_O_CLOEXEC, 0600);
	if (fd < 0)
		return (0);
	(void)lc_sysWrite((int)fd, "data", 4);
	(void)lc_sysClose((int)fd);
	return (1);
}

static long	rawOpen(const char *path)
{
	long	fd = lc_sysOpen(path, LC_O_RDONLY | LC_O_CLOEXEC, 0);

	if (fd >= 0)
		(void)lc_sysClose((int)fd);
	return (fd);
}

/* 0, and the file is gone: opening it gives ENOENT. */
TEST_OPT(removes_the_file, .ref = "XSH unlink()")
{
	char	f[128];

	ASSERT_TRUE(makeFile(f, sizeof(f), "ul"));
	EXPECT_EQ(unlink(f), 0);
	EXPECT_EQ(rawOpen(f), -ENOENT);
}

/* Still open when unlinked: readable until it is closed. */
TEST_OPT(open_file_stays_readable, .ref = "XSH unlink()")
{
	char			f[128];
	unsigned char	b[4];
	long			fd;

	ASSERT_TRUE(makeFile(f, sizeof(f), "ul"));
	fd = lc_sysOpen(f, LC_O_RDONLY | LC_O_CLOEXEC, 0);
	ASSERT_TRUE(fd >= 0);
	EXPECT_EQ(unlink(f), 0);
	EXPECT_EQ(rawOpen(f), -ENOENT);
	EXPECT_EQ(lc_sysRead((int)fd, b, 4), 4);
	EXPECT_MEMEQ(b, "data", 4);
	(void)lc_sysClose((int)fd);
}

/* A symbolic link: the link goes, the file it names stays. */
TEST_OPT(removes_a_link_not_its_target, .ref = "XSH unlink()")
{
	char	f[128];
	char	l[128];

	ASSERT_TRUE(makeFile(f, sizeof(f), "ulf"));
	(void)lc_tmpPath(l, sizeof(l), "ull");
	ASSERT_EQ(lc_sysSymlink(f, l), 0);
	EXPECT_EQ(unlink(l), 0);
	EXPECT_EQ(rawOpen(l), -ENOENT);
	EXPECT_TRUE(rawOpen(f) >= 0);
	(void)lc_sysUnlink(f);
}

/* A dangling link can be removed too. */
TEST_OPT(removes_a_dangling_link, .ref = "XSH unlink()")
{
	char	l[128];

	(void)lc_tmpPath(l, sizeof(l), "uld");
	ASSERT_EQ(lc_sysSymlink("/no/such/file", l), 0);
	EXPECT_EQ(unlink(l), 0);
	EXPECT_EQ(lc_sysSymlink("/no/such/file", l), 0);
	(void)lc_sysUnlink(l);
}

TEST_OPT(missing_file_gives_enoent, .ref = "XSH unlink() ERRORS")
{
	char	f[128];

	ASSERT_TRUE(makeFile(f, sizeof(f), "ul"));
	EXPECT_EQ(unlink(f), 0);
	errno = 0;
	EXPECT_EQ(unlink(f), -1);
	EXPECT_EQ(errno, ENOENT);
	errno = 0;
	EXPECT_EQ(unlink(""), -1);
	EXPECT_EQ(errno, ENOENT);
}

/* A file in the prefix: ENOTDIR, and that file is not changed. */
TEST_OPT(file_in_the_prefix_gives_enotdir, .ref = "XSH unlink() ERRORS")
{
	char	f[128];
	char	p[160];
	size_t	n;

	ASSERT_TRUE(makeFile(f, sizeof(f), "ul"));
	n = lc_strlen(f);
	for (size_t i = 0; i < n; i++)
		p[i] = f[i];
	p[n] = '/';
	p[n + 1] = 'x';
	p[n + 2] = '\0';
	errno = 0;
	EXPECT_EQ(unlink(p), -1);
	EXPECT_EQ(errno, ENOTDIR);
	EXPECT_TRUE(rawOpen(f) >= 0);
	(void)lc_sysUnlink(f);
}

/* A directory: -1, EPERM (not privileged or not supported), the directory kept. */
TEST_OPT(directory_gives_eperm, .ref = "XSH unlink() ERRORS")
{
	char	d[128];

	(void)lc_tmpPath(d, sizeof(d), "uldir");
	ASSERT_EQ(lc_sysMkdir(d, 0700), 0);
	errno = 0;
	EXPECT_EQ(unlink(d), -1);
	EXPECT_EQ(errno, EPERM);
	EXPECT_EQ(lc_sysRmdir(d), 0);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[128];
	char	b[128];

	ASSERT_TRUE(makeFile(a, sizeof(a), "ula"));
	ASSERT_TRUE(makeFile(b, sizeof(b), "ulb"));
	EXPECT_EQ(unlink(a), 0);
	EXPECT_EQ((unlink)(b), 0);
}
