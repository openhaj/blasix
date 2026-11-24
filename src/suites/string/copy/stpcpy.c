/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file stpcpy.c
 * @brief Tests for stpcpy() (<string.h>).
 * @Created: 2026/11/24 05:18:32 by Moutig
 * @Updated: 2026/11/24 05:48:44 by Moutig
 *
 * stpcpy() (POSIX.1-2008) copies the string s2, terminating NUL included,
 * into s1 like strcpy(), and returns a pointer to the terminating NUL
 * copied into s1.
 *
 * POSIX: XSH strcpy() (stpcpy, CX, POSIX.1-2008 on); POSIX.1-2024: errno
 * unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX stpcpy() SYNOPSIS")
{
	EXPECT_TYPE(&stpcpy, char *(*)(char *restrict, const char *restrict));
}

TEST_OPT(returns_end_of_copy, .ref = "POSIX stpcpy() RETURN VALUE")
{
	char	dst[8] = "xxxxxxx";

	EXPECT_PTREQ(stpcpy(dst, "abc"), dst + 3);
	EXPECT_MEMEQ(dst, "abc\0xxx", 8);
}

TEST_OPT(empty_string_returns_s1, .ref = "POSIX stpcpy() RETURN VALUE")
{
	char	dst[4] = "xxx";

	EXPECT_PTREQ(stpcpy(dst, ""), dst);
	EXPECT_EQ(dst[0], '\0');
}

/* The result chains: stpcpy(stpcpy(d, a), b) concatenates. */
TEST_OPT(chained_calls_concatenate, .ref = "POSIX stpcpy() RETURN VALUE")
{
	char	dst[16];

	(void)stpcpy(stpcpy(stpcpy(dst, "ab"), ""), "cde");
	EXPECT_STREQ(dst, "abcde");
}

TEST_OPT(every_size, .ref = "POSIX stpcpy() DESCRIPTION")
{
	char	src[72];
	char	dst[80];
	int		bad = -1;

	for (int len = 0; len <= 64 && bad < 0; len++)
	{
		for (int i = 0; i < len; i++)
			src[i] = (char)('A' + i % 26);
		src[len] = '\0';
		lc_fill(dst, 'x', sizeof(dst));
		if (stpcpy(dst + 1, src) != dst + 1 + len || dst[0] != 'x'
			|| dst[1 + len] != '\0' || dst[2 + len] != 'x')
			bad = len;
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX stpcpy() SYNOPSIS")
{
	char	a[4];
	char	b[4];

	EXPECT_PTREQ(stpcpy(a, "ab"), a + 2);
	EXPECT_PTREQ((stpcpy)(b, "ab"), b + 2);
}

ST_ERRNO_KEPT("POSIX stpcpy()",
	char d[4];
	(void)stpcpy(d, "abc");)
