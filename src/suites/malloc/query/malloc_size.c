/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file malloc_size.c
 * @brief Tests for malloc_size() (<malloc.h>, extension).
 * @Created: 2026/11/25 14:26:38 by Moutig
 * @Updated: 2026/11/25 14:27:16 by Moutig
 *
 * malloc_size(ptr) returns the size of the memory block that backs the
 * allocation ptr points to: always at least as large as the allocation,
 * maybe larger.
 *
 * In no standard (EXT=1). Reference: Apple malloc_size(3) (declared in
 * <malloc/malloc.h> on macOS, in <malloc.h> by the libcs that copy it).
 */
#include "malloc/malloc_cases.h"

TEST_OPT(prototype, .ref = "Apple malloc_size(3)")
{
	EXPECT_TYPE(&malloc_size, size_t (*)(const void *));
}

/* Every size class: at least the size of the allocation. */
TEST_OPT(at_least_the_allocation_size, .ref = "Apple malloc_size(3)")
{
	size_t	bad = 0;

	for (int i = 0; sl_sizes(i) && !bad; i++)
	{
		void	*p = malloc(sl_sizes(i));

		ASSERT_NOTNULL(p);
		if (malloc_size(p) < sl_sizes(i))
			bad = sl_sizes(i);
		free(p);
	}
	EXPECT_UEQ(bad, 0);
}

/* The block that backs it can be written whole, neighbours intact. */
TEST_OPT(whole_block_is_writable, .ref = "Apple malloc_size(3)")
{
	static unsigned char	*p[200];
	static size_t			n[200];
	int						bad = -1;

	for (int i = 0; i < 200; i++)
	{
		p[i] = malloc(sl_sizes(i * 11 % 1110));
		ASSERT_NOTNULL(p[i]);
		n[i] = malloc_size(p[i]);
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int i = 0; i < 200 && bad < 0; i++)
		if (!sl_check(p[i], n[i], (unsigned)i))
			bad = i;
	for (int i = 0; i < 200; i++)
		free(p[i]);
	EXPECT_EQ(bad, -1);
}

/* Blocks of calloc() and realloc(). */
TEST_OPT(blocks_of_calloc_and_realloc, .ref = "Apple malloc_size(3)")
{
	void	*c = calloc(7, 100);
	void	*r = realloc(NULL, 50);

	ASSERT_NOTNULL(c);
	ASSERT_NOTNULL(r);
	EXPECT_UGT(malloc_size(c), 699);
	r = realloc(r, 123456);
	ASSERT_NOTNULL(r);
	EXPECT_UGT(malloc_size(r), 123455);
	free(c);
	free(r);
}
