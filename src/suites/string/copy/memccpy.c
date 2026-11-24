/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memccpy.c
 * @brief Tests for memccpy() (<string.h>).
 * @Created: 2026/11/24 05:16:15 by Moutig
 * @Updated: 2026/11/24 05:40:42 by Moutig
 *
 * memccpy() copies from s2 to s1, stopping after the first c (converted
 * to unsigned char) is copied or after n characters. It returns a pointer
 * to the character after the copy of c in s1, or a null pointer if c is
 * not in the first n characters of s2.
 *
 * Clauses: C23 7.26.2.2. POSIX: XSH memccpy() (XSI in every edition);
 * POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C23 7.26.2.2p1")
{
	EXPECT_TYPE(&memccpy, void *(*)(void *restrict, const void *restrict, int, size_t));
}

TEST_OPT(stops_after_c, .ref = "C23 7.26.2.2p2-3")
{
	char	dst[8] = "xxxxxxx";

	EXPECT_PTREQ(memccpy(dst, "ab:cd", ':', 5), dst + 3);
	EXPECT_STREQ(dst, "ab:xxxx");
}

TEST_OPT(not_found_copies_n_and_returns_null, .ref = "C23 7.26.2.2p3")
{
	char	dst[8] = "xxxxxxx";

	EXPECT_NULL(memccpy(dst, "abcdef", ':', 4));
	EXPECT_STREQ(dst, "abcdxxx");
}

/* c beyond the first n characters is not found. */
TEST_OPT(c_after_n_is_not_found, .ref = "C23 7.26.2.2p3")
{
	char	dst[8] = "xxxxxxx";

	EXPECT_NULL(memccpy(dst, "abc:", ':', 3));
	EXPECT_STREQ(dst, "abcxxxx");
}

TEST_OPT(c_converted_to_unsigned_char, .ref = "C23 7.26.2.2p2")
{
	char	dst[8] = "xxxxxxx";
	char	src[4] = {'a', '\xe9', 'b', 'c'};

	EXPECT_PTREQ(memccpy(dst, src, 0xe9 + 256, 4), dst + 2);
	EXPECT_PTREQ(memccpy(dst, src, (char)0xe9, 4), dst + 2);
}

/* NUL is an ordinary byte: memccpy(d, s, '\0', n) copies a string. */
TEST_OPT(nul_can_be_the_stop_byte, .ref = "C23 7.26.2.2p2")
{
	char	dst[8] = "xxxxxxx";

	EXPECT_PTREQ(memccpy(dst, "ab\0cd", '\0', 5), dst + 3);
	EXPECT_MEMEQ(dst, "ab\0xxxx", 8);
}

TEST_OPT(zero_length_copies_nothing, .ref = "C23 7.26.1p3")
{
	char	dst[4] = "xxx";

	EXPECT_NULL(memccpy(dst, "abc", 'a', 0));
	EXPECT_STREQ(dst, "xxx");
}

TEST_OPT(macro_and_function_agree, .ref = "C23 7.1.4p1")
{
	char	a[6] = "xxxxx";
	char	b[6] = "xxxxx";

	EXPECT_PTREQ(memccpy(a, "ab:c", ':', 4), a + 3);
	EXPECT_PTREQ((memccpy)(b, "ab:c", ':', 4), b + 3);
	EXPECT_STREQ(a, b);
}

ST_ERRNO_KEPT("POSIX memccpy()",
	char d[4];
	(void)memccpy(d, "ab", 'b', 2);)
