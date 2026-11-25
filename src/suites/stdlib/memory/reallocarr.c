/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file reallocarr.c
 * @brief Tests for reallocarr() (<stdlib.h>, extension).
 * @Created: 2026/11/25 14:17:35 by Moutig
 * @Updated: 2026/11/25 14:18:30 by Moutig
 *
 * reallocarr(ptrp, number, size) resizes the block *ptrp (a void *, maybe
 * a null pointer) to number * size bytes, failing when the product
 * overflows: 0 on success with *ptrp updated, an error number otherwise
 * with *ptrp unchanged (the old block still owned and intact).
 *
 * In no standard (EXT=1). Reference: NetBSD reallocarr(3).
 */
#include "stdlib/stdlib_cases.h"

TEST_OPT(prototype, .ref = "NetBSD reallocarr(3)")
{
	EXPECT_TYPE(&reallocarr, int (*)(void *, size_t, size_t));
}

/* From a null pointer, then grown and shrunk: 0, the contents kept. */
TEST_OPT(resizes_and_keeps_the_contents, .ref = "NetBSD reallocarr(3)")
{
	static const size_t	shapes[][2] = {{1, 16}, {10, 100}, {1000, 9}, {3, 1 << 20}, {70, 70}, {2, 4}};
	unsigned char		*p = NULL;
	size_t				kept = 0;
	int					bad = -1;

	for (int k = 0; k < (int)(sizeof(shapes) / sizeof(shapes[0])) && bad < 0; k++)
	{
		size_t	n = shapes[k][0] * shapes[k][1];

		if (reallocarr(&p, shapes[k][0], shapes[k][1]) != 0 || p == NULL)
			bad = k;
		else
		{
			if (k == 0)
			{
				sl_fill(p, n, 4);
				kept = n;
			}
			if (n < kept)
				kept = n;
			if (!sl_check(p, kept, 4))
				bad = k;
		}
	}
	free(p);
	EXPECT_EQ(bad, -1);
}

/* The product overflows: an error number, *ptrp unchanged and intact. */
TEST_OPT(overflow_is_an_error, .ref = "NetBSD reallocarr(3)")
{
	static const size_t	ov[][2] = {{SIZE_MAX, 2}, {SIZE_MAX / 3 + 1, 3}, {SIZE_MAX, SIZE_MAX}};
	unsigned char		*p = malloc(100);
	unsigned char		*old = p;
	int					bad = -1;

	ASSERT_NOTNULL(p);
	sl_fill(p, 100, 6);
	for (int k = 0; k < 3 && bad < 0; k++)
		if (reallocarr(&p, ov[k][0], ov[k][1]) == 0 || p != old || !sl_check(p, 100, 6))
			bad = k;
	free(p);
	EXPECT_EQ(bad, -1);
}

/* Not enough memory: an error number, *ptrp unchanged and intact. */
TEST_OPT(impossible_size_is_an_error, .ref = "NetBSD reallocarr(3)")
{
	unsigned char	*p = malloc(64);
	unsigned char	*old = p;

	ASSERT_NOTNULL(p);
	sl_fill(p, 64, 2);
	EXPECT_NE(reallocarr(&p, 1, SIZE_MAX), 0);
	EXPECT_PTREQ(p, old);
	EXPECT_NE(reallocarr(&p, SIZE_MAX / 16, 8), 0);
	EXPECT_PTREQ(p, old);
	EXPECT_TRUE(sl_check(p, 64, 2));
	free(p);
}
