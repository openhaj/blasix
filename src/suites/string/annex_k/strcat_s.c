/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcat_s.c
 * @brief Tests for strcat_s() (<string.h>, Annex K).
 * @Created: 2026/11/24 05:18:48 by Moutig
 * @Updated: 2026/11/24 05:50:11 by Moutig
 *
 * strcat_s() appends the string s2 to s1 and returns 0. With m = s1max -
 * strnlen_s(s1, s1max): violations (null pointers, s1max 0 or >
 * RSIZE_MAX, m 0 i.e. s1 unterminated, m <= strnlen_s(s2, m)) return
 * nonzero and set s1[0] to '\0' when s1 is usable.
 *
 * Annex K is a conditional feature (OPTS=LIB_EXT1): only an
 * implementation that defines __STDC_LIB_EXT1__ provides it, and only when
 * __STDC_WANT_LIB_EXT1__ is 1 before the header. Runtime-constraint
 * violations go to the handler, set here to ignore_handler_s (<stdlib.h>)
 * so that the function returns (the default handler may abort).
 *
 * Clauses: C11, C17 K.3.7.2.1; C23 K.3.7.2.1.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 K.3.7.2.1p1")
{
	EXPECT_TYPE(&strcat_s, errno_t (*)(char *restrict, rsize_t, const char *restrict));
}

TEST_OPT(appends_and_returns_zero, .ref = "C11 K.3.7.2.1p5-7")
{
	char	d[8] = "ab";

	ST_EXT1_IGNORE();
	EXPECT_EQ(strcat_s(d, sizeof(d), "cde"), 0);
	EXPECT_STREQ(d, "abcde");
}

TEST_OPT(no_room_clears_s1, .ref = "C11 K.3.7.2.1p3-4")
{
	char	d[5] = "ab";

	ST_EXT1_IGNORE();
	EXPECT_NE(strcat_s(d, sizeof(d), "cde"), 0);
	EXPECT_EQ(d[0], '\0');
}

TEST_OPT(unterminated_s1_fails, .ref = "C11 K.3.7.2.1p3")
{
	char	d[4] = {'a', 'b', 'c', 'd'};

	ST_EXT1_IGNORE();
	EXPECT_NE(strcat_s(d, sizeof(d), "e"), 0);
	EXPECT_EQ(d[0], '\0');
}

TEST_OPT(null_source_clears_s1, .ref = "C11 K.3.7.2.1p3-4")
{
	char	d[8] = "ab";

	ST_EXT1_IGNORE();
	EXPECT_NE(strcat_s(d, sizeof(d), NULL), 0);
	EXPECT_EQ(d[0], '\0');
}

BITS_LIB_EXT1_TESTS
