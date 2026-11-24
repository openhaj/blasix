/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memmove_s.c
 * @brief Tests for memmove_s() (<string.h>, Annex K).
 * @Created: 2026/11/24 05:17:28 by Moutig
 * @Updated: 2026/11/24 05:44:35 by Moutig
 *
 * memmove_s() copies n characters as if through a temporary array (the
 * objects may overlap) and returns 0. Violations (s1 or s2 null, sizes >
 * RSIZE_MAX, n > s1max) return nonzero and zero the first s1max characters
 * of s1 when it is usable.
 *
 * Annex K is a conditional feature (OPTS=LIB_EXT1): only an
 * implementation that defines __STDC_LIB_EXT1__ provides it, and only when
 * __STDC_WANT_LIB_EXT1__ is 1 before the header. Runtime-constraint
 * violations go to the handler, set here to ignore_handler_s (<stdlib.h>)
 * so that the function returns (the default handler may abort).
 *
 * Clauses: C11, C17 K.3.7.1.2; C23 K.3.7.1.2.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 K.3.7.1.2p1")
{
	EXPECT_TYPE(&memmove_s, errno_t (*)(void *, rsize_t, const void *, rsize_t));
}

TEST_OPT(overlapping_copy, .ref = "C11 K.3.7.1.2p4-5")
{
	char	b[11] = "0123456789";

	ST_EXT1_IGNORE();
	EXPECT_EQ(memmove_s(b + 2, 8, b, 6), 0);
	EXPECT_STREQ(b, "0101234589");
	EXPECT_EQ(memmove_s(b, 10, b + 3, 6), 0);
	EXPECT_STREQ(b, "1234584589");
}

TEST_OPT(n_greater_than_s1max_zeroes_s1, .ref = "C11 K.3.7.1.2p2-3")
{
	char	d[4] = "xxx";

	ST_EXT1_IGNORE();
	EXPECT_NE(memmove_s(d, 3, "abcd", 4), 0);
	EXPECT_MEMEQ(d, "\0\0\0", 4);
}

TEST_OPT(null_source_zeroes_s1, .ref = "C11 K.3.7.1.2p2-3")
{
	char	d[4] = "xxx";

	ST_EXT1_IGNORE();
	EXPECT_NE(memmove_s(d, 3, NULL, 1), 0);
	EXPECT_MEMEQ(d, "\0\0\0", 4);
}

BITS_LIB_EXT1_TESTS
