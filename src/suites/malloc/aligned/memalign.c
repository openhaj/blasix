/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memalign.c
 * @brief Tests for memalign() (<malloc.h>, extension).
 * @Created: 2026/11/25 14:21:55 by Moutig
 * @Updated: 2026/11/25 14:22:34 by Moutig
 *
 * memalign(alignment, size) allocates size bytes and returns a pointer to
 * them, a multiple of alignment, which must be a power of two; free()
 * releases it. On error: a null pointer, errno set (ENOMEM). Obsolete in
 * favour of posix_memalign() and aligned_alloc().
 *
 * In no standard (EXT=1). Reference: glibc posix_memalign(3).
 */
#include "malloc/malloc_cases.h"

TEST_OPT(prototype, .ref = "glibc posix_memalign(3)")
{
	EXPECT_TYPE(&memalign, void *(*)(size_t, size_t));
}

/* Every power of two up to 1 MiB, sizes below, at and above it. */
TEST_OPT(aligned_and_writable, .ref = "glibc posix_memalign(3)")
{
	static const size_t	sizes[] = {1, 7, 64, 1000, 5000, 70000, (size_t)3 << 20};
	size_t				bad = 0;

	for (size_t a = 1; a <= ((size_t)1 << 20) && !bad; a *= 2)
		for (int k = 0; k < (int)(sizeof(sizes) / sizeof(sizes[0])) && !bad; k++)
		{
			void	*p = memalign(a, sizes[k]);

			if (p == NULL || (uintptr_t)p % a != 0)
				bad = a;
			else
			{
				sl_fill(p, sizes[k], (unsigned)k);
				if (!sl_check(p, sizes[k], (unsigned)k))
					bad = a;
			}
			free(p);
		}
	EXPECT_UEQ(bad, 0);
}

/* Live blocks are disjoint and keep their contents. */
TEST_OPT(allocations_are_disjoint, .ref = "glibc posix_memalign(3)")
{
	static void		*p[100];
	static size_t	n[100];
	int				bad = -1;

	for (int i = 0; i < 100; i++)
	{
		n[i] = (size_t)(i * 37 % 2000) + 1;
		p[i] = memalign((size_t)1 << (i % 12), n[i]);
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int i = 0; i < 100 && bad < 0; i++)
	{
		if (!sl_check(p[i], n[i], (unsigned)i))
			bad = i;
		for (int j = 0; j < i && bad < 0; j++)
			if (!sl_disjoint(p[i], n[i], p[j], n[j]))
				bad = i;
	}
	for (int i = 0; i < 100; i++)
		free(p[i]);
	EXPECT_EQ(bad, -1);
}

TEST_OPT(impossible_size_sets_enomem, .ref = "glibc posix_memalign(3)")
{
	errno = 0;
	EXPECT_NULL(memalign(64, SIZE_MAX - 63));
	EXPECT_EQ(errno, ENOMEM);
	errno = 0;
	EXPECT_NULL(memalign(4096, SIZE_MAX / 2 + 1));
	EXPECT_EQ(errno, ENOMEM);
}
