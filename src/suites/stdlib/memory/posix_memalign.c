/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file posix_memalign.c
 * @brief Tests for posix_memalign() (<stdlib.h>, POSIX ADV).
 * @Created: 2026/11/25 13:40:45 by Moutig
 * @Updated: 2026/11/25 13:41:12 by Moutig
 *
 * posix_memalign() allocates size bytes aligned on alignment, which is a
 * power of two multiple of sizeof(void *), and stores the pointer in
 * *memptr, a multiple of alignment; free() deallocates it (CX). It
 * returns 0, or an error number: EINVAL for an alignment that is not a
 * power of two multiple of sizeof(void *), ENOMEM when there is not
 * enough memory with that alignment. POSIX.1-2017 on: on error, *memptr
 * is left unmodified or set to a null pointer; size 0 gives an
 * implementation-defined error number, or 0 with a null pointer, or 0
 * with a pointer that must not be used to access an object.
 *
 * Advisory Information in every edition: the unit runs with OPTS=...,ADV.
 *
 * Clauses: POSIX: XSH posix_memalign() (POSIX.1-2001 on, ADV).
 */
#include "stdlib/stdlib_cases.h"

TEST_OPT(prototype, .ref = "XSH posix_memalign()")
{
	EXPECT_TYPE(&posix_memalign, int (*)(void **, size_t, size_t));
}

/*
 * Every valid alignment from sizeof(void *) to 1 MiB, with sizes below,
 * at and above it: 0, a multiple of the alignment, the space writable.
 */
TEST_OPT(valid_alignments, .ref = "XSH posix_memalign()")
{
	static const size_t	sizes[] = {1, 7, 64, 1000, 5000, 70000, (size_t)3 << 20};
	size_t				bad = 0;

	for (size_t a = sizeof(void *); a <= ((size_t)1 << 20) && !bad; a *= 2)
		for (int k = 0; k < (int)(sizeof(sizes) / sizeof(sizes[0])) && !bad; k++)
		{
			void	*p = NULL;

			if (posix_memalign(&p, a, sizes[k]) != 0 || p == NULL
				|| (uintptr_t)p % a != 0)
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

/* Live aligned blocks are disjoint, and each keeps its contents. */
TEST_OPT(allocations_are_disjoint, .ref = "XSH posix_memalign()")
{
	static void		*p[100];
	static size_t	n[100];
	int				bad = -1;

	for (int i = 0; i < 100; i++)
	{
		size_t	a = sizeof(void *) << (i % 10);

		n[i] = (size_t)(i * 53 % 3000) + 1;
		p[i] = NULL;
		ASSERT_EQ(posix_memalign(&p[i], a, n[i]), 0);
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

/* The invalid alignments of the case below: not a power of two multiple of sizeof(void *). */
static const size_t	g_badAlign[] = {0, 1, sizeof(void *) / 2, sizeof(void *) - 1,
	sizeof(void *) + 1, sizeof(void *) * 3, sizeof(void *) * 6, 100, 4095, SIZE_MAX};

TEST_OPT(invalid_alignment_returns_einval, .ref = "XSH posix_memalign() ERRORS")
{
	int	bad = -1;

	for (int k = 0; k < (int)(sizeof(g_badAlign) / sizeof(g_badAlign[0])) && bad < 0; k++)
	{
		void	*p = NULL;

		if (posix_memalign(&p, g_badAlign[k], 64) != EINVAL)
			bad = k;
		if (p != NULL)
			free(p);
	}
	EXPECT_EQ(bad, -1);
}

/* POSIX.1-2017 on: on error, *memptr is left unmodified or set to a null pointer. */
TEST_OPT(error_leaves_memptr_or_nulls_it, .since = "POSIX.1-2017",
	.ref = "XSH posix_memalign() RETURN VALUE")
{
	static char	mark;
	void		*p = &mark;

	EXPECT_EQ(posix_memalign(&p, sizeof(void *) * 3, 64), EINVAL);
	EXPECT_TRUE(p == &mark || p == NULL);
	p = &mark;
	EXPECT_EQ(posix_memalign(&p, 64, SIZE_MAX - 63), ENOMEM);
	EXPECT_TRUE(p == &mark || p == NULL);
}

/* Not enough memory with that alignment: ENOMEM; the next calls still work. */
TEST_OPT(impossible_size_returns_enomem, .ref = "XSH posix_memalign() ERRORS")
{
	void	*p = NULL;

	EXPECT_EQ(posix_memalign(&p, sizeof(void *), SIZE_MAX), ENOMEM);
	EXPECT_EQ(posix_memalign(&p, 4096, SIZE_MAX - 4095), ENOMEM);
	EXPECT_EQ(posix_memalign(&p, 64, SIZE_MAX / 2 + 1), ENOMEM);
	p = NULL;
	EXPECT_EQ(posix_memalign(&p, 64, 64), 0);
	EXPECT_NOTNULL(p);
	free(p);
}

/*
 * POSIX.1-2017 on, size 0: an error number, or 0 with a null pointer, or
 * 0 with a pointer that free() accepts.
 */
TEST_OPT(zero_size, .since = "POSIX.1-2017", .ref = "XSH posix_memalign() RETURN VALUE")
{
	void	*p = NULL;
	int		r = posix_memalign(&p, 64, 0);

	if (r != 0)
		LOG("posix_memalign(&p, 64, 0) returns an error number");
	else
		free(p);
	EXPECT_TRUE(r != 0 || p == NULL || (uintptr_t)p % 64 == 0);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	void	*a = NULL;
	void	*b = NULL;

	EXPECT_EQ(posix_memalign(&a, 128, 10), 0);
	EXPECT_EQ((posix_memalign)(&b, 128, 10), 0);
	EXPECT_UEQ((uintptr_t)a % 128, 0);
	EXPECT_UEQ((uintptr_t)b % 128, 0);
	free(a);
	free(b);
}
