/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strncat.c
 * @brief Tests for strncat() (<string.h>).
 * @Created: 2026/11/24 05:23:17 by Moutig
 * @Updated: 2026/11/24 06:12:05 by Moutig
 *
 * strncat() appends at most n characters of the array s2 (stopping at a
 * null character) to the string s1, then always a terminating null
 * character. It returns s1.
 *
 * Clauses: C99 7.21.3.2; C11, C17 7.24.3.2; C23 7.26.3.2. POSIX: XSH
 * strncat(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.3.2p1")
{
	EXPECT_TYPE(&strncat, char *(*)(char *restrict, const char *restrict, size_t));
}

TEST_OPT(appends_at_most_n_then_nul, .ref = "C11 7.24.3.2p2")
{
	char	dst[8] = "ab\0xxxx";

	EXPECT_PTREQ(strncat(dst, "cdef", 2), dst);
	EXPECT_MEMEQ(dst, "abcd\0xx", 8);
}

TEST_OPT(short_source_appended_whole, .ref = "C11 7.24.3.2p2")
{
	char	dst[8] = "ab\0xxxx";

	(void)strncat(dst, "c", 5);
	EXPECT_MEMEQ(dst, "abc\0xxx", 8);
}

/* Only one null character is written, never padding. */
TEST_OPT(no_padding, .ref = "C11 7.24.3.2p2")
{
	char	dst[8] = "a\0xxxxx";

	(void)strncat(dst, "b", 4);
	EXPECT_MEMEQ(dst, "ab\0xxxx", 8);
}

TEST_OPT(zero_length_appends_nothing, .ref = "C11 7.24.3.2p2")
{
	char	dst[6] = "ab\0xx";

	EXPECT_PTREQ(strncat(dst, "cd", 0), dst);
	EXPECT_MEMEQ(dst, "ab\0xx", 6);
}

/* s2 is an array: it need not be terminated within n characters. */
TEST_OPT(source_array_need_not_be_terminated, .ref = "C11 7.24.3.2p2")
{
	char	dst[8] = "ab\0xxxx";
	char	*src = lc_guarded(2);

	ASSERT_NOTNULL(src);
	src[0] = 'c';
	src[1] = 'd';
	(void)strncat(dst, src, 2);
	EXPECT_MEMEQ(dst, "abcd\0xx", 8);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[6] = "ab";
	char	b[6] = "ab";

	(void)strncat(a, "cde", 2);
	(void)(strncat)(b, "cde", 2);
	EXPECT_STREQ(a, b);
}

ST_ERRNO_KEPT("POSIX strncat()",
	char d[6] = "ab";
	(void)strncat(d, "cd", 1);)
