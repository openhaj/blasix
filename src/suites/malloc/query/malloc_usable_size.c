/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file malloc_usable_size.c
 * @brief Tests for malloc_usable_size() (<malloc.h>, extension).
 * @Created: 2026/11/25 14:27:14 by Moutig
 * @Updated: 2026/11/25 14:27:43 by Moutig
 *
 * malloc_usable_size(ptr) returns the number of usable bytes in the block
 * ptr points to, a block of malloc() or a related function: at least the
 * size requested, maybe more, and the excess can be written (glibc says
 * "without ill effects"); 0 for a null pointer.
 *
 * C23 recommended practice (7.24.3.4p5, 7.24.3.5p5): where such a query
 * exists, free_sized() and free_aligned_sized() should accept the usable
 * size in place of the requested one ("should": a failing case means the
 * recommendation is not followed, not that the libc does not conform).
 *
 * In no standard (EXT=1). Reference: glibc malloc_usable_size(3).
 */
#include "malloc/malloc_cases.h"

TEST_OPT(prototype, .ref = "glibc malloc_usable_size(3)")
{
	EXPECT_TYPE(&malloc_usable_size, size_t (*)(void *));
}

TEST_OPT(null_pointer_gives_zero, .ref = "glibc malloc_usable_size(3)")
{
	EXPECT_UEQ(malloc_usable_size(NULL), 0);
}

/* Every size class: at least the requested size. */
TEST_OPT(at_least_the_requested_size, .ref = "glibc malloc_usable_size(3)")
{
	size_t	bad = 0;

	for (int i = 0; sl_sizes(i) && !bad; i++)
	{
		void	*p = malloc(sl_sizes(i));

		ASSERT_NOTNULL(p);
		if (malloc_usable_size(p) < sl_sizes(i))
			bad = sl_sizes(i);
		free(p);
	}
	EXPECT_UEQ(bad, 0);
}

/* Blocks of calloc(), realloc() and aligned_alloc() too. */
TEST_OPT(blocks_of_the_other_allocation_functions, .ref = "glibc malloc_usable_size(3)")
{
	void	*c = calloc(10, 33);
	void	*r = realloc(NULL, 700);
	void	*a = aligned_alloc(256, 512);

	ASSERT_NOTNULL(c);
	ASSERT_NOTNULL(r);
	ASSERT_NOTNULL(a);
	EXPECT_UGT(malloc_usable_size(c), 329);
	EXPECT_UGT(malloc_usable_size(a), 511);
	r = realloc(r, 9000);
	ASSERT_NOTNULL(r);
	EXPECT_UGT(malloc_usable_size(r), 8999);
	free(c);
	free(r);
	free(a);
}

/*
 * The whole usable size can be written: live neighbours keep their
 * contents, and the block is freed normally.
 */
TEST_OPT(whole_usable_size_is_writable, .ref = "glibc malloc_usable_size(3)")
{
	static unsigned char	*p[300];
	static size_t			n[300];
	int						bad = -1;

	for (int i = 0; i < 300; i++)
	{
		p[i] = malloc(sl_sizes(i * 7 % 1112));
		ASSERT_NOTNULL(p[i]);
		n[i] = malloc_usable_size(p[i]);
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int i = 0; i < 300 && bad < 0; i++)
		if (!sl_check(p[i], n[i], (unsigned)i))
			bad = i;
	for (int i = 0; i < 300; i++)
		free(p[i]);
	EXPECT_EQ(bad, -1);
}

/* A block grown in place keeps a usable size of at least its new size. */
TEST_OPT(follows_realloc, .ref = "glibc malloc_usable_size(3)")
{
	unsigned char	*p = NULL;
	size_t			bad = 0;

	for (size_t n = 1; n <= 70000 && !bad; n = n * 3 + 1)
	{
		unsigned char	*q = realloc(p, n);

		ASSERT_NOTNULL(q);
		p = q;
		if (malloc_usable_size(p) < n)
			bad = n;
	}
	free(p);
	EXPECT_UEQ(bad, 0);
}

/* C23 recommended practice: free_sized() accepts the usable size. */
TEST_OPT(free_sized_should_accept_the_usable_size, .since = "C23", .ref = "C23 7.24.3.4p5")
{
	int	n = 0;

	for (int i = 0; sl_sizes(i) && sl_sizes(i) <= ((size_t)1 << 20) + 1; i += 3)
	{
		void	*p = malloc(sl_sizes(i));

		ASSERT_NOTNULL(p);
		free_sized(p, malloc_usable_size(p));
		n++;
	}
	for (int i = 0; i < 1000; i++)
		free(malloc((size_t)i + 1));
	EXPECT_GT(n, 300);
}

/* C23 recommended practice: free_aligned_sized() accepts the usable size. */
TEST_OPT(free_aligned_sized_should_accept_the_usable_size, .since = "C23", .ref = "C23 7.24.3.5p5")
{
	int	n = 0;

	for (size_t a = 1; a <= 65536; a *= 2)
		for (size_t k = 1; k <= 5; k++)
		{
			void	*p = aligned_alloc(a, a * k);

			ASSERT_NOTNULL(p);
			free_aligned_sized(p, a, malloc_usable_size(p));
			n++;
		}
	for (int i = 0; i < 1000; i++)
		free(malloc((size_t)i + 1));
	EXPECT_EQ(n, 85);
}
