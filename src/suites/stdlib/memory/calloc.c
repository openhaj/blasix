/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file calloc.c
 * @brief Tests for calloc() (<stdlib.h>).
 * @Created: 2026/11/25 13:20:46 by Moutig
 * @Updated: 2026/11/25 13:20:53 by Moutig
 *
 * calloc() allocates space for an array of nmemb objects of size bytes,
 * initialized to all bits zero (7.24.3.2p2), with the guarantees of every
 * allocation (7.24.3p1: alignment, disjoint, start of the space, null
 * pointer when the space cannot be allocated, size zero
 * implementation-defined). C23 says it: a null pointer when nmemb * size
 * would wrap around size_t (7.24.3.2p3); before, such an array cannot be
 * allocated either. POSIX (CX): a failure sets errno, ENOMEM; POSIX.1-2024
 * names the overflow of nelem * elsize.
 *
 * Clauses: C99 7.20.3.1; C11, C17 7.22.3.2; C23 7.24.3.2. POSIX: XSH
 * calloc().
 */
#include "stdlib/stdlib_cases.h"

TEST_OPT(prototype, .ref = "C11 7.22.3.2p1")
{
	EXPECT_TYPE(&calloc, void *(*)(size_t, size_t));
}

/* The first byte that is not zero, or n. */
static size_t	firstNonZero(const unsigned char *p, size_t n)
{
	size_t	i = 0;

	while (i < n && p[i] == 0)
		i++;
	return (i);
}

/* Every size class, one element: every byte is zero. */
TEST_OPT(space_is_all_bits_zero, .ref = "C11 7.22.3.2p2")
{
	size_t	bad = 0;

	for (int i = 0; sl_sizes(i) && !bad; i++)
	{
		size_t			n = sl_sizes(i);
		unsigned char	*p = calloc(1, n);

		if (p == NULL || firstNonZero(p, n) != n)
			bad = n;
		free(p);
	}
	EXPECT_UEQ(bad, 0);
}

/*
 * Space that malloc() gave (up to 16 MiB), dirtied and freed, comes back
 * zeroed through calloc(): the allocator may not trust it to be still zero.
 */
TEST_OPT(reused_space_is_zeroed, .ref = "C11 7.22.3.2p2")
{
	size_t	bad = 0;

	for (int round = 0; round < 2 && !bad; round++)
		for (int i = 0; sl_sizes(i) && sl_sizes(i) <= ((size_t)16 << 20) + 1 && !bad;
			i += (i < 1100 ? 7 : 1))
		{
			size_t			n = sl_sizes(i);
			unsigned char	*m = malloc(n);
			unsigned char	*p;

			ASSERT_NOTNULL(m);
			lc_fill(m, 0xa5, n);
			free(m);
			p = calloc(1, n);
			if (p == NULL || firstNonZero(p, n) != n)
				bad = n;
			free(p);
		}
	EXPECT_UEQ(bad, 0);
}

/* Many dirty blocks of one size freed, then as many from calloc(). */
TEST_OPT(many_reused_blocks_are_zeroed, .ref = "C11 7.22.3.2p2")
{
	static unsigned char	*p[4000];
	int						bad = -1;

	for (int i = 0; i < 4000; i++)
	{
		p[i] = malloc(48);
		ASSERT_NOTNULL(p[i]);
		lc_fill(p[i], 0xff, 48);
	}
	for (int i = 0; i < 4000; i++)
		free(p[i]);
	for (int i = 0; i < 4000; i++)
	{
		p[i] = calloc(6, 8);
		ASSERT_NOTNULL(p[i]);
		if (bad < 0 && firstNonZero(p[i], 48) != 48)
			bad = i;
	}
	for (int i = 0; i < 4000; i++)
		free(p[i]);
	EXPECT_EQ(bad, -1);
}

/* nmemb elements of size bytes: the whole array is zero, then writable. */
TEST_OPT(array_of_nmemb_elements, .ref = "C11 7.22.3.2p2")
{
	static const size_t	shapes[][2] = {{3, 7}, {7, 3}, {100, 1}, {1, 100}, {10, 1000},
		{1000, 10}, {4096, 16}, {16, 4096}, {3, 1 << 20}, {1 << 20, 3}};
	size_t				bad = 0;

	for (int k = 0; k < (int)(sizeof(shapes) / sizeof(shapes[0])) && !bad; k++)
	{
		size_t			n = shapes[k][0] * shapes[k][1];
		unsigned char	*p = calloc(shapes[k][0], shapes[k][1]);

		if (p == NULL || firstNonZero(p, n) != n)
			bad = (size_t)k + 1;
		else
		{
			sl_fill(p, n, (unsigned)k);
			if (!sl_check(p, n, (unsigned)k))
				bad = (size_t)k + 1;
		}
		free(p);
	}
	EXPECT_UEQ(bad, 0);
}

/* An array of long double and of pointers: zero values, usable at once. */
TEST_OPT(holds_an_array_of_objects, .ref = "C11 7.22.3p1")
{
	long double	*d = calloc(5, sizeof(long double));
	int			*v = calloc(9, sizeof(int));

	ASSERT_NOTNULL(d);
	ASSERT_NOTNULL(v);
	d[4] = 2.5L;
	v[8] = -3;
	EXPECT_TRUE(d[4] == 2.5L && v[8] == -3);
	EXPECT_EQ(v[0] + v[7], 0);
	free(d);
	free(v);
}

/* C99 to C17 (and POSIX): aligned for every fundamental type. */
TEST_OPT(aligned_for_every_fundamental_type, .until = "C23", .ref = "C11 7.22.3p1")
{
	const char	*bad = NULL;

	for (size_t n = 1; n <= 64 && !bad; n++)
		for (size_t s = 1; s <= 24 && !bad; s += 7)
		{
			void	*p = calloc(n, s);

			ASSERT_NOTNULL(p);
			bad = sl_misalignedType(p, n * s, 0);
			free(p);
		}
	if (bad)
		LOG(bad);
	EXPECT_NULL(bad);
}

/* C23: aligned for every fundamental type that fits in nmemb * size. */
TEST_OPT(aligned_for_every_type_that_fits, .since = "C23", .ref = "C23 7.24.3p1")
{
	const char	*bad = NULL;

	for (size_t n = 1; n <= 64 && !bad; n++)
		for (size_t s = 1; s <= 24 && !bad; s += 7)
		{
			void	*p = calloc(n, s);

			ASSERT_NOTNULL(p);
			bad = sl_misalignedType(p, n * s, 1);
			free(p);
		}
	if (bad)
		LOG(bad);
	EXPECT_NULL(bad);
}

/* Live arrays of mixed shapes do not overlap. */
TEST_OPT(allocations_are_disjoint, .ref = "C11 7.22.3p1")
{
	static void		*p[300];
	static size_t	n[300];
	int				bad = -1;

	for (int i = 0; i < 300; i++)
	{
		size_t	e = (size_t)(i % 13) + 1;

		n[i] = e * ((size_t)i * 17 % 400 + 1);
		p[i] = calloc(e, n[i] / e);
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int i = 0; i < 300 && bad < 0; i++)
	{
		if (!sl_check(p[i], n[i], (unsigned)i))
			bad = i;
		for (int j = 0; j < i && bad < 0; j++)
			if (!sl_disjoint(p[i], n[i], p[j], n[j]))
				bad = i;
	}
	for (int i = 0; i < 300; i++)
		free(p[i]);
	EXPECT_EQ(bad, -1);
}

/* nmemb or size zero: a null pointer, or a unique pointer free() accepts. */
TEST_OPT(zero_size_is_null_or_unique, .ref = "C11 7.22.3p1")
{
	void	*a = calloc(0, 8);
	void	*b = calloc(8, 0);
	void	*c = calloc(0, 0);
	void	*d = calloc(1, 1);

	ASSERT_NOTNULL(d);
	EXPECT_TRUE(a == NULL || (a != b && a != c && a != d));
	EXPECT_TRUE(b == NULL || (b != c && b != d));
	EXPECT_TRUE(c == NULL || c != d);
	free(a);
	free(b);
	free(c);
	free(d);
}

/*
 * nmemb * size wraps around size_t: a null pointer, never the small
 * block the wrapped product asks for ((SIZE_MAX / 3 + 1) * 3 wraps to 2).
 */
TEST_OPT(overflowing_product_returns_null, .ref = "C23 7.24.3.2p3")
{
	EXPECT_NULL(calloc(SIZE_MAX, 2));
	EXPECT_NULL(calloc(2, SIZE_MAX));
	EXPECT_NULL(calloc(SIZE_MAX / 2 + 1, 2));
	EXPECT_NULL(calloc(SIZE_MAX / 3 + 1, 3));
	EXPECT_NULL(calloc((size_t)1 << (sizeof(size_t) * 4), (size_t)1 << (sizeof(size_t) * 4)));
	EXPECT_NULL(calloc(SIZE_MAX, SIZE_MAX));
}

/* An array too large for any system, without overflow: a null pointer. */
TEST_OPT(impossible_size_returns_null, .ref = "C11 7.22.3p1")
{
	void	*p;

	EXPECT_NULL(calloc(1, SIZE_MAX));
	EXPECT_NULL(calloc(SIZE_MAX, 1));
	EXPECT_NULL(calloc(SIZE_MAX / 16, 8));
	p = calloc(4, 4);
	EXPECT_NOTNULL(p);
	free(p);
}

/* POSIX (CX): insufficient memory sets errno to ENOMEM. */
TEST_OPT(impossible_size_sets_enomem, .since = "POSIX.1-2001", .ref = "XSH calloc() ERRORS")
{
	errno = 0;
	EXPECT_NULL(calloc(1, SIZE_MAX));
	EXPECT_EQ(errno, ENOMEM);
	errno = 0;
	EXPECT_NULL(calloc(SIZE_MAX / 16, 8));
	EXPECT_EQ(errno, ENOMEM);
}

/* POSIX.1-2024: "including the case when nelem * elsize would overflow". */
TEST_OPT(overflow_sets_enomem, .since = "POSIX.1-2024", .ref = "XSH calloc() ERRORS (Issue 8)")
{
	errno = 0;
	EXPECT_NULL(calloc(SIZE_MAX, 2));
	EXPECT_EQ(errno, ENOMEM);
	errno = 0;
	EXPECT_NULL(calloc(SIZE_MAX / 3 + 1, 3));
	EXPECT_EQ(errno, ENOMEM);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	unsigned char	*a = calloc(4, 4);
	unsigned char	*b = (calloc)(4, 4);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	EXPECT_UEQ(firstNonZero(a, 16), 16);
	EXPECT_UEQ(firstNonZero(b, 16), 16);
	free(a);
	free(b);
}
