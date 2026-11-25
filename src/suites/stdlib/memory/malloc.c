/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file malloc.c
 * @brief Tests for malloc() (<stdlib.h>).
 * @Created: 2026/11/25 13:05:17 by Moutig
 * @Updated: 2026/11/25 13:07:57 by Moutig
 *
 * malloc() allocates space for an object of the given size, of
 * unspecified value (7.24.3.6p2). The pointer is suitably aligned for any
 * type of object with a fundamental alignment requirement (C23: of at
 * most the size requested), points to the start of the space, and each
 * allocation is disjoint from any other object; a null pointer means the
 * space cannot be allocated (7.24.3p1). A size of zero is
 * implementation-defined: a null pointer, or as if some nonzero size
 * whose pointer must not be used to access an object (POSIX.1-2001 to
 * 2017: "a unique pointer that can be successfully passed to free()").
 * POSIX: a failure sets errno, ENOMEM when the storage is insufficient
 * (CX).
 *
 * Clauses: C99 7.20.3, 7.20.3.3; C11, C17 7.22.3, 7.22.3.4; C23 7.24.3,
 * 7.24.3.6. POSIX: XSH malloc().
 */
#include "stdlib/stdlib_cases.h"

TEST_OPT(prototype, .ref = "C11 7.22.3.4p1")
{
	EXPECT_TYPE(&malloc, void *(*)(size_t));
}

/* Every size: the whole space can be written and read back. */
TEST_OPT(whole_space_is_writable, .ref = "C11 7.22.3.4p2")
{
	size_t	bad = 0;

	for (int i = 0; sl_sizes(i) && !bad; i++)
	{
		size_t	n = sl_sizes(i);
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

/* C99 to C17 (and POSIX): aligned for every fundamental type, whatever the size. */
TEST_OPT(aligned_for_every_fundamental_type, .until = "C23", .ref = "C11 7.22.3p1")
{
	const char	*bad = NULL;
	size_t		at = 0;

	for (int i = 0; sl_sizes(i) && !bad; i++)
	{
		void	*p = malloc(sl_sizes(i));

		if (p != NULL)
		{
			bad = sl_misalignedType(p, sl_sizes(i), 0);
			at = sl_sizes(i);
			free(p);
		}
	}
	if (bad)
		LOG(bad);
	EXPECT_UEQ(bad ? at : 0, 0);
}

/* C23: aligned for every fundamental type whose size fits in the request. */
TEST_OPT(aligned_for_every_type_that_fits, .since = "C23", .ref = "C23 7.24.3p1")
{
	const char	*bad = NULL;
	size_t		at = 0;

	for (int i = 0; sl_sizes(i) && !bad; i++)
	{
		void	*p = malloc(sl_sizes(i));

		if (p != NULL)
		{
			bad = sl_misalignedType(p, sl_sizes(i), 1);
			at = sl_sizes(i);
			free(p);
		}
	}
	if (bad)
		LOG(bad);
	EXPECT_UEQ(bad ? at : 0, 0);
}

/* The space holds an object, and an array of them, of the strictest types. */
TEST_OPT(holds_objects_of_any_type, .ref = "C11 7.22.3p1")
{
	long double	*ld = malloc(4 * sizeof(long double));
	long long	*ll = malloc(sizeof(long long));
	void		**pp = malloc(3 * sizeof(void *));

	ASSERT_NOTNULL(ld);
	ASSERT_NOTNULL(ll);
	ASSERT_NOTNULL(pp);
	for (int i = 0; i < 4; i++)
		ld[i] = (long double)i / 3;
	*ll = -1234567890123LL;
	pp[0] = ld;
	pp[2] = ll;
	EXPECT_TRUE(ld[3] == (long double)3 / 3 && *ll == -1234567890123LL);
	EXPECT_PTREQ(pp[0], ld);
	EXPECT_PTREQ(pp[2], ll);
	free(ld);
	free(ll);
	free(pp);
}

/*
 * Live allocations of every size class up to 1 MiB are disjoint: no two
 * overlap, and each keeps its own contents while the others are written.
 */
TEST_OPT(allocations_are_disjoint, .ref = "C11 7.22.3p1")
{
	static void		*p[1200];
	static size_t	n[1200];
	int				cnt = 0;
	int				bad = -1;

	for (int i = 0; sl_sizes(i) && sl_sizes(i) <= ((size_t)1 << 20) + 1 && cnt < 1200;
		i += 1 + (i < 1100) * 3)
	{
		n[cnt] = sl_sizes(i);
		p[cnt] = malloc(n[cnt]);
		ASSERT_NOTNULL(p[cnt]);
		sl_fill(p[cnt], n[cnt], (unsigned)cnt);
		cnt++;
	}
	for (int i = 0; i < cnt && bad < 0; i++)
	{
		if (!sl_check(p[i], n[i], (unsigned)i))
			bad = i;
		for (int j = 0; j < i && bad < 0; j++)
			if (!sl_disjoint(p[i], n[i], p[j], n[j]))
				bad = i;
	}
	for (int i = 0; i < cnt; i++)
		free(p[i]);
	EXPECT_EQ(bad, -1);
}

/*
 * Many small blocks of one size (beyond one page of the allocator), all
 * live at once: distinct, and each keeps its contents.
 */
TEST_OPT(many_blocks_of_one_size, .ref = "C11 7.22.3p1")
{
	static unsigned char	*p[20000];
	int						bad = -1;

	for (int i = 0; i < 20000; i++)
	{
		p[i] = malloc(24);
		ASSERT_NOTNULL(p[i]);
		sl_fill(p[i], 24, (unsigned)i);
	}
	for (int i = 0; i < 20000 && bad < 0; i++)
		if (!sl_check(p[i], 24, (unsigned)i))
			bad = i;
	for (int i = 0; i < 20000; i++)
		free(p[i]);
	EXPECT_EQ(bad, -1);
}

/* A large block: its first and last bytes, and every page in between. */
TEST_OPT(large_block_is_writable, .ref = "C11 7.22.3.4p2")
{
	size_t			n = (size_t)256 << 20;
	unsigned char	*p = malloc(n);
	size_t			bad = 0;

	if (p == NULL)
		SKIP("no 256 MiB block on this system");
	for (size_t i = 0; i < n; i += 4096)
		p[i] = (unsigned char)(i >> 12);
	p[n - 1] = 0x5a;
	for (size_t i = 0; i < n && !bad; i += 4096)
		if (p[i] != (unsigned char)(i >> 12))
			bad = i + 1;
	EXPECT_UEQ(bad, 0);
	EXPECT_UEQ(p[n - 1], 0x5a);
	free(p);
}

/*
 * Size zero: a null pointer, or a pointer that free() accepts and that
 * differs from any other live allocation (as if a nonzero size: disjoint;
 * POSIX.1-2001 to 2017: "a unique pointer").
 */
TEST_OPT(zero_size_is_null_or_unique, .ref = "C11 7.22.3p1")
{
	void	*a = malloc(0);
	void	*b = malloc(0);
	void	*c = malloc(1);

	ASSERT_NOTNULL(c);
	if (a == NULL || b == NULL)
		LOG("malloc(0) returns a null pointer");
	EXPECT_TRUE(a == NULL || a != b);
	EXPECT_TRUE(a == NULL || a != c);
	EXPECT_TRUE(b == NULL || b != c);
	free(a);
	free(b);
	free(c);
}

/* Sizes no system can give: a null pointer (and the next calls still work). */
TEST_OPT(impossible_size_returns_null, .ref = "C11 7.22.3p1")
{
	void	*p;

	EXPECT_NULL(malloc(SIZE_MAX));
	EXPECT_NULL(malloc(SIZE_MAX - 4095));
	EXPECT_NULL(malloc(SIZE_MAX / 2 + 1));
	p = malloc(16);
	EXPECT_NOTNULL(p);
	free(p);
}

/* POSIX (CX): the failure sets errno to ENOMEM. */
TEST_OPT(impossible_size_sets_enomem, .since = "POSIX.1-2001", .ref = "XSH malloc() ERRORS")
{
	errno = 0;
	EXPECT_NULL(malloc(SIZE_MAX));
	EXPECT_EQ(errno, ENOMEM);
	errno = 0;
	EXPECT_NULL(malloc(SIZE_MAX - 4095));
	EXPECT_EQ(errno, ENOMEM);
	errno = 0;
	EXPECT_NULL(malloc(SIZE_MAX / 2 + 1));
	EXPECT_EQ(errno, ENOMEM);
}

/* Blocks allocated and freed in turn: the space of each is usable. */
TEST_OPT(allocations_after_frees, .ref = "C11 7.22.3p1")
{
	int	bad = -1;

	for (int round = 0; round < 64 && bad < 0; round++)
	{
		void	*p[32];

		for (int i = 0; i < 32; i++)
		{
			size_t	n = sl_sizes((round * 37 + i * 101) % 1127);

			p[i] = malloc(n);
			if (p[i] == NULL)
				bad = round;
			else
				sl_fill(p[i], n, (unsigned)i);
		}
		for (int i = 0; i < 32 && bad < 0; i++)
			if (!sl_check(p[i], sl_sizes((round * 37 + i * 101) % 1127), (unsigned)i))
				bad = round;
		for (int i = round % 2; i < 32; i += 2)
			free(p[i]);
		for (int i = 1 - round % 2; i < 32; i += 2)
			free(p[i]);
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	void	*a = malloc(8);
	void	*b = (malloc)(8);

	EXPECT_NOTNULL(a);
	EXPECT_NOTNULL(b);
	EXPECT_PTRNE(a, b);
	free(a);
	free(b);
}
