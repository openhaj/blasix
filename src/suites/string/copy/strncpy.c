/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strncpy.c
 * @brief Tests for strncpy() (<string.h>).
 * @Created: 2026/11/24 05:23:45 by Moutig
 * @Updated: 2026/11/24 06:12:32 by Moutig
 *
 * strncpy() copies at most n characters of the array s2, stopping at a
 * null character; if s2 is shorter than n, null characters are appended
 * until n characters in all are written. Without a null character in the
 * first n characters, the result is not terminated. It returns s1.
 *
 * Clauses: C99 7.21.2.4; C11, C17 7.24.2.4; C23 7.26.2.5. POSIX: XSH
 * strncpy(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.2.4p1")
{
	EXPECT_TYPE(&strncpy, char *(*)(char *restrict, const char *restrict, size_t));
}

TEST_OPT(pads_with_nul_up_to_n, .ref = "C11 7.24.2.4p3")
{
	char	dst[8] = "xxxxxxx";

	EXPECT_PTREQ(strncpy(dst, "ab", 5), dst);
	EXPECT_MEMEQ(dst, "ab\0\0\0xx", 8);
}

TEST_OPT(no_nul_when_source_is_long, .ref = "C11 7.24.2.4p2 (footnote)")
{
	char	dst[6] = "xxxxx";

	(void)strncpy(dst, "abcdef", 3);
	EXPECT_MEMEQ(dst, "abcxx", 6);
}

TEST_OPT(exact_length_has_no_nul, .ref = "C11 7.24.2.4p2")
{
	char	dst[6] = "xxxxx";

	(void)strncpy(dst, "abc", 3);
	EXPECT_MEMEQ(dst, "abcxx", 6);
}

/* Characters after a null character of s2 are not copied: NULs instead. */
TEST_OPT(bytes_after_nul_not_copied, .ref = "C11 7.24.2.4p2")
{
	char	dst[6] = "xxxxx";

	(void)strncpy(dst, "a\0bcd", 4);
	EXPECT_MEMEQ(dst, "a\0\0\0x", 6);
}

TEST_OPT(zero_length_writes_nothing, .ref = "C11 7.24.1p2")
{
	char	dst[4] = "xxx";

	EXPECT_PTREQ(strncpy(dst, "abc", 0), dst);
	EXPECT_STREQ(dst, "xxx");
}

/* s2 is an array: unterminated is fine when n stops before its end. */
TEST_OPT(source_array_need_not_be_terminated, .ref = "C11 7.24.2.4p2")
{
	char	dst[6] = "xxxxx";
	char	*src = lc_guarded(3);

	ASSERT_NOTNULL(src);
	src[0] = 'a';
	src[1] = 'b';
	src[2] = 'c';
	(void)strncpy(dst, src, 3);
	EXPECT_MEMEQ(dst, "abcxx", 6);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[5] = "xxxx";
	char	b[5] = "xxxx";

	(void)strncpy(a, "a", 3);
	(void)(strncpy)(b, "a", 3);
	EXPECT_MEMEQ(a, b, 5);
}

ST_ERRNO_KEPT("POSIX strncpy()",
	char d[4];
	(void)strncpy(d, "ab", 4);)
