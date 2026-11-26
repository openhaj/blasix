/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file valloc.c
 * @brief Tests for valloc() (<malloc.h>, extension).
 * @Created: 2026/11/25 14:23:06 by Moutig
 * @Updated: 2026/11/25 14:23:27 by Moutig
 *
 * valloc(size) allocates size bytes and returns a pointer to them, a
 * multiple of the page size; free() releases it. On error: a null
 * pointer, errno set (ENOMEM). Obsolete.
 *
 * In no standard (EXT=1). Reference: glibc posix_memalign(3) (also in the
 * <stdlib.h> of the BSDs).
 */
#include "malloc/malloc_cases.h"

TEST_OPT(prototype, .ref = "glibc posix_memalign(3)")
{
	EXPECT_TYPE(&valloc, void *(*)(size_t));
}

/* Sizes below, at and around the page, up to 3 MiB: page aligned, writable. */
TEST_OPT(page_aligned_and_writable, .ref = "glibc posix_memalign(3)")
{
	ML_PAGE(page);
	size_t	sizes[8];
	size_t	bad = 0;

	sizes[0] = 1;
	sizes[1] = 100;
	sizes[2] = page - 1;
	sizes[3] = page;
	sizes[4] = page + 1;
	sizes[5] = 3 * page + 5;
	sizes[6] = (size_t)1 << 20;
	sizes[7] = ((size_t)3 << 20) + 7;
	for (int k = 0; k < 8 && !bad; k++)
	{
		size_t	n = sizes[k];
		void	*p = valloc(n);

		if (p == NULL || (uintptr_t)p % page != 0)
			bad = n;
		else
		{
			sl_fill(p, n, (unsigned)k);
			if (!sl_check(p, n, (unsigned)k))
				bad = n;
		}
		free(p);
	}
	EXPECT_UEQ(bad, 0);
}

/* Live blocks are distinct pages and keep their contents. */
TEST_OPT(allocations_are_disjoint, .ref = "glibc posix_memalign(3)")
{
	static void		*p[64];
	static size_t	n[64];
	int				bad = -1;

	for (int i = 0; i < 64; i++)
	{
		n[i] = (size_t)(i * 997 % 20000) + 1;
		p[i] = valloc(n[i]);
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int i = 0; i < 64 && bad < 0; i++)
	{
		if (!sl_check(p[i], n[i], (unsigned)i))
			bad = i;
		for (int j = 0; j < i && bad < 0; j++)
			if (!sl_disjoint(p[i], n[i], p[j], n[j]))
				bad = i;
	}
	for (int i = 0; i < 64; i++)
		free(p[i]);
	EXPECT_EQ(bad, -1);
}

TEST_OPT(impossible_size_sets_enomem, .ref = "glibc posix_memalign(3)")
{
	errno = 0;
	EXPECT_NULL(valloc(SIZE_MAX - 65535));
	EXPECT_EQ(errno, ENOMEM);
	errno = 0;
	EXPECT_NULL(valloc(SIZE_MAX / 2 + 1));
	EXPECT_EQ(errno, ENOMEM);
}
