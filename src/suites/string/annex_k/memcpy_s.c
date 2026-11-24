/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memcpy_s.c
 * @brief Tests for memcpy_s() (<string.h>, Annex K).
 * @Created: 2026/11/24 05:16:49 by Moutig
 * @Updated: 2026/11/24 05:43:23 by Moutig
 *
 * memcpy_s() copies n characters from s2 to s1 and returns 0. Violations
 * (s1 or s2 null, s1max or n > RSIZE_MAX, n > s1max) return nonzero and,
 * when s1 is usable, zero the first s1max characters of s1.
 *
 * Annex K is a conditional feature (OPTS=LIB_EXT1): only an
 * implementation that defines __STDC_LIB_EXT1__ provides it, and only when
 * __STDC_WANT_LIB_EXT1__ is 1 before the header. Runtime-constraint
 * violations go to the handler, set here to ignore_handler_s (<stdlib.h>)
 * so that the function returns (the default handler may abort).
 *
 * Clauses: C11, C17 K.3.7.1.1; C23 K.3.7.1.1.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 K.3.7.1.1p1")
{
	EXPECT_TYPE(&memcpy_s, errno_t (*)(void *restrict, rsize_t, const void *restrict, rsize_t));
}

TEST_OPT(copies_and_returns_zero, .ref = "C11 K.3.7.1.1p4-5")
{
	char	d[6] = "xxxxx";

	ST_EXT1_IGNORE();
	EXPECT_EQ(memcpy_s(d, sizeof(d), "a\0b", 3), 0);
	EXPECT_MEMEQ(d, "a\0bxx", 6);
}

TEST_OPT(n_greater_than_s1max_zeroes_s1, .ref = "C11 K.3.7.1.1p2-3")
{
	char	d[4] = "xxx";

	ST_EXT1_IGNORE();
	EXPECT_NE(memcpy_s(d, 3, "abcd", 4), 0);
	EXPECT_MEMEQ(d, "\0\0\0", 4);
}

TEST_OPT(null_source_zeroes_s1, .ref = "C11 K.3.7.1.1p2-3")
{
	char	d[4] = "xxx";

	ST_EXT1_IGNORE();
	EXPECT_NE(memcpy_s(d, 3, NULL, 1), 0);
	EXPECT_MEMEQ(d, "\0\0\0", 4);
}

TEST_OPT(null_destination_fails, .ref = "C11 K.3.7.1.1p2")
{
	ST_EXT1_IGNORE();
	EXPECT_NE(memcpy_s(NULL, 4, "abc", 1), 0);
}

TEST_OPT(size_above_rsize_max_fails, .ref = "C11 K.3.7.1.1p2")
{
	char	d[4] = "xxx";

	ST_EXT1_IGNORE();
	EXPECT_NE(memcpy_s(d, sizeof(d), "abc", (rsize_t)RSIZE_MAX + 1), 0);
}

TEST_OPT(zero_length_succeeds, .ref = "C11 K.3.7.1.1p4")
{
	char	d[4] = "xxx";

	ST_EXT1_IGNORE();
	EXPECT_EQ(memcpy_s(d, sizeof(d), "abc", 0), 0);
	EXPECT_STREQ(d, "xxx");
}

BITS_LIB_EXT1_TESTS
