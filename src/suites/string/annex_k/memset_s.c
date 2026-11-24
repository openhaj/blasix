/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memset_s.c
 * @brief Tests for memset_s() (<string.h>, Annex K).
 * @Created: 2026/11/24 05:17:52 by Moutig
 * @Updated: 2026/11/24 05:46:09 by Moutig
 *
 * memset_s() copies c (converted to unsigned char) into the first n
 * characters of s and returns 0; the store is never elided. Violations (s
 * null, sizes > RSIZE_MAX, n > smax) return nonzero and, when s is
 * usable, fill the first smax characters with c.
 *
 * Annex K is a conditional feature (OPTS=LIB_EXT1): only an
 * implementation that defines __STDC_LIB_EXT1__ provides it, and only when
 * __STDC_WANT_LIB_EXT1__ is 1 before the header. Runtime-constraint
 * violations go to the handler, set here to ignore_handler_s (<stdlib.h>)
 * so that the function returns (the default handler may abort).
 *
 * Clauses: C11, C17 K.3.7.4.1; C23 K.3.7.4.1.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 K.3.7.4.1p1")
{
	EXPECT_TYPE(&memset_s, errno_t (*)(void *, rsize_t, int, rsize_t));
}

TEST_OPT(fills_and_returns_zero, .ref = "C11 K.3.7.4.1p4-5")
{
	char	d[6] = "abcde";

	ST_EXT1_IGNORE();
	EXPECT_EQ(memset_s(d, sizeof(d), 'z', 3), 0);
	EXPECT_STREQ(d, "zzzde");
}

TEST_OPT(value_converted_to_unsigned_char, .ref = "C11 K.3.7.4.1p4")
{
	unsigned char	d[1] = {0};

	ST_EXT1_IGNORE();
	EXPECT_EQ(memset_s(d, 1, 0x141, 1), 0);
	EXPECT_UEQ(d[0], 0x41);
}

TEST_OPT(n_greater_than_smax_fills_smax, .ref = "C11 K.3.7.4.1p2-3")
{
	char	d[6] = "abcde";

	ST_EXT1_IGNORE();
	EXPECT_NE(memset_s(d, 3, 'q', 5), 0);
	EXPECT_STREQ(d, "qqqde");
}

TEST_OPT(null_pointer_fails, .ref = "C11 K.3.7.4.1p2")
{
	ST_EXT1_IGNORE();
	EXPECT_NE(memset_s(NULL, 4, 0, 1), 0);
}

BITS_LIB_EXT1_TESTS
