/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file openat.c
 * @brief Tests for openat() (<fcntl.h>, POSIX.1-2008).
 * @Created: 2026/11/28 12:04:32 by Moutig
 * @Updated: 2026/11/28 12:10:59 by Moutig
 *
 * openat(fd, path, oflag, ...) is open(), except that a relative path is
 * resolved from the directory of fd instead of the current directory;
 * AT_FDCWD gives open() itself, and an absolute path ignores fd. Errors:
 * EBADF (relative path, fd neither AT_FDCWD nor open), ENOTDIR (relative
 * path, fd not a directory).
 *
 * Clauses: POSIX: XSH open() (openat).
 */
#include "fcntl/fcntl_cases.h"

/* The last component of path (after its last '/'). */
static const char	*oa_base(const char *path)
{
	const char	*b = path;

	for (; *path; path++)
		if (*path == '/')
			b = path + 1;
	return (b);
}

/* Open the directory holding path (cut at its last '/'), raw. */
static int	oa_dir(const char *path)
{
	char	d[128];
	int		n = (int)(oa_base(path) - path);

	for (int i = 0; i < n - 1; i++)
		d[i] = path[i];
	d[n > 1 ? n - 1 : 0] = 0;
	if (n == 1)
		d[0] = '/', d[1] = 0;
	return ((int)lc_sysOpen(d, LC_O_RDONLY | LC_O_CLOEXEC, 0));
}

TEST_OPT(prototype, .ref = "XSH open()")
{
	EXPECT_TYPE(&openat, int (*)(int, const char *, int, ...));
}

/* A relative path is looked up from the directory of fd. */
TEST_OPT(relative_path_from_the_directory, .ref = "XSH open()")
{
	char	p[128];
	int		d;
	int		fd;

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "at"), 9), 0);
	d = oa_dir(p);
	ASSERT_GE(d, 0);
	fd = openat(d, oa_base(p), O_RDONLY);
	ASSERT_GE(fd, 0);
	EXPECT_EQ(fc_size(fd), 9);
	(void)lc_sysClose(fd);
	(void)lc_sysClose(d);
	(void)lc_sysUnlink(p);
}

/* O_CREAT from the directory of fd: the file is made there, with the mode less the umask. */
TEST_OPT(o_creat_makes_the_file_in_the_directory, .ref = "XSH open()")
{
	char	p[128];
	int		d;
	int		old = fc_umask(077);
	int		fd;

	d = oa_dir(fc_path(p, sizeof(p), "ac"));
	fd = d < 0 ? -1 : openat(d, oa_base(p), O_WRONLY | O_CREAT | O_EXCL, 0644);
	(void)fc_umask(old);
	ASSERT_GE(fd, 0);
	EXPECT_EQ(fc_perm(p), 0600);
	(void)lc_sysClose(fd);
	(void)lc_sysClose(d);
	(void)lc_sysUnlink(p);
}

/* An absolute path: fd is ignored, even when it is not a descriptor. */
TEST_OPT(absolute_path_ignores_fd, .ref = "XSH open()")
{
	char	p[128];
	int		fd;

	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "ab"), 2), 0);
	fd = openat(fc_lowestFree() + 100, p, O_RDONLY);
	EXPECT_GE(fd, 0);
	(void)lc_sysClose(fd);
	(void)lc_sysUnlink(p);
}

/* AT_FDCWD: relative to the current directory, as open(). */
TEST_OPT(at_fdcwd_is_the_current_directory, .ref = "XSH open()")
{
	int	fd = openat(AT_FDCWD, ".", O_RDONLY);
	fc_st	a;
	fc_st	b;

	ASSERT_GE(fd, 0);
	ASSERT_EQ(fc_stat(fd, 0, 0, &a), 0);
	ASSERT_EQ(fc_stat(-1, ".", 0, &b), 0);
	EXPECT_EQ(a.mode, b.mode);
	EXPECT_EQ(a.nlink, b.nlink);
	(void)lc_sysClose(fd);
}

/* A relative path with fd not open: EBADF; with fd not a directory: ENOTDIR. */
TEST_OPT(bad_fd_gives_ebadf_or_enotdir, .ref = "XSH open()")
{
	char	p[128];
	int		f;

	errno = 0;
	EXPECT_EQ(openat(fc_lowestFree() + 100, "x", O_RDONLY), -1);
	EXPECT_EQ(errno, EBADF);
	ASSERT_EQ(fc_makeFile(fc_path(p, sizeof(p), "an"), 1), 0);
	f = (int)lc_sysOpen(p, LC_O_RDONLY | LC_O_CLOEXEC, 0);
	ASSERT_GE(f, 0);
	errno = 0;
	EXPECT_EQ(openat(f, "x", O_RDONLY), -1);
	EXPECT_EQ(errno, ENOTDIR);
	(void)lc_sysClose(f);
	(void)lc_sysUnlink(p);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ(openat(-1, "", O_RDONLY), (openat)(-1, "", O_RDONLY));
}
