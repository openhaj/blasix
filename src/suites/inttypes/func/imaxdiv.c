/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file imaxdiv.c
 * @brief Tests for imaxdiv() (<inttypes.h>).
 * @Created: 2026/11/24 12:17:34 by Moutig
 * @Updated: 2026/11/24 12:19:31 by Moutig
 *
 * imaxdiv() computes numer / denom and numer % denom in one operation and
 * returns them in the quot and rem members (intmax_t) of an imaxdiv_t.
 * Division truncates toward zero (C 6.5.5p6), so quot * denom + rem ==
 * numer. Unrepresentable parts (INTMAX_MIN / -1) and a zero divisor are
 * undefined: not tested.
 *
 * Clauses: C99, C11, C17 7.8.2.2; C23 7.8.2.2. POSIX: XSH imaxdiv().
 */
#include "inttypes/inttypes_cases.h"

TEST_OPT(prototype, .ref = "C11 7.8.2.2p1")
{
	EXPECT_TYPE(&imaxdiv, imaxdiv_t (*)(intmax_t, intmax_t));
}

TEST_OPT(quotient_and_remainder, .ref = "C11 7.8.2.2p2-3")
{
	imaxdiv_t	r = imaxdiv(7, 2);

	EXPECT_TRUE(r.quot == 3);
	EXPECT_TRUE(r.rem == 1);
}

TEST_OPT(truncates_toward_zero, .ref = "C11 6.5.5p6")
{
	imaxdiv_t	a = imaxdiv(-7, 2);
	imaxdiv_t	b = imaxdiv(7, -2);
	imaxdiv_t	c = imaxdiv(-7, -2);

	EXPECT_TRUE(a.quot == -3 && a.rem == -1);
	EXPECT_TRUE(b.quot == -3 && b.rem == 1);
	EXPECT_TRUE(c.quot == 3 && c.rem == -1);
}

TEST_OPT(identity_holds_on_wide_values, .ref = "C11 6.5.5p6")
{
	static const intmax_t	n[] = {INTMAX_MAX, INTMAX_MIN, INTMAX_MAX / 7, -12345, 0};
	static const intmax_t	d[] = {2, 3, -1000003, 7, INTMAX_MAX};
	int						bad = -1;

	for (int i = 0; i < 5 && bad < 0; i++)
		for (int j = 0; j < 5 && bad < 0; j++)
		{
			imaxdiv_t	r = imaxdiv(n[i], d[j]);

			if (r.quot != n[i] / d[j] || r.rem != n[i] % d[j])
				bad = i * 5 + j;
		}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(members_are_intmax_t, .ref = "C11 7.8.2.2p3")
{
	imaxdiv_t	r = imaxdiv(9, 4);

	EXPECT_TYPE(r.quot, intmax_t);
	EXPECT_TYPE(r.rem, intmax_t);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	imaxdiv_t	a = imaxdiv(100, 7);
	imaxdiv_t	b = (imaxdiv)(100, 7);

	EXPECT_TRUE(a.quot == b.quot && a.rem == b.rem);
}
