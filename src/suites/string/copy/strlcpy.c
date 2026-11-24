/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strlcpy.c
 * @brief Tests for strlcpy() (<string.h>).
 * @Created: 2026/11/24 05:22:51 by Moutig
 * @Updated: 2026/11/24 06:10:27 by Moutig
 *
 * strlcpy() (POSIX.1-2024) copies at most dstsize - 1 bytes of src to dst
 * and NUL-terminates unless dstsize is 0. It returns strlen(src), so a
 * result >= dstsize means truncation.
 *
 * POSIX: XSH strlcpy() (CX, Issue 8); errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX strlcpy() SYNOPSIS")
{
	EXPECT_TYPE(&strlcpy, size_t (*)(char *restrict, const char *restrict, size_t));
}

TEST_OPT(copies_when_it_fits, .ref = "POSIX strlcpy() DESCRIPTION")
{
	char	dst[8] = "xxxxxxx";

	EXPECT_UEQ(strlcpy(dst, "abc", sizeof(dst)), 3);
	EXPECT_MEMEQ(dst, "abc\0xxx", 8);
}

TEST_OPT(truncates_and_terminates, .ref = "POSIX strlcpy() DESCRIPTION")
{
	char	dst[6] = "xxxxx";

	EXPECT_UEQ(strlcpy(dst, "abcdefgh", 4), 8);
	EXPECT_MEMEQ(dst, "abc\0x", 6);
}

TEST_OPT(exact_fit, .ref = "POSIX strlcpy() DESCRIPTION")
{
	char	dst[4] = "xxx";

	EXPECT_UEQ(strlcpy(dst, "abc", 4), 3);
	EXPECT_STREQ(dst, "abc");
}

TEST_OPT(zero_size_writes_nothing, .ref = "POSIX strlcpy() DESCRIPTION")
{
	char	dst[4] = "xxx";

	EXPECT_UEQ(strlcpy(dst, "abc", 0), 3);
	EXPECT_STREQ(dst, "xxx");
}

TEST_OPT(size_one_gives_empty_string, .ref = "POSIX strlcpy() DESCRIPTION")
{
	char	dst[4] = "xxx";

	EXPECT_UEQ(strlcpy(dst, "abc", 1), 3);
	EXPECT_MEMEQ(dst, "\0xx", 4);
}

TEST_OPT(stops_at_nul_of_src, .ref = "POSIX strlcpy() DESCRIPTION")
{
	char	dst[6] = "xxxxx";

	EXPECT_UEQ(strlcpy(dst, "a\0bc", sizeof(dst)), 1);
	EXPECT_MEMEQ(dst, "a\0xxx", 6);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strlcpy() SYNOPSIS")
{
	char	a[4];
	char	b[4];

	EXPECT_UEQ(strlcpy(a, "abcd", 3), (strlcpy)(b, "abcd", 3));
	EXPECT_STREQ(a, b);
}

ST_ERRNO_KEPT("POSIX strlcpy()",
	char d[4];
	(void)strlcpy(d, "abcdef", sizeof(d));)
