/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file reallocarray.c
 * @brief Tests for reallocarray() (<stdlib.h>, POSIX.1-2024).
 * @Created: 2026/11/25 13:52:55 by Moutig
 * @Updated: 2026/11/25 13:53:14 by Moutig
 *
 * reallocarray(ptr, nelem, elsize) is equivalent to realloc(ptr, nelem *
 * elsize), except that an overflow of the multiplication is an error: a
 * null pointer and ENOMEM, the old object not deallocated. A null ptr: as
 * malloc() of the product. Not enough memory: a null pointer, ENOMEM.
 * nelem or elsize 0 ([OB]): either a null pointer with errno EINVAL (ptr
 * not null), or a pointer to allocated space with the old object freed.
 * The new object has the guarantees of every allocation (aligned for any
 * type with a fundamental alignment requirement, disjoint).
 *
 * Clauses: POSIX: XSH realloc() (POSIX.1-2024, CX, Austin Group Defect
 * 1218).
 */
#include "stdlib/stdlib_cases.h"

TEST_OPT(prototype, .ref = "XSH realloc() (Issue 8)")
{
	EXPECT_TYPE(&reallocarray, void *(*)(void *, size_t, size_t));
}

/* Shapes nelem x elsize, from one byte to 3 MiB. */
static const size_t	g_shapes[][2] = {{1, 1}, {3, 7}, {7, 3}, {100, 1}, {1, 100},
	{10, 1000}, {1000, 10}, {4096, 16}, {16, 4096}, {3, 1 << 20}, {1 << 20, 3}};

# define N_SHAPES	((int)(sizeof(g_shapes) / sizeof(g_shapes[0])))

/* A null pointer: like malloc(nelem * elsize), aligned and writable. */
TEST_OPT(null_pointer_behaves_like_malloc, .ref = "XSH realloc() (Issue 8)")
{
	int	bad = -1;

	for (int k = 0; k < N_SHAPES && bad < 0; k++)
	{
		size_t	n = g_shapes[k][0] * g_shapes[k][1];
		void	*p = reallocarray(NULL, g_shapes[k][0], g_shapes[k][1]);

		if (p == NULL || sl_misalignedType(p, n, 0) != NULL)
			bad = k;
		else
		{
			sl_fill(p, n, (unsigned)k);
			if (!sl_check(p, n, (unsigned)k))
				bad = k;
		}
		free(p);
	}
	EXPECT_EQ(bad, -1);
}

/*
 * Every shape resized to every other: the contents up to the lesser size
 * are kept, the whole new array is writable and aligned.
 */
TEST_OPT(contents_kept_up_to_the_lesser_size, .ref = "XSH realloc() (Issue 8)")
{
	int	bad = -1;

	for (int a = 0; a < N_SHAPES && bad < 0; a++)
		for (int b = 0; b < N_SHAPES && bad < 0; b++)
		{
			size_t	from = g_shapes[a][0] * g_shapes[a][1];
			size_t	to = g_shapes[b][0] * g_shapes[b][1];
			void	*p = reallocarray(NULL, g_shapes[a][0], g_shapes[a][1]);
			void	*q;

			ASSERT_NOTNULL(p);
			sl_fill(p, from, (unsigned)a);
			q = reallocarray(p, g_shapes[b][0], g_shapes[b][1]);
			ASSERT_NOTNULL(q);
			if (!sl_checkPrefix(q, from < to ? from : to, from, (unsigned)a)
				|| sl_misalignedType(q, to, 0) != NULL)
				bad = a * N_SHAPES + b;
			sl_fill(q, to, (unsigned)b);
			if (!sl_check(q, to, (unsigned)b))
				bad = a * N_SHAPES + b;
			free(q);
		}
	EXPECT_EQ(bad, -1);
}

/* One array grown an element at a time (24-byte elements), then shrunk. */
TEST_OPT(grown_and_shrunk_element_by_element, .ref = "XSH realloc() (Issue 8)")
{
	unsigned char	*p = NULL;
	size_t			bad = 0;

	for (size_t n = 1; n <= 600 && !bad; n++)
	{
		unsigned char	*q = reallocarray(p, n, 24);

		ASSERT_NOTNULL(q);
		p = q;
		if (!sl_check(p, (n - 1) * 24, 5))
			bad = n;
		for (size_t i = (n - 1) * 24; i < n * 24; i++)
			p[i] = sl_byte(5, i);
	}
	for (size_t n = 599; n >= 1 && !bad; n--)
	{
		unsigned char	*q = reallocarray(p, n, 24);

		ASSERT_NOTNULL(q);
		p = q;
		if (!sl_check(p, n * 24, 5))
			bad = n;
	}
	free(p);
	EXPECT_UEQ(bad, 0);
}

/* Resized arrays among live neighbours stay disjoint, the neighbours intact. */
TEST_OPT(resized_arrays_stay_disjoint, .ref = "XSH realloc() (Issue 8)")
{
	static unsigned char	*p[150];
	static size_t			n[150];
	int						bad = -1;

	for (int i = 0; i < 150; i++)
	{
		n[i] = (size_t)(i % 40 + 1) * 8;
		p[i] = reallocarray(NULL, (size_t)(i % 40 + 1), 8);
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int i = 0; i < 150; i += 2)
	{
		unsigned char	*q = reallocarray(p[i], n[i] / 8 * 5, 16);

		ASSERT_NOTNULL(q);
		if (bad < 0 && !sl_check(q, n[i], (unsigned)i))
			bad = i;
		p[i] = q;
		n[i] = n[i] / 8 * 80;
		sl_fill(p[i], n[i], (unsigned)i);
	}
	for (int i = 0; i < 150 && bad < 0; i++)
	{
		if (!sl_check(p[i], n[i], (unsigned)i))
			bad = i;
		for (int j = 0; j < i && bad < 0; j++)
			if (!sl_disjoint(p[i], n[i], p[j], n[j]))
				bad = 1000 + i;
	}
	for (int i = 0; i < 150; i++)
		free(p[i]);
	EXPECT_EQ(bad, -1);
}

/* Products that wrap around size_t ((SIZE_MAX / 3 + 1) * 3 wraps to 2). */
static const size_t	g_overflow[][2] = {{SIZE_MAX, 2}, {2, SIZE_MAX}, {SIZE_MAX / 2 + 1, 2},
	{SIZE_MAX / 3 + 1, 3}, {(size_t)1 << (sizeof(size_t) * 4), (size_t)1 << (sizeof(size_t) * 4)},
	{SIZE_MAX, SIZE_MAX}};

# define N_OVERFLOW	((int)(sizeof(g_overflow) / sizeof(g_overflow[0])))

/*
 * nelem * elsize overflows: a null pointer and ENOMEM, never the small
 * block the wrapped product asks for; the old object is not deallocated
 * and keeps its contents.
 */
TEST_OPT(overflow_is_an_error, .ref = "XSH realloc() ERRORS (Issue 8)")
{
	int	bad = -1;

	for (int k = 0; k < N_OVERFLOW && bad < 0; k++)
	{
		unsigned char	*p = malloc(100);

		ASSERT_NOTNULL(p);
		sl_fill(p, 100, (unsigned)k);
		errno = 0;
		if (reallocarray(p, g_overflow[k][0], g_overflow[k][1]) != NULL || errno != ENOMEM
			|| !sl_check(p, 100, (unsigned)k))
			bad = k;
		free(p);
	}
	EXPECT_EQ(bad, -1);
}

/* With a null pointer too: the overflow is an error, not a small malloc(). */
TEST_OPT(overflow_with_a_null_pointer, .ref = "XSH realloc() ERRORS (Issue 8)")
{
	int	bad = -1;

	for (int k = 0; k < N_OVERFLOW && bad < 0; k++)
	{
		errno = 0;
		if (reallocarray(NULL, g_overflow[k][0], g_overflow[k][1]) != NULL || errno != ENOMEM)
			bad = k;
	}
	EXPECT_EQ(bad, -1);
}

/* No overflow but no system can give it: a null pointer, ENOMEM, the old object kept. */
TEST_OPT(impossible_size_keeps_the_old_object, .ref = "XSH realloc() ERRORS (Issue 8)")
{
	unsigned char	*p = malloc(64);
	unsigned char	*q;

	ASSERT_NOTNULL(p);
	sl_fill(p, 64, 8);
	errno = 0;
	EXPECT_NULL(reallocarray(p, 1, SIZE_MAX));
	EXPECT_EQ(errno, ENOMEM);
	errno = 0;
	EXPECT_NULL(reallocarray(p, SIZE_MAX / 16, 8));
	EXPECT_EQ(errno, ENOMEM);
	EXPECT_TRUE(sl_check(p, 64, 8));
	q = reallocarray(p, 2, 64);
	ASSERT_NOTNULL(q);
	EXPECT_TRUE(sl_check(q, 64, 8));
	free(q);
}

/*
 * nelem or elsize 0 ([OB]): a null pointer with errno EINVAL, or a
 * pointer to allocated space (the old object freed).
 */
TEST_OPT(zero_count_is_null_with_einval_or_a_new_object, .ref = "XSH realloc() RETURN VALUE (Issue 8)")
{
	static const size_t	zero[][2] = {{0, 8}, {8, 0}, {0, 0}};
	int					bad = -1;

	for (int k = 0; k < 3 && bad < 0; k++)
	{
		void	*p = malloc(100);
		void	*q;

		ASSERT_NOTNULL(p);
		errno = 0;
		q = reallocarray(p, zero[k][0], zero[k][1]);
		if (q == NULL && errno != EINVAL)
			bad = k;
		free(q);
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	unsigned char	*a = reallocarray(NULL, 4, 4);
	unsigned char	*b = (reallocarray)(NULL, 4, 4);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	sl_fill(a, 16, 1);
	sl_fill(b, 16, 1);
	a = reallocarray(a, 100, 40);
	b = (reallocarray)(b, 100, 40);
	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	EXPECT_TRUE(sl_check(a, 16, 1) && sl_check(b, 16, 1));
	free(a);
	free(b);
}
