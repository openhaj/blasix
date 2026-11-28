/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file readlink.c
 * @brief Tests for readlink() (<unistd.h>).
 * @Created: 2026/11/28 10:40:27 by Moutig
 * @Updated: 2026/11/28 10:41:13 by Moutig
 *
 * readlink(path, buf, bufsize) places the contents of the symbolic link
 * path in buf, without a terminating null byte, and returns the number of
 * bytes placed; when buf is too small, the first bufsize bytes. -1, errno
 * and buf unchanged otherwise: EINVAL (path is not a symbolic link),
 * ENOENT (a missing component, or an empty path), ENOTDIR (a file in the
 * prefix), ELOOP.
 *
 * The links are made with raw syscalls in /tmp.
 *
 * Clauses: POSIX: XSH readlink().
 */
#include "unistd/unistd_cases.h"

TEST_OPT(prototype, .ref = "XSH readlink()")
{
	EXPECT_TYPE(&readlink, ssize_t (*)(const char *restrict, char *restrict, size_t));
}

/* A link to target at a temporary path (out: path). */
static int	makeLink(char *path, unsigned long size, const char *tag, const char *target)
{
	(void)lc_tmpPath(path, size, tag);
	(void)lc_sysUnlink(path);
	return (lc_sysSymlink(target, path) == 0);
}

/* The contents of the link, byte count returned, no null byte added. */
TEST_OPT(places_the_contents_without_a_null_byte, .ref = "XSH readlink()")
{
	char	l[128];
	char	buf[32];

	ASSERT_TRUE(makeLink(l, sizeof(l), "rl", "some/target"));
	lc_fill(buf, 'Z', sizeof(buf));
	EXPECT_EQ(readlink(l, buf, sizeof(buf)), 11);
	EXPECT_MEMEQ(buf, "some/target", 11);
	EXPECT_EQ(buf[11], 'Z');
	(void)lc_sysUnlink(l);
}

/* The contents as written: a dangling link, an absolute target, odd bytes. */
TEST_OPT(contents_are_not_resolved, .ref = "XSH readlink()")
{
	char	l[128];
	char	buf[64];

	ASSERT_TRUE(makeLink(l, sizeof(l), "rl", "/no/such/../file"));
	EXPECT_EQ(readlink(l, buf, sizeof(buf)), 16);
	EXPECT_MEMEQ(buf, "/no/such/../file", 16);
	(void)lc_sysUnlink(l);
	ASSERT_TRUE(makeLink(l, sizeof(l), "rl", " a\x01\xff "));
	EXPECT_EQ(readlink(l, buf, sizeof(buf)), 5);
	EXPECT_MEMEQ(buf, " a\x01\xff ", 5);
	(void)lc_sysUnlink(l);
}

/* A buffer too small: the first bufsize bytes, bufsize returned. */
TEST_OPT(too_small_buffer_gets_the_first_bytes, .ref = "XSH readlink()")
{
	char	l[128];
	char	buf[8];

	ASSERT_TRUE(makeLink(l, sizeof(l), "rl", "0123456789"));
	lc_fill(buf, 'Z', sizeof(buf));
	EXPECT_EQ(readlink(l, buf, 4), 4);
	EXPECT_MEMEQ(buf, "0123", 4);
	EXPECT_EQ(buf[4], 'Z');
	EXPECT_EQ(readlink(l, buf, 1), 1);
	EXPECT_EQ(buf[0], '0');
	(void)lc_sysUnlink(l);
}

/* Not a symbolic link: -1, EINVAL, the buffer unchanged. */
TEST_OPT(not_a_link_gives_einval, .ref = "XSH readlink() ERRORS")
{
	char	buf[8];

	lc_fill(buf, 'Z', sizeof(buf));
	errno = 0;
	EXPECT_EQ(readlink("/tmp", buf, sizeof(buf)), -1);
	EXPECT_EQ(errno, EINVAL);
	EXPECT_EQ(buf[0], 'Z');
}

TEST_OPT(missing_path_gives_enoent, .ref = "XSH readlink() ERRORS")
{
	char	buf[8];

	errno = 0;
	EXPECT_EQ(readlink("/tmp/lc-no-such-link-here", buf, sizeof(buf)), -1);
	EXPECT_EQ(errno, ENOENT);
	errno = 0;
	EXPECT_EQ(readlink("", buf, sizeof(buf)), -1);
	EXPECT_EQ(errno, ENOENT);
}

/* A link in the prefix that leads to a file: ENOTDIR. */
TEST_OPT(file_in_the_prefix_gives_enotdir, .ref = "XSH readlink() ERRORS")
{
	char	f[128];
	char	p[160];
	long	fd;
	size_t	n;

	(void)lc_tmpPath(f, sizeof(f), "rlf");
	fd = lc_sysOpen(f, LC_O_WRONLY | LC_O_CREAT | LC_O_TRUNC | LC_O_CLOEXEC, 0600);
	ASSERT_TRUE(fd >= 0);
	(void)lc_sysClose((int)fd);
	n = lc_strlen(f);
	for (size_t i = 0; i < n; i++)
		p[i] = f[i];
	p[n] = '/';
	p[n + 1] = 'x';
	p[n + 2] = '\0';
	errno = 0;
	EXPECT_EQ(readlink(p, (char [8]){0}, 8), -1);
	EXPECT_EQ(errno, ENOTDIR);
	(void)lc_sysUnlink(f);
}

/* A loop of links in the prefix: ELOOP. */
TEST_OPT(loop_in_the_prefix_gives_eloop, .ref = "XSH readlink() ERRORS")
{
	char	a[128];
	char	b[128];
	char	p[160];
	size_t	n;

	(void)lc_tmpPath(a, sizeof(a), "rla");
	(void)lc_tmpPath(b, sizeof(b), "rlb");
	ASSERT_EQ(lc_sysSymlink(b, a), 0);
	ASSERT_EQ(lc_sysSymlink(a, b), 0);
	n = lc_strlen(a);
	for (size_t i = 0; i < n; i++)
		p[i] = a[i];
	p[n] = '/';
	p[n + 1] = 'x';
	p[n + 2] = '\0';
	errno = 0;
	EXPECT_EQ(readlink(p, (char [8]){0}, 8), -1);
	EXPECT_EQ(errno, ELOOP);
	(void)lc_sysUnlink(a);
	(void)lc_sysUnlink(b);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	l[128];
	char	a[8];
	char	b[8];

	ASSERT_TRUE(makeLink(l, sizeof(l), "rl", "abc"));
	EXPECT_EQ(readlink(l, a, 8), 3);
	EXPECT_EQ((readlink)(l, b, 8), 3);
	EXPECT_MEMEQ(a, b, 3);
	(void)lc_sysUnlink(l);
}
