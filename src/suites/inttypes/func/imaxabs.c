/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file imaxabs.c
 * @brief Tests for imaxabs() (<inttypes.h>).
 * @Created: 2026/11/24 12:17:24 by Moutig
 * @Updated: 2026/11/24 12:19:24 by Moutig
 *
 * imaxabs() returns the absolute value of j. The result for the most
 * negative value cannot be represented: undefined, not tested.
 *
 * Clauses: C99, C11, C17 7.8.2.1; C23 7.8.2.1. POSIX: XSH imaxabs().
 */
#include "inttypes/inttypes_cases.h"

TEST_OPT(prototype, .ref = "C11 7.8.2.1p1")
{
	EXPECT_TYPE(&imaxabs, intmax_t (*)(intmax_t));
}

TEST_OPT(positive_unchanged, .ref = "C11 7.8.2.1p3")
{
	EXPECT_TRUE(imaxabs(5) == 5);
	EXPECT_TRUE(imaxabs(INTMAX_MAX) == INTMAX_MAX);
}

TEST_OPT(negative_negated, .ref = "C11 7.8.2.1p3")
{
	EXPECT_TRUE(imaxabs(-5) == 5);
	EXPECT_TRUE(imaxabs(-INTMAX_MAX) == INTMAX_MAX);
	EXPECT_TRUE(imaxabs(INTMAX_MIN + 1) == INTMAX_MAX);
}

TEST_OPT(zero, .ref = "C11 7.8.2.1p3")
{
	EXPECT_TRUE(imaxabs(0) == 0);
}

/* Values beyond long: the full width of intmax_t is used. */
TEST_OPT(wide_values, .ref = "C11 7.8.2.1p2")
{
	intmax_t	big = INTMAX_MAX / 3;

	EXPECT_TRUE(imaxabs(-big) == big);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_TRUE(imaxabs(-7) == (imaxabs)(-7));
}
