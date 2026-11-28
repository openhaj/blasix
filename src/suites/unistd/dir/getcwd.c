/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file getcwd.c
 * @brief Tests for getcwd() (<unistd.h>).
 * @Created: 2026/11/28 10:38:57 by Moutig
 * @Updated: 2026/11/28 10:39:26 by Moutig
 *
 * getcwd(buf, size) places an absolute pathname of the current working
 * directory, without symbolic links (POSIX.1-2024: nor dot, dot-dot,
 * unnecessary slashes), in buf and returns buf; NULL and errno otherwise:
 * EINVAL (size 0), ERANGE (size greater than 0 but smaller than the
 * length + 1). A null buf is unspecified (not tested).
 *
 * The working directory is set with a raw chdir(2); the reference is the
 * kernel's getcwd(2).
 *
 * Clauses: POSIX: XSH getcwd().
 */
#include "unistd/unistd_cases.h"
#include <limits.h>

TEST_OPT(prototype, .ref = "XSH getcwd()")
{
	EXPECT_TYPE(&getcwd, char *(*)(char *, size_t));
}

static int	streq(const char *a, const char *b)
{
	while (*a && *a == *b)
	{
		a++;
		b++;
	}
	return (*a == *b);
}

/* Is getcwd() the kernel's working directory, returned in buf? */
static int	matchesTheKernel(void)
{
	static char	want[PATH_MAX];
	static char	buf[PATH_MAX];

	if (lc_sysGetcwd(want, sizeof(want)) <= 0)
		return (0);
	return (getcwd(buf, sizeof(buf)) == buf && streq(buf, want));
}

TEST_OPT(returns_the_working_directory_in_buf, .ref = "XSH getcwd()")
{
	EXPECT_TRUE(matchesTheKernel());
}

/* After a raw chdir(): the new directory, absolute, with a single leading slash. */
TEST_OPT(follows_chdir, .ref = "XSH getcwd()")
{
	char	d[128];
	char	buf[PATH_MAX];

	(void)lc_tmpPath(d, sizeof(d), "cwd");
	ASSERT_EQ(lc_sysMkdir(d, 0700), 0);
	ASSERT_EQ(lc_sysChdir(d), 0);
	EXPECT_TRUE(matchesTheKernel());
	ASSERT_NOTNULL(getcwd(buf, sizeof(buf)));
	EXPECT_TRUE(buf[0] == '/' && buf[1] != '/');
	ASSERT_EQ(lc_sysChdir("/"), 0);
	EXPECT_TRUE(getcwd(buf, sizeof(buf)) == buf && streq(buf, "/"));
	(void)lc_sysRmdir(d);
}

/* Through a symbolic link: the pathname has no link in it (the kernel's one). */
TEST_OPT(no_symbolic_link_in_the_result, .ref = "XSH getcwd()")
{
	char	d[128];
	char	l[128];

	(void)lc_tmpPath(d, sizeof(d), "cwdd");
	(void)lc_tmpPath(l, sizeof(l), "cwdl");
	ASSERT_EQ(lc_sysMkdir(d, 0700), 0);
	ASSERT_EQ(lc_sysSymlink(d, l), 0);
	ASSERT_EQ(lc_sysChdir(l), 0);
	EXPECT_TRUE(matchesTheKernel());
	(void)lc_sysChdir("/");
	(void)lc_sysUnlink(l);
	(void)lc_sysRmdir(d);
}

/* size exactly length + 1: enough; one less: NULL and ERANGE. */
TEST_OPT(too_small_buffer_gives_erange, .ref = "XSH getcwd() ERRORS")
{
	char	want[PATH_MAX];
	char	buf[PATH_MAX];
	long	n;

	ASSERT_EQ(lc_sysChdir("/tmp"), 0);
	n = lc_sysGetcwd(want, sizeof(want));
	ASSERT_TRUE(n > 1);
	EXPECT_PTREQ(getcwd(buf, (size_t)n), buf);
	errno = 0;
	EXPECT_NULL(getcwd(buf, (size_t)n - 1));
	EXPECT_EQ(errno, ERANGE);
	errno = 0;
	EXPECT_NULL(getcwd(buf, 1));
	EXPECT_EQ(errno, ERANGE);
}

TEST_OPT(zero_size_gives_einval, .ref = "XSH getcwd() ERRORS")
{
	char	buf[8];

	errno = 0;
	EXPECT_NULL(getcwd(buf, 0));
	EXPECT_EQ(errno, EINVAL);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[PATH_MAX];
	char	b[PATH_MAX];

	EXPECT_PTREQ(getcwd(a, sizeof(a)), a);
	EXPECT_PTREQ((getcwd)(b, sizeof(b)), b);
	EXPECT_TRUE(streq(a, b));
}
