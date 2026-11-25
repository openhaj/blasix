/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file free.c
 * @brief Tests for free() (<stdlib.h>).
 * @Created: 2026/11/25 12:57:36 by Moutig
 * @Updated: 2026/11/25 13:05:04 by Moutig
 *
 * free() deallocates the space ptr points to, which becomes available for
 * further allocation; a null pointer causes no action. Any other argument
 * that does not come from an allocation function, or a space already
 * deallocated, is undefined (not tested). It returns no value.
 * POSIX.1-2024 (CX): free() does not modify errno for a null pointer or a
 * live pointer returned as if by malloc().
 *
 * Only malloc() provides the blocks here: the pointers of calloc(),
 * realloc(), aligned_alloc() ... are freed in their own units.
 *
 * Clauses: C99 7.20.3.2; C11, C17 7.22.3.3; C23 7.24.3.3. POSIX: XSH
 * free().
 */
#include "stdlib/stdlib_cases.h"

TEST_OPT(prototype, .ref = "C11 7.22.3.3p1")
{
	EXPECT_TYPE(&free, void (*)(void *));
}

TEST_OPT(null_pointer_does_nothing, .ref = "C11 7.22.3.3p2")
{
	void	*p = malloc(32);

	ASSERT_NOTNULL(p);
	sl_fill(p, 32, 7);
	free(NULL);
	free((void *)0);
	EXPECT_TRUE(sl_check(p, 32, 7));
	free(p);
}

/* Blocks of every size class can be freed (each one once). */
TEST_OPT(frees_every_size_class, .ref = "C11 7.22.3.3p2")
{
	int	n = 0;

	for (int i = 0; sl_sizes(i); i++)
	{
		void	*p = malloc(sl_sizes(i));

		ASSERT_NOTNULL(p);
		sl_fill(p, sl_sizes(i), (unsigned)i);
		free(p);
		n++;
	}
	EXPECT_GT(n, 1100);
}

/* What malloc(0) returns, null or not, can be passed to free(). */
TEST_OPT(frees_a_zero_size_allocation, .ref = "C11 7.22.3p1")
{
	void	*p = malloc(0);
	void	*q;

	free(p);
	q = malloc(8);
	EXPECT_NOTNULL(q);
	free(q);
}

/*
 * Freeing some blocks leaves the others intact: in allocation order, in
 * reverse order and one in two, with blocks of mixed sizes.
 */
TEST_OPT(other_blocks_are_untouched, .ref = "C11 7.22.3.3p2")
{
	static void		*p[600];
	static size_t	n[600];
	int				bad = -1;

	for (int order = 0; order < 3 && bad < 0; order++)
	{
		for (int i = 0; i < 600; i++)
		{
			n[i] = sl_sizes((i * 7 + order) % 1127);
			p[i] = malloc(n[i]);
			ASSERT_NOTNULL(p[i]);
			sl_fill(p[i], n[i], (unsigned)i);
		}
		for (int k = 0; k < 600; k++)
		{
			int	i = order == 0 ? k : order == 1 ? 599 - k : (k * 2) % 600 + (k >= 300);

			free(p[i]);
			p[i] = NULL;
			for (int j = 0; j < 600 && bad < 0 && k % 50 == 0; j++)
				if (p[j] && !sl_check(p[j], n[j], (unsigned)j))
					bad = order * 1000 + j;
		}
	}
	EXPECT_EQ(bad, -1);
}

/* The freed space can be allocated again, over and over. */
TEST_OPT(space_can_be_allocated_again, .ref = "C11 7.22.3.3p2")
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
			free(p);
		}
	}
	EXPECT_EQ(bad, -1);
}

/* POSIX.1-2024 (CX): errno is not modified, for NULL or a live block. */
TEST_OPT(errno_unchanged, .since = "POSIX.1-2024", .ref = "XSH free() (Issue 8)")
{
	void	*small = malloc(16);
	void	*big = malloc((size_t)4 << 20);

	ASSERT_NOTNULL(small);
	ASSERT_NOTNULL(big);
	errno = EDOM;
	free(NULL);
	EXPECT_EQ(errno, EDOM);
	free(small);
	EXPECT_EQ(errno, EDOM);
	free(big);
	EXPECT_EQ(errno, EDOM);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	void	*a = malloc(8);
	void	*b = malloc(8);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	free(a);
	(free)(b);
	(free)(NULL);
	EXPECT_TRUE(1);
}
