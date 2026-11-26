/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file free_sized.c
 * @brief Tests for free_sized() (<stdlib.h>, C23).
 * @Created: 2026/11/25 13:47:59 by Moutig
 * @Updated: 2026/11/25 13:48:18 by Moutig
 *
 * free_sized(ptr, size), with ptr a null pointer or the result of
 * malloc(), realloc() or calloc() and size the requested allocation size,
 * is equivalent to free(ptr); anything else (another size, a block of
 * aligned_alloc()) is undefined (not tested). An implementation may
 * ignore size and call free().
 *
 * Clauses: C23 7.24.3.4. Not in POSIX.1-2024 (built on C17).
 */
#include "stdlib/stdlib_cases.h"

TEST_OPT(prototype, .ref = "C23 7.24.3.4p1")
{
	EXPECT_TYPE(&free_sized, void (*)(void *, size_t));
}

/* A null pointer: no action, whatever the size. */
TEST_OPT(null_pointer_does_nothing, .ref = "C23 7.24.3.4p2")
{
	void	*p = malloc(32);

	ASSERT_NOTNULL(p);
	sl_fill(p, 32, 4);
	free_sized(NULL, 0);
	free_sized(NULL, 32);
	free_sized(NULL, SIZE_MAX);
	EXPECT_TRUE(sl_check(p, 32, 4));
	free(p);
}

/* Blocks of malloc() of every size class, freed with their size. */
TEST_OPT(frees_malloc_blocks_of_every_size, .ref = "C23 7.24.3.4p2")
{
	int	n = 0;

	for (int i = 0; sl_sizes(i); i++)
	{
		void	*p = malloc(sl_sizes(i));

		ASSERT_NOTNULL(p);
		sl_fill(p, sl_sizes(i), (unsigned)i);
		free_sized(p, sl_sizes(i));
		n++;
	}
	EXPECT_GT(n, 1100);
}

/* calloc() blocks, freed with nmemb * size (the requested size). */
TEST_OPT(frees_calloc_blocks, .ref = "C23 7.24.3.4p2")
{
	static const size_t	shapes[][2] = {{1, 1}, {3, 7}, {100, 10}, {16, 4096}, {3, 1 << 20}};

	for (int k = 0; k < (int)(sizeof(shapes) / sizeof(shapes[0])); k++)
	{
		void	*p = calloc(shapes[k][0], shapes[k][1]);

		ASSERT_NOTNULL(p);
		free_sized(p, shapes[k][0] * shapes[k][1]);
	}
	EXPECT_TRUE(1);
}

/* A block resized by realloc(), freed with the size of the last request. */
TEST_OPT(frees_realloc_blocks, .ref = "C23 7.24.3.4p2")
{
	static const size_t	steps[] = {10, 3000, 70000, (size_t)2 << 20, 40};
	void				*p = NULL;

	for (int k = 0; k < (int)(sizeof(steps) / sizeof(steps[0])); k++)
	{
		void	*q = realloc(p, steps[k]);

		ASSERT_NOTNULL(q);
		p = q;
		free_sized(realloc(NULL, steps[k] + 1), steps[k] + 1);
	}
	free_sized(p, steps[4]);
	EXPECT_TRUE(1);
}

/* What malloc(0) returns, null or not, freed with size 0. */
TEST_OPT(frees_a_zero_size_allocation, .ref = "C23 7.24.3.4p2")
{
	void	*p = malloc(0);
	void	*q;

	free_sized(p, 0);
	q = malloc(8);
	EXPECT_NOTNULL(q);
	free_sized(q, 8);
}

/* As free(): the other blocks are untouched, in any order. */
TEST_OPT(other_blocks_are_untouched, .ref = "C23 7.24.3.4p2")
{
	static void		*p[400];
	static size_t	n[400];
	int				bad = -1;

	for (int i = 0; i < 400; i++)
	{
		n[i] = sl_sizes(i * 7 % 1127);
		p[i] = malloc(n[i]);
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int k = 0; k < 400; k++)
	{
		int	i = (k * 2) % 400 + (k >= 200);

		free_sized(p[i], n[i]);
		p[i] = NULL;
		for (int j = 0; j < 400 && bad < 0 && k % 40 == 0; j++)
			if (p[j] && !sl_check(p[j], n[j], (unsigned)j))
				bad = j;
	}
	EXPECT_EQ(bad, -1);
}

/* As free(): the space can be allocated again, over and over. */
TEST_OPT(space_can_be_allocated_again, .ref = "C23 7.24.3.4p2")
{
	int	bad = -1;

	for (int round = 0; round < 2000 && bad < 0; round++)
	{
		size_t	n = round % 2 ? (size_t)1 << 20 : 100 + (size_t)round;
		void	*p = malloc(n);

		if (p == NULL)
			bad = round;
		else
		{
			sl_fill(p, n, (unsigned)round);
			if (!sl_check(p, n, (unsigned)round))
				bad = round;
			free_sized(p, n);
		}
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	void	*a = malloc(8);
	void	*b = malloc(8);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	free_sized(a, 8);
	(free_sized)(b, 8);
	(free_sized)(NULL, 0);
	EXPECT_TRUE(1);
}
