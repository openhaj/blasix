/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strncpy_s.c
 * @brief Tests for strncpy_s() (<string.h>, Annex K).
 * @Created: 2026/11/24 05:23:36 by Moutig
 * @Updated: 2026/11/24 06:12:23 by Moutig
 *
 * strncpy_s() copies at most n characters of s2 (stopping at a null
 * character) and always terminates s1; it returns 0. Violations (null
 * pointers, sizes > RSIZE_MAX, s1max 0, n >= s1max with s1max <=
 * strnlen_s(s2, s1max)) return nonzero and set s1[0] to '\0'.
 *
 * Annex K is a conditional feature (OPTS=LIB_EXT1): only an
 * implementation that defines __STDC_LIB_EXT1__ provides it, and only when
 * __STDC_WANT_LIB_EXT1__ is 1 before the header. Runtime-constraint
 * violations go to the handler, set here to ignore_handler_s (<stdlib.h>)
 * so that the function returns (the default handler may abort).
 *
 * Clauses: C11, C17 K.3.7.1.4 (EXAMPLE 1 is checked as given); C23
 * K.3.7.1.4.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 K.3.7.1.4p1")
{
	EXPECT_TYPE(&strncpy_s, errno_t (*)(char *restrict, rsize_t, const char *restrict, rsize_t));
}

TEST_OPT(standard_example_fits, .ref = "C11 K.3.7.1.4p7")
{
	char	src1[100] = "hello";
	char	dst1[6];

	ST_EXT1_IGNORE();
	EXPECT_EQ(strncpy_s(dst1, 6, src1, 100), 0);
	EXPECT_MEMEQ(dst1, "hello", 6);
}

TEST_OPT(standard_example_too_long, .ref = "C11 K.3.7.1.4p7")
{
	char	src2[7] = {'g', 'o', 'o', 'd', 'b', 'y', 'e'};
	char	dst2[5] = "xxxx";

	ST_EXT1_IGNORE();
	EXPECT_NE(strncpy_s(dst2, 5, src2, 7), 0);
	EXPECT_EQ(dst2[0], '\0');
}

TEST_OPT(standard_example_truncated, .ref = "C11 K.3.7.1.4p7")
{
	char	src2[7] = {'g', 'o', 'o', 'd', 'b', 'y', 'e'};
	char	dst3[5];

	ST_EXT1_IGNORE();
	EXPECT_EQ(strncpy_s(dst3, 5, src2, 4), 0);
	EXPECT_MEMEQ(dst3, "good", 5);
}

TEST_OPT(null_source_clears_s1, .ref = "C11 K.3.7.1.4p2-3")
{
	char	d[4] = "xxx";

	ST_EXT1_IGNORE();
	EXPECT_NE(strncpy_s(d, sizeof(d), NULL, 1), 0);
	EXPECT_EQ(d[0], '\0');
}

BITS_LIB_EXT1_TESTS
