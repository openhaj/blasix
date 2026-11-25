/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file reallocf.c
 * @brief Tests for reallocf() (<stdlib.h>, extension).
 * @Created: 2026/11/25 14:18:22 by Moutig
 * @Updated: 2026/11/25 14:18:57 by Moutig
 *
 * reallocf() is identical to realloc() except that it frees the passed
 * pointer when the requested memory cannot be allocated: a null pointer
 * is then returned and the old block is gone (never to be used again).
 *
 * In no standard (EXT=1). Reference: FreeBSD malloc(3).
 */
#include "stdlib/stdlib_cases.h"

TEST_OPT(prototype, .ref = "FreeBSD malloc(3)")
{
	EXPECT_TYPE(&reallocf, void *(*)(void *, size_t));
}

/* A null pointer: like malloc(). */
TEST_OPT(null_pointer_behaves_like_malloc, .ref = "FreeBSD malloc(3)")
{
	size_t	bad = 0;

	for (int i = 0; sl_sizes(i) && !bad; i += 5)
	{
		void	*p = reallocf(NULL, sl_sizes(i));

		if (p == NULL)
			bad = sl_sizes(i);
		else
		{
			sl_fill(p, sl_sizes(i), (unsigned)i);
			if (!sl_check(p, sl_sizes(i), (unsigned)i))
				bad = sl_sizes(i);
			free(p);
		}
	}
	EXPECT_UEQ(bad, 0);
}

/* As realloc(): the contents up to the lesser size are kept. */
TEST_OPT(contents_kept_up_to_the_lesser_size, .ref = "FreeBSD malloc(3)")
{
	static const size_t	steps[] = {16, 1000, 9000, 70000, 600000, (size_t)8 << 20, 300000, 500, 8};
	unsigned char		*p = malloc(16);
	size_t				kept = 16;
	int					bad = -1;

	ASSERT_NOTNULL(p);
	sl_fill(p, 16, 9);
	for (int k = 1; k < (int)(sizeof(steps) / sizeof(steps[0])) && bad < 0; k++)
	{
		unsigned char	*q = reallocf(p, steps[k]);

		ASSERT_NOTNULL(q);
		p = q;
		if (steps[k] < kept)
			kept = steps[k];
		if (!sl_check(p, kept, 9))
			bad = k;
	}
	free(p);
	EXPECT_EQ(bad, -1);
}

/*
 * No space for the new object: a null pointer (the old block is freed by
 * reallocf(), so it is not used again); the next calls still work.
 */
TEST_OPT(failure_returns_null_and_frees, .ref = "FreeBSD malloc(3)")
{
	static const size_t	huge[] = {SIZE_MAX, SIZE_MAX - 4095, SIZE_MAX / 2 + 1};
	int					bad = -1;

	for (int k = 0; k < 3 && bad < 0; k++)
	{
		void	*p = malloc(k ? (size_t)2 << 20 : 40);
		void	*q;

		ASSERT_NOTNULL(p);
		if (reallocf(p, huge[k]) != NULL)
			bad = k;
		q = malloc(64);
		if (q == NULL)
			bad = 10 + k;
		free(q);
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	void	*a = reallocf(NULL, 8);
	void	*b = (reallocf)(NULL, 8);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	EXPECT_PTRNE(a, b);
	free(a);
	free(b);
}
