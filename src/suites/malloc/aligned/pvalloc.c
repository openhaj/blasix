/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file pvalloc.c
 * @brief Tests for pvalloc() (<malloc.h>, extension).
 * @Created: 2026/11/25 14:22:40 by Moutig
 * @Updated: 2026/11/25 14:22:58 by Moutig
 *
 * pvalloc(size) is valloc() with the size rounded up to the next
 * multiple of the page size: the pointer is a multiple of the page size,
 * and every byte up to the rounded size can be used; free() releases it.
 * On error: a null pointer, errno set (ENOMEM). Obsolete.
 *
 * In no standard (EXT=1). Reference: glibc posix_memalign(3).
 */
#include "malloc/malloc_cases.h"

TEST_OPT(prototype, .ref = "glibc posix_memalign(3)")
{
	EXPECT_TYPE(&pvalloc, void *(*)(size_t));
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
		void	*p = pvalloc(n);

		if (p == NULL || (uintptr_t)p % page != 0)
			bad = n;
		else
		{
			sl_fill(p, (n + page - 1) / page * page, (unsigned)k);
			if (!sl_check(p, (n + page - 1) / page * page, (unsigned)k))
				bad = n;
		}
		free(p);
	}
	EXPECT_UEQ(bad, 0);
}

/* The size is rounded up to the page: every byte of the last page is usable. */
TEST_OPT(size_rounded_up_to_the_page, .ref = "glibc posix_memalign(3)")
{
	ML_PAGE(page);
	unsigned char	*p = pvalloc(1);
	unsigned char	*q;

	ASSERT_NOTNULL(p);
	q = pvalloc(page + 1);
	ASSERT_NOTNULL(q);
	lc_fill(p, 0x5a, page);
	lc_fill(q, 0xa5, 2 * page);
	EXPECT_UEQ(p[page - 1], 0x5a);
	EXPECT_UEQ(q[2 * page - 1], 0xa5);
	free(p);
	free(q);
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
		p[i] = pvalloc(n[i]);
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
	EXPECT_NULL(pvalloc(SIZE_MAX - 65535));
	EXPECT_EQ(errno, ENOMEM);
	errno = 0;
	EXPECT_NULL(pvalloc(SIZE_MAX / 2 + 1));
	EXPECT_EQ(errno, ENOMEM);
}
