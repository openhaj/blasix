/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file malloc_good_size.c
 * @brief Tests for malloc_good_size()
 * @Created: 2026/11/25 14:26:05 by Moutig
 * @Updated: 2026/11/25 14:26:30 by Moutig
 *
 * malloc_good_size(size) rounds size up to a value the allocator can give
 * without adding padding, and returns it: at least size.
 *
 * In no standard (EXT=1). Reference: Apple malloc_size(3) (declared in
 * <malloc/malloc.h> on macOS, in <malloc.h> by the libcs that copy it).
 */
#include "malloc/malloc_cases.h"

TEST_OPT(prototype, .ref = "Apple malloc_size(3)")
{
	EXPECT_TYPE(&malloc_good_size, size_t (*)(size_t));
}

/* Every size class: rounded up, never down. */
TEST_OPT(at_least_the_size, .ref = "Apple malloc_size(3)")
{
	size_t	bad = 0;

	for (int i = 0; sl_sizes(i) && !bad; i++)
		if (malloc_good_size(sl_sizes(i)) < sl_sizes(i))
			bad = sl_sizes(i);
	EXPECT_UEQ(bad, 0);
}

/* A block of the good size can be allocated and written whole. */
TEST_OPT(good_size_can_be_allocated, .ref = "Apple malloc_size(3)")
{
	size_t	bad = 0;

	for (int i = 0; sl_sizes(i) && sl_sizes(i) <= ((size_t)4 << 20) && !bad; i += 7)
	{
		size_t	n = malloc_good_size(sl_sizes(i));
		void	*p = malloc(n);

		if (p == NULL)
			bad = n;
		else
		{
			sl_fill(p, n, (unsigned)i);
			if (!sl_check(p, n, (unsigned)i))
				bad = n;
			free(p);
		}
	}
	EXPECT_UEQ(bad, 0);
}
