/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strcpy_s.c
 * @brief Tests for strcpy_s() (<string.h>, Annex K).
 * @Created: 2026/11/24 05:19:52 by Moutig
 * @Updated: 2026/11/24 05:55:03 by Moutig
 *
 * strcpy_s() copies the string s2 into s1 and returns 0. Violations (s1
 * or s2 null, s1max 0 or > RSIZE_MAX, s1max <= strnlen_s(s2, s1max))
 * return nonzero and set s1[0] to the null character when s1 is usable.
 *
 * Annex K is a conditional feature (OPTS=LIB_EXT1): only an
 * implementation that defines __STDC_LIB_EXT1__ provides it, and only when
 * __STDC_WANT_LIB_EXT1__ is 1 before the header. Runtime-constraint
 * violations go to the handler, set here to ignore_handler_s (<stdlib.h>)
 * so that the function returns (the default handler may abort).
 *
 * Clauses: C11, C17 K.3.7.1.3; C23 K.3.7.1.3.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 K.3.7.1.3p1")
{
	EXPECT_TYPE(&strcpy_s, errno_t (*)(char *restrict, rsize_t, const char *restrict));
}

TEST_OPT(copies_and_returns_zero, .ref = "C11 K.3.7.1.3p4-6")
{
	char	d[8] = "xxxxxxx";

	ST_EXT1_IGNORE();
	EXPECT_EQ(strcpy_s(d, sizeof(d), "abc"), 0);
	EXPECT_STREQ(d, "abc");
}

TEST_OPT(exact_fit, .ref = "C11 K.3.7.1.3p2")
{
	char	d[4] = "xxx";

	ST_EXT1_IGNORE();
	EXPECT_EQ(strcpy_s(d, 4, "abc"), 0);
	EXPECT_STREQ(d, "abc");
}

TEST_OPT(too_small_clears_s1, .ref = "C11 K.3.7.1.3p2-3")
{
	char	d[4] = "xxx";

	ST_EXT1_IGNORE();
	EXPECT_NE(strcpy_s(d, 3, "abc"), 0);
	EXPECT_EQ(d[0], '\0');
}

TEST_OPT(null_source_clears_s1, .ref = "C11 K.3.7.1.3p2-3")
{
	char	d[4] = "xxx";

	ST_EXT1_IGNORE();
	EXPECT_NE(strcpy_s(d, sizeof(d), NULL), 0);
	EXPECT_EQ(d[0], '\0');
}

TEST_OPT(zero_size_fails, .ref = "C11 K.3.7.1.3p2")
{
	char	d[4] = "xxx";

	ST_EXT1_IGNORE();
	EXPECT_NE(strcpy_s(d, 0, "a"), 0);
	EXPECT_STREQ(d, "xxx");
}

BITS_LIB_EXT1_TESTS
