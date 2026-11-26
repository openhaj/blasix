/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file cfree.c
 * @brief Tests for cfree() (<malloc.h>, extension).
 * @Created: 2026/11/25 14:23:36 by Moutig
 * @Updated: 2026/11/25 14:23:50 by Moutig
 *
 * cfree(ptr) is a synonym of free(): it deallocates a block of malloc(),
 * calloc() or realloc(); a null pointer does nothing. Removed from the
 * glibc headers in 2.26.
 *
 * In no standard (EXT=1). Reference: glibc cfree(3).
 */
#include "malloc/malloc_cases.h"

TEST_OPT(prototype, .ref = "glibc cfree(3)")
{
	EXPECT_TYPE(&cfree, void (*)(void *));
}

TEST_OPT(null_pointer_does_nothing, .ref = "glibc cfree(3)")
{
	void	*p = malloc(16);

	ASSERT_NOTNULL(p);
	sl_fill(p, 16, 1);
	cfree(NULL);
	EXPECT_TRUE(sl_check(p, 16, 1));
	cfree(p);
}

/* Blocks of every size class and of calloc() / realloc(), as free(). */
TEST_OPT(frees_like_free, .ref = "glibc cfree(3)")
{
	int	n = 0;

	for (int i = 0; sl_sizes(i); i += 2)
	{
		void	*p = malloc(sl_sizes(i));

		ASSERT_NOTNULL(p);
		sl_fill(p, sl_sizes(i), (unsigned)i);
		cfree(p);
		n++;
	}
	cfree(calloc(3, 1000));
	cfree(realloc(NULL, 5000));
	EXPECT_GT(n, 550);
}

/* Other blocks untouched, the space usable again. */
TEST_OPT(other_blocks_are_untouched, .ref = "glibc cfree(3)")
{
	static void		*p[300];
	static size_t	n[300];
	int				bad = -1;

	for (int i = 0; i < 300; i++)
	{
		n[i] = sl_sizes(i * 7 % 1112);
		p[i] = malloc(n[i]);
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int i = 0; i < 300; i += 2)
	{
		cfree(p[i]);
		p[i] = malloc(n[i]);
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int i = 0; i < 300 && bad < 0; i++)
		if (!sl_check(p[i], n[i], (unsigned)i))
			bad = i;
	for (int i = 0; i < 300; i++)
		cfree(p[i]);
	EXPECT_EQ(bad, -1);
}
