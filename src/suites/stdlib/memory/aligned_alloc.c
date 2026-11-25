/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file aligned_alloc.c
 * @brief Tests for aligned_alloc() (<stdlib.h>, C11).
 * @Created: 2026/11/25 13:39:57 by Moutig
 * @Updated: 2026/11/25 13:41:00 by Moutig
 *
 * aligned_alloc() allocates space for an object of the given alignment
 * and size, of indeterminate value; it returns a null pointer or a
 * pointer to the space, with the guarantees of every allocation
 * (7.24.3p1). Valid alignments are the alignof values of the fundamental
 * types and an implementation-defined set of others, all powers of two
 * (6.2.8p4); above alignof(max_align_t) they are extended and may be
 * unsupported.
 *
 * C11: the alignment must be valid and the size a multiple of it (else
 * undefined: not tested). C17 (DR 460): an alignment the implementation
 * does not support makes it fail with a null pointer, and the size is
 * free. C23: the requirements of 7.24.3 hold even for a less strict
 * alignment (footnote 345). POSIX.1-2024 (CX): EINVAL for an invalid
 * alignment, ENOMEM for insufficient storage.
 *
 * Clauses: C11, C17 7.22.3.1; C23 7.24.3.1; 6.2.8. POSIX: XSH
 * aligned_alloc() (POSIX.1-2024).
 */
#include "stdlib/stdlib_cases.h"

TEST_OPT(prototype, .ref = "C11 7.22.3.1p1")
{
	EXPECT_TYPE(&aligned_alloc, void *(*)(size_t, size_t));
}

/*
 * Allocate @p size bytes aligned on @p align, check the alignment, write
 * and read back the space, free it.
 *
 * @return 1 when it held, 0 when it did not, -1 for a null pointer.
 */
static int	alignedAndWritable(size_t align, size_t size, unsigned id)
{
	void	*p = aligned_alloc(align, size);
	int		ok;

	if (p == NULL)
		return (-1);
	sl_fill(p, size, id);
	ok = (uintptr_t)p % align == 0 && sl_check(p, size, id);
	free(p);
	return (ok);
}

/*
 * Every fundamental alignment (1 to alignof(max_align_t)), sizes that
 * are multiples of it up to 64 of them and a large one: a block, aligned
 * and writable.
 */
TEST_OPT(fundamental_alignments, .ref = "C11 7.22.3.1p2")
{
	size_t	bad = 0;

	for (size_t a = 1; a <= SL_ALIGNOF(max_align_t) && !bad; a *= 2)
	{
		for (size_t k = 1; k <= 64 && !bad; k++)
			if (alignedAndWritable(a, a * k, (unsigned)k) != 1)
				bad = a * 1000 + k;
		if (!bad && alignedAndWritable(a, a * 65536, 1) != 1)
			bad = a * 1000;
	}
	EXPECT_UEQ(bad, 0);
}

/*
 * Extended alignments, 32 bytes to 1 MiB: may be unsupported (a null
 * pointer), but a block that is given has that alignment.
 */
TEST_OPT(extended_alignments_when_supported, .ref = "C11 6.2.8p3")
{
	size_t	bad = 0;
	int		given = 0;

	for (size_t a = SL_ALIGNOF(max_align_t) * 2; a <= ((size_t)1 << 20) && !bad; a *= 2)
		for (size_t k = 1; k <= 3 && !bad; k++)
		{
			int	r = alignedAndWritable(a, a * k, (unsigned)k);

			if (r == 0)
				bad = a;
			given += r == 1;
		}
	if (given == 0)
		LOG("no extended alignment is supported");
	EXPECT_UEQ(bad, 0);
}

/* Live aligned blocks are disjoint, and each keeps its contents. */
TEST_OPT(allocations_are_disjoint, .ref = "C11 7.22.3p1")
{
	static void		*p[120];
	static size_t	n[120];
	int				cnt = 0;
	int				bad = -1;

	for (size_t a = 1; a <= 4096 && cnt < 120; a *= 2)
		for (size_t k = 1; k <= 9 && cnt < 120; k++)
		{
			n[cnt] = a * k;
			p[cnt] = aligned_alloc(a, n[cnt]);
			if (p[cnt] == NULL)
				continue ;
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
	EXPECT_GT(cnt, 50);
	EXPECT_EQ(bad, -1);
}

/* C17: a size that is not a multiple of the alignment is allowed. */
TEST_OPT(any_size_from_c17, .since = "C17", .ref = "C17 7.22.3.1p2")
{
	static const size_t	cases[][2] = {{16, 17}, {16, 1}, {64, 1}, {64, 100}, {4096, 100},
		{4096, 4097}, {8, 13}, {2, 3}};
	int					bad = -1;

	for (int k = 0; k < (int)(sizeof(cases) / sizeof(cases[0])) && bad < 0; k++)
		if (alignedAndWritable(cases[k][0], cases[k][1], (unsigned)k) == 0)
			bad = k;
	EXPECT_EQ(bad, -1);
}

/*
 * C17: an alignment that is no valid alignment (not a power of two, or
 * zero) makes aligned_alloc() fail with a null pointer.
 */
TEST_OPT(invalid_alignment_returns_null, .since = "C17", .ref = "C17 7.22.3.1p2")
{
	static const size_t	bad[] = {0, 3, 5, 6, 12, 24, 48, 100, 4095, 4097};
	int					got = -1;

	for (int k = 0; k < (int)(sizeof(bad) / sizeof(bad[0])) && got < 0; k++)
	{
		void	*p = aligned_alloc(bad[k], bad[k] ? bad[k] * 4 : 64);

		if (p != NULL)
		{
			got = k;
			free(p);
		}
	}
	EXPECT_EQ(got, -1);
	EXPECT_NULL(aligned_alloc(SIZE_MAX, 64));
	EXPECT_NULL(aligned_alloc(SIZE_MAX / 2 + 2, 64));
}

/* POSIX.1-2024 (CX): EINVAL for an invalid alignment. */
TEST_OPT(invalid_alignment_sets_einval, .since = "POSIX.1-2024", .ref = "XSH aligned_alloc() ERRORS")
{
	errno = 0;
	EXPECT_NULL(aligned_alloc(3, 12));
	EXPECT_EQ(errno, EINVAL);
	errno = 0;
	EXPECT_NULL(aligned_alloc(24, 48));
	EXPECT_EQ(errno, EINVAL);
	errno = 0;
	EXPECT_NULL(aligned_alloc(0, 16));
	EXPECT_EQ(errno, EINVAL);
}

/*
 * C23: a less strict alignment than the size needs still gives the
 * alignment 7.24.3 requires for the types that fit (footnote 345).
 */
TEST_OPT(weak_alignment_still_fits_the_types, .since = "C23", .ref = "C23 7.24.3.1p2")
{
	const char	*bad = NULL;

	for (size_t a = 1; a <= 8 && !bad; a *= 2)
		for (size_t n = a; n <= 256 && !bad; n += a)
		{
			void	*p = aligned_alloc(a, n);

			ASSERT_NOTNULL(p);
			bad = sl_misalignedType(p, n, 1);
			free(p);
		}
	if (bad)
		LOG(bad);
	EXPECT_NULL(bad);
}

/* Sizes no system can give: a null pointer. */
TEST_OPT(impossible_size_returns_null, .ref = "C11 7.22.3p1")
{
	void	*p;

	EXPECT_NULL(aligned_alloc(16, SIZE_MAX - 15));
	EXPECT_NULL(aligned_alloc(4096, SIZE_MAX - 4095));
	EXPECT_NULL(aligned_alloc(64, SIZE_MAX / 2 + 1));
	p = aligned_alloc(64, 64);
	EXPECT_NOTNULL(p);
	free(p);
}

/* POSIX.1-2024 (CX): insufficient storage sets errno to ENOMEM. */
TEST_OPT(impossible_size_sets_enomem, .since = "POSIX.1-2024", .ref = "XSH aligned_alloc() ERRORS")
{
	errno = 0;
	EXPECT_NULL(aligned_alloc(16, SIZE_MAX - 15));
	EXPECT_EQ(errno, ENOMEM);
	errno = 0;
	EXPECT_NULL(aligned_alloc(64, SIZE_MAX / 2 + 1));
	EXPECT_EQ(errno, ENOMEM);
}

/* Size zero: a null pointer, or a unique pointer free() accepts. */
TEST_OPT(zero_size_is_null_or_unique, .ref = "C11 7.22.3p1")
{
	void	*a = aligned_alloc(64, 0);
	void	*b = aligned_alloc(64, 0);
	void	*c = aligned_alloc(64, 64);

	ASSERT_NOTNULL(c);
	EXPECT_TRUE(a == NULL || (a != b && a != c));
	EXPECT_TRUE(b == NULL || b != c);
	free(a);
	free(b);
	free(c);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	void	*a = aligned_alloc(256, 512);
	void	*b = (aligned_alloc)(256, 512);

	ASSERT_NOTNULL(a);
	ASSERT_NOTNULL(b);
	EXPECT_UEQ((uintptr_t)a % 256, 0);
	EXPECT_UEQ((uintptr_t)b % 256, 0);
	free(a);
	free(b);
}
