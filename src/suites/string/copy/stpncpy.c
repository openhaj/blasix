/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file stpncpy.c
 * @brief Tests for stpncpy() (<string.h>).
 * @Created: 2026/11/24 05:18:40 by Moutig
 * @Updated: 2026/11/24 05:49:29 by Moutig
 *
 * stpncpy() (POSIX.1-2008) copies like strncpy(): at most n bytes,
 * stopping at a NUL, padding with NULs up to n. It returns the address of
 * the first NUL written to s1, or &s1[n] when no NUL was written.
 *
 * POSIX: XSH strncpy() (stpncpy, CX, POSIX.1-2008 on); POSIX.1-2024:
 * errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX stpncpy() SYNOPSIS")
{
	EXPECT_TYPE(&stpncpy, char *(*)(char *restrict, const char *restrict, size_t));
}

TEST_OPT(returns_first_nul_written, .ref = "POSIX stpncpy() RETURN VALUE")
{
	char	dst[8] = "xxxxxxx";

	EXPECT_PTREQ(stpncpy(dst, "ab", 5), dst + 2);
	EXPECT_MEMEQ(dst, "ab\0\0\0xx", 8);
}

TEST_OPT(returns_s1_plus_n_without_nul, .ref = "POSIX stpncpy() RETURN VALUE")
{
	char	dst[6] = "xxxxx";

	EXPECT_PTREQ(stpncpy(dst, "abcdef", 3), dst + 3);
	EXPECT_MEMEQ(dst, "abcxx", 6);
}

TEST_OPT(exact_length_returns_s1_plus_n, .ref = "POSIX stpncpy() RETURN VALUE")
{
	char	dst[6] = "xxxxx";

	EXPECT_PTREQ(stpncpy(dst, "abc", 3), dst + 3);
	EXPECT_MEMEQ(dst, "abcxx", 6);
}

TEST_OPT(zero_length_returns_s1, .ref = "POSIX stpncpy() RETURN VALUE")
{
	char	dst[4] = "xxx";

	EXPECT_PTREQ(stpncpy(dst, "abc", 0), dst);
	EXPECT_STREQ(dst, "xxx");
}

TEST_OPT(source_array_need_not_be_terminated, .ref = "POSIX stpncpy() DESCRIPTION")
{
	char	dst[6] = "xxxxx";
	char	*src = lc_guarded(3);

	ASSERT_NOTNULL(src);
	src[0] = 'a';
	src[1] = 'b';
	src[2] = 'c';
	EXPECT_PTREQ(stpncpy(dst, src, 3), dst + 3);
	EXPECT_MEMEQ(dst, "abcxx", 6);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX stpncpy() SYNOPSIS")
{
	char	a[5] = "xxxx";
	char	b[5] = "xxxx";

	EXPECT_PTREQ(stpncpy(a, "a", 3), a + 1);
	EXPECT_PTREQ((stpncpy)(b, "a", 3), b + 1);
	EXPECT_MEMEQ(a, b, 5);
}

ST_ERRNO_KEPT("POSIX stpncpy()",
	char d[4];
	(void)stpncpy(d, "ab", 4);)
