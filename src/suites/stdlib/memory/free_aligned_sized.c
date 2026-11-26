/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file free_aligned_sized.c
 * @brief Tests for free_aligned_sized() (<stdlib.h>, C23).
 * @Created: 2026/11/25 13:47:25 by Moutig
 * @Updated: 2026/11/25 13:47:52 by Moutig
 *
 * free_aligned_sized(ptr, alignment, size), with ptr a null pointer or the
 * result of aligned_alloc() and alignment and size those of the request,
 * is equivalent to free(ptr); anything else (other values, a block of
 * malloc(), calloc() or realloc()) is undefined (not tested). An
 * implementation may ignore alignment and size and call free().
 *
 * Clauses: C23 7.24.3.5. Not in POSIX.1-2024 (built on C17).
 */
#include "stdlib/stdlib_cases.h"

TEST_OPT(prototype, .ref = "C23 7.24.3.5p1")
{
	EXPECT_TYPE(&free_aligned_sized, void (*)(void *, size_t, size_t));
}

/* A null pointer: no action, whatever the alignment and size. */
TEST_OPT(null_pointer_does_nothing, .ref = "C23 7.24.3.5p2")
{
	void	*p = aligned_alloc(64, 64);

	ASSERT_NOTNULL(p);
	sl_fill(p, 64, 2);
	free_aligned_sized(NULL, 0, 0);
	free_aligned_sized(NULL, 64, 64);
	free_aligned_sized(NULL, 3, SIZE_MAX);
	EXPECT_TRUE(sl_check(p, 64, 2));
	free_aligned_sized(p, 64, 64);
}

/*
 * Blocks of aligned_alloc() of every alignment from 1 to 1 MiB (those the
 * implementation supports) and sizes up to 4 MiB, freed with both values.
 */
TEST_OPT(frees_aligned_blocks, .ref = "C23 7.24.3.5p2")
{
	int	freed = 0;

	for (size_t a = 1; a <= ((size_t)1 << 20); a *= 2)
		for (size_t k = 1; k <= 4; k++)
		{
			size_t	n = a * k * (a < 64 ? 1000 : 1);
			void	*p = aligned_alloc(a, n);

			if (p == NULL)
				continue ;
			sl_fill(p, n, (unsigned)k);
			free_aligned_sized(p, a, n);
			freed++;
		}
	EXPECT_GT(freed, 20);
}

/* What aligned_alloc(a, 0) returns, null or not, freed with size 0. */
TEST_OPT(frees_a_zero_size_allocation, .ref = "C23 7.24.3.5p2")
{
	void	*p = aligned_alloc(64, 0);
	void	*q;

	free_aligned_sized(p, 64, 0);
	q = aligned_alloc(64, 64);
	EXPECT_NOTNULL(q);
	free_aligned_sized(q, 64, 64);
}

/* As free(): the other blocks are untouched, in any order. */
TEST_OPT(other_blocks_are_untouched, .ref = "C23 7.24.3.5p2")
{
	static void		*p[200];
	static size_t	n[200];
	static size_t	a[200];
	int				bad = -1;

	for (int i = 0; i < 200; i++)
	{
		a[i] = (size_t)1 << (i % 13);
		n[i] = a[i] * (size_t)(i % 5 + 1);
		p[i] = aligned_alloc(a[i], n[i]);
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int k = 0; k < 200; k++)
	{
		int	i = 199 - (k * 2) % 200 - (k >= 100);

		free_aligned_sized(p[i], a[i], n[i]);
		p[i] = NULL;
		for (int j = 0; j < 200 && bad < 0 && k % 20 == 0; j++)
			if (p[j] && !sl_check(p[j], n[j], (unsigned)j))
				bad = j;
	}
	EXPECT_EQ(bad, -1);
}

/* As free(): the space can be allocated again, over and over. */
TEST_OPT(space_can_be_allocated_again, .ref = "C23 7.24.3.5p2")
{
	int	bad = -1;

	for (int round = 0; round < 2000 && bad < 0; round++)
	{
		size_t	a = (size_t)1 << (round % 12);
		size_t	n = round % 2 ? (size_t)1 << 20 : a * 4;
		void	*p = aligned_alloc(a, n);

		if (p == NULL || (uintptr_t)p % a != 0)
			bad = round;
		else
		{
			sl_fill(p, n, (unsigned)round);
			if (!sl_check(p, n, (unsigned)round))
				bad = round;
		}
		free_aligned_sized(p, a, n);
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	void	*a = aligned_alloc(128, 256);
	void	*b = aligned_alloc(128, 256);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	free_aligned_sized(a, 128, 256);
	(free_aligned_sized)(b, 128, 256);
	(free_aligned_sized)(NULL, 128, 256);
	EXPECT_TRUE(1);
}
