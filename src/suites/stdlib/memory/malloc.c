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
 * @Updated: 2026/11/25 14:04:06 by Moutig
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
 * Stress (POSIX): 8 threads allocating at once (C11 7.22.3p2, XSH 2.9.1:
 * thread-safe), and allocations in the child of fork() (XSH fork()).
 *
 * Clauses: C99 7.20.3, 7.20.3.3; C11, C17 7.22.3, 7.22.3.4; C23 7.24.3,
 * 7.24.3.6. POSIX: XSH malloc().
 */
#include "stdlib/stdlib_procs.h"

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

/* ----- Stress: threads and fork() (stdlib_procs.h) ----- */

/** One thread of concurrent_allocations_from_threads. */
typedef struct s_mWorker
{
	unsigned	id;
	int			bad;
}	t_mWorker;

/*
 * 20000 rounds over 64 slots: check the block of the slot (its pattern),
 * free it, allocate another size there (small, every 50th up to 64 KiB).
 */
static void	*allocWorker(void *arg)
{
	t_mWorker	*w = arg;
	void		*p[64];
	size_t		n[64];

	lc_fill(p, 0, sizeof(p));
	lc_fill(n, 0, sizeof(n));
	for (int r = 0; r < 20000 && !w->bad; r++)
	{
		int	k = (r * 7 + (int)w->id) % 64;
		int	i = r % 50 == 0 ? 1100 + (r / 50) % 18 : (r * 31 + (int)w->id * 17) % 1100;

		if (p[k] && !sl_check(p[k], n[k], w->id * 64 + (unsigned)k))
			w->bad = r + 1;
		free(p[k]);
		n[k] = sl_sizes(i);
		p[k] = malloc(n[k]);
		if (p[k] == NULL)
			w->bad = r + 1;
		else
			sl_fill(p[k], n[k], w->id * 64 + (unsigned)k);
	}
	for (int k = 0; k < 64; k++)
	{
		if (p[k] && !w->bad && !sl_check(p[k], n[k], w->id * 64 + (unsigned)k))
			w->bad = -1;
		free(p[k]);
	}
	return (NULL);
}

/*
 * 8 threads allocate and free at once: every block keeps the pattern its
 * thread wrote (no block given twice, none overlapping another thread's).
 */
TEST_OPT(concurrent_allocations_from_threads, .since = "POSIX.1-2008",
	.ref = "C11 7.22.3p2; XSH 2.9.1")
{
	t_mWorker	w[8];

	for (int i = 0; i < 8; i++)
	{
		w[i].id = (unsigned)i;
		w[i].bad = 0;
	}
	SL_THREADS(8, allocWorker, w);
	for (int i = 0; i < 8; i++)
		EXPECT_EQ(w[i].bad, 0);
}

/* Blocks of the parent, alive across fork(). */
static void		*g_parent[48];
static size_t	g_parentSize[48];

/*
 * In the child: the parent's blocks hold what the parent wrote; blocks of
 * every class up to 1 MiB can be allocated; half of the parent's blocks
 * are freed and the space allocated again.
 *
 * @return 0, or the step that failed.
 */
static int	childAllocates(void *arg)
{
	(void)arg;
	for (int i = 0; i < 48; i++)
		if (!sl_check(g_parent[i], g_parentSize[i], (unsigned)i))
			return (1);
	for (int i = 0; sl_sizes(i) && sl_sizes(i) <= ((size_t)1 << 20) + 1; i += 3)
	{
		void	*p = malloc(sl_sizes(i));

		if (p == NULL)
			return (2);
		sl_fill(p, sl_sizes(i), 99);
		if (!sl_check(p, sl_sizes(i), 99))
			return (3);
		free(p);
	}
	for (int i = 0; i < 48; i += 2)
	{
		free(g_parent[i]);
		g_parent[i] = malloc(g_parentSize[i]);
		if (g_parent[i] == NULL)
			return (4);
		sl_fill(g_parent[i], g_parentSize[i], 77);
	}
	for (int i = 1; i < 48; i += 2)
		if (!sl_check(g_parent[i], g_parentSize[i], (unsigned)i))
			return (5);
	return (0);
}

/*
 * The child of fork() of a single-threaded process can allocate and free,
 * including the blocks it got from the parent; the parent's blocks do not
 * change.
 */
TEST_OPT(allocation_in_the_child_of_fork, .since = "POSIX.1-2001", .ref = "XSH fork()")
{
	int	bad = -1;

	for (int i = 0; i < 48; i++)
	{
		g_parentSize[i] = sl_sizes(i * 23 % 1127);
		g_parent[i] = malloc(g_parentSize[i]);
		ASSERT_NOTNULL(g_parent[i]);
		sl_fill(g_parent[i], g_parentSize[i], (unsigned)i);
	}
	{
		SL_FORK(st, childAllocates, NULL);
		EXPECT_EQ(st, 0);
	}
	for (int i = 0; i < 48 && bad < 0; i++)
		if (!sl_check(g_parent[i], g_parentSize[i], (unsigned)i))
			bad = i;
	for (int i = 0; i < 48; i++)
		free(g_parent[i]);
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
