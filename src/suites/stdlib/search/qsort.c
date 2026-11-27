/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file qsort.c
 * @brief Tests for qsort() (<stdlib.h>).
 * @Created: 2026/11/27 13:03:43 by Moutig
 * @Updated: 2026/11/27 13:10:45 by Moutig
 *
 * qsort() sorts an array of nmemb objects of size bytes into ascending
 * order according to compar, which returns less than, equal to or greater
 * than zero; the order of equal elements is unspecified (7.24.5.2). Both
 * arguments of compar point to elements of the array; elements may be
 * reordered between calls but never altered; nmemb may be zero: compar
 * is not called and nothing moves, the pointers still valid (7.24.5p1-3).
 * POSIX says the same ("If the nel argument has the value zero, the
 * comparison function ... shall not be called").
 *
 * Every comparator here defines a total order on the array (7.24.5p4):
 * the adversary of McIlroy ("A Killer Adversary for Quicksort", 1999)
 * too, which decides the values while qsort() runs to make a quicksort
 * as slow as it can be.
 *
 * Clauses: C99 7.20.5, 7.20.5.2; C11, C17 7.22.5, 7.22.5.2; C23 7.24.5,
 * 7.24.5.2. POSIX: XSH qsort().
 */
#include "stdlib/stdlib_cases.h"
#include <limits.h>

TEST_OPT(prototype, .ref = "C11 7.22.5.2p1")
{
	EXPECT_TYPE(&qsort, void (*)(void *, size_t, size_t, int (*)(const void *, const void *)));
}

/* ----- Comparators and checks ----- */

/* What the comparators saw: calls, and arguments outside the array. */
static struct
{
	const char	*base;
	size_t		n;
	size_t		size;
	long		calls;
	long		outside;
}	g_seen;

static void	watch(const void *base, size_t n, size_t size)
{
	g_seen.base = base;
	g_seen.n = n;
	g_seen.size = size;
	g_seen.calls = 0;
	g_seen.outside = 0;
}

/* Count the call; is p the start of an element of the watched array? */
static void	see(const void *p)
{
	const char	*c = p;

	if (c < g_seen.base || c >= g_seen.base + g_seen.n * g_seen.size
		|| (size_t)(c - g_seen.base) % g_seen.size != 0)
		g_seen.outside++;
}

static int	cmpInt(const void *a, const void *b)
{
	int	x = *(const int *)a;
	int	y = *(const int *)b;

	g_seen.calls++;
	see(a);
	see(b);
	return ((x > y) - (x < y));
}

/* Only the sign counts: extreme magnitudes. */
static int	cmpIntExtreme(const void *a, const void *b)
{
	int	x = *(const int *)a;
	int	y = *(const int *)b;

	return (x < y ? INT_MIN : x > y ? INT_MAX : 0);
}

/* Is a[0 .. n) in ascending order? */
static int	ascending(const int *a, size_t n)
{
	for (size_t i = 1; i < n; i++)
		if (a[i - 1] > a[i])
			return (0);
	return (1);
}

/* Fill a[0 .. n) with pattern @p kind (values in 0 ... 65535). */
static void	pattern(int *a, size_t n, int kind, unsigned *seed)
{
	for (size_t i = 0; i < n; i++)
	{
		size_t	r = n - 1 - i;

		*seed = *seed * 1103515245u + 12345u;
		switch (kind)
		{
			case 0: a[i] = (int)((*seed >> 8) & 0xffff); break ;		/* random */
			case 1: a[i] = (int)(i & 0xffff); break ;					/* sorted */
			case 2: a[i] = (int)(r & 0xffff); break ;					/* reversed */
			case 3: a[i] = 7; break ;									/* all equal */
			case 4: a[i] = (int)((*seed >> 8) % 4); break ;				/* few values */
			case 5: a[i] = (int)((i < n / 2 ? i : r) & 0xffff); break ;	/* organ pipe */
			case 6: a[i] = (int)(i % 17); break ;						/* sawtooth */
			case 7: a[i] = (int)((i ^ 1) & 0xffff); break ;				/* pairs swapped */
			default: a[i] = (int)(i & 0xffff); if (i % 50 == 0) a[i] = (int)((*seed >> 8) & 0xffff);
		}
	}
}

# define N_PATTERNS	9

/* Count of each value 0 ... 65535 of a[0 .. n), added to (sign 1) or taken from (-1) cnt. */
static void	countValues(long *cnt, const int *a, size_t n, int sign)
{
	for (size_t i = 0; i < n; i++)
		cnt[a[i] & 0xffff] += sign;
}

/*
 * Sort a[0 .. n) of pattern @p kind: ascending, the same values, compar
 * only on elements of the array.
 *
 * @return 1 when it held.
 */
static int	sortsPattern(int *a, size_t n, int kind, unsigned *seed)
{
	static long	cnt[65536];
	int			ok;

	pattern(a, n, kind, seed);
	countValues(cnt, a, n, 1);
	watch(a, n, sizeof(int));
	qsort(a, n, sizeof(int), cmpInt);
	countValues(cnt, a, n, -1);
	ok = ascending(a, n) && g_seen.outside == 0;
	for (size_t i = 0; i < n; i++)
		if (cnt[a[i] & 0xffff] != 0)
		{
			ok = 0;
			cnt[a[i] & 0xffff] = 0;
		}
	return (ok);
}

/* ----- Cases ----- */

/*
 * Every length 0 ... 300 (around the small-array cut-offs of common
 * implementations) and 9 patterns: random, sorted, reversed, all equal,
 * few values, organ pipe, sawtooth, swapped pairs, nearly sorted.
 */
TEST_OPT(sorts_every_small_length_and_pattern, .ref = "C11 7.22.5.2p3")
{
	static int	a[300];
	unsigned	seed = 1;
	int			bad = -1;

	for (size_t n = 0; n <= 300 && bad < 0; n++)
		for (int k = 0; k < N_PATTERNS && bad < 0; k++)
			if (!sortsPattern(a, n, k, &seed))
				bad = (int)n * 10 + k;
	EXPECT_EQ(bad, -1);
}

/* 200000 elements of each pattern: the same, at scale. */
TEST_OPT(sorts_large_arrays_of_every_pattern, .timeout = 60, .ref = "C11 7.22.5.2p3")
{
	static int	a[200000];
	unsigned	seed = 7;
	int			bad = -1;

	for (int k = 0; k < N_PATTERNS && bad < 0; k++)
		if (!sortsPattern(a, 200000, k, &seed))
			bad = k;
	EXPECT_EQ(bad, -1);
}

/* nmemb zero: compar is not called, nothing moves. */
TEST_OPT(zero_elements_does_nothing, .ref = "C11 7.22.5p1")
{
	int	a[3] = {3, 1, 2};

	watch(a, 3, sizeof(int));
	qsort(a, 0, sizeof(int), cmpInt);
	EXPECT_EQ(g_seen.calls, 0);
	EXPECT_TRUE(a[0] == 3 && a[1] == 1 && a[2] == 2);
}

TEST_OPT(one_element_is_unchanged, .ref = "C11 7.22.5.2p3")
{
	int	a[2] = {42, 99};

	watch(a, 1, sizeof(int));
	qsort(a, 1, sizeof(int), cmpInt);
	EXPECT_EQ(a[0], 42);
	EXPECT_EQ(a[1], 99);
	EXPECT_EQ(g_seen.outside, 0);
}

/* Only the sign of compar counts: INT_MIN and INT_MAX as results. */
TEST_OPT(only_the_sign_of_compar_counts, .ref = "C11 7.22.5.2p3")
{
	static int	a[1000];
	unsigned	seed = 3;

	pattern(a, 1000, 0, &seed);
	qsort(a, 1000, sizeof(int), cmpIntExtreme);
	EXPECT_TRUE(ascending(a, 1000));
}

/* ----- Elements of any size ----- */

/* An element: a key, its original position, bytes made of both. */
static unsigned char	recordByte(unsigned key, unsigned id, size_t i)
{
	return ((unsigned char)(key * 31u + id * 7u + (unsigned)i * 13u + 1u));
}

static void	makeRecord(unsigned char *e, size_t size, unsigned key, unsigned id)
{
	for (size_t i = 0; i < size; i++)
		e[i] = recordByte(key, id, i);
	e[0] = (unsigned char)key;
	if (size >= 3)
		e[1] = (unsigned char)id, e[2] = (unsigned char)(id >> 8);
}

/* Records compare by their first byte (the key). */
static int	cmpRecord(const void *a, const void *b)
{
	unsigned	x = *(const unsigned char *)a;
	unsigned	y = *(const unsigned char *)b;

	g_seen.calls++;
	see(a);
	see(b);
	return ((x > y) - (x < y));
}

/*
 * Sort @p n records of @p size bytes at @p base: keys ascending, every
 * record whole (its bytes still those of its key and position), each
 * original record once.
 */
static int	sortsRecords(unsigned char *base, size_t n, size_t size, unsigned *seed)
{
	static unsigned char	seenId[4096];
	int						ok = 1;

	for (size_t i = 0; i < n; i++)
	{
		*seed = *seed * 1103515245u + 12345u;
		makeRecord(base + i * size, size, (*seed >> 16) % 50, (unsigned)i);
	}
	watch(base, n, size);
	qsort(base, n, size, cmpRecord);
	lc_fill(seenId, 0, sizeof(seenId));
	for (size_t i = 0; i < n && ok; i++)
	{
		unsigned char	*e = base + i * size;
		unsigned		key = e[0];
		unsigned		id = size >= 3 ? (unsigned)e[1] | (unsigned)e[2] << 8 : 0;

		if (i > 0 && e[-(long)size] > key)
			ok = 0;
		for (size_t j = 3; j < size && ok; j++)
			if (e[j] != recordByte(key, id, j))
				ok = 0;
		if (size >= 3 && (id >= n || seenId[id]++))
			ok = 0;
	}
	return (ok && g_seen.outside == 0);
}

/*
 * Elements of 1 to 1000 bytes, sizes that are not powers of two or not
 * multiples of a word: each element is moved whole, never altered.
 */
TEST_OPT(elements_of_any_size_are_moved_whole, .ref = "C11 7.22.5p3")
{
	static const size_t		sizes[] = {1, 2, 3, 4, 5, 7, 8, 12, 16, 24, 33, 64, 100, 257, 1000};
	static unsigned char	buf[1000 * 600];
	unsigned				seed = 11;
	int						bad = -1;

	for (int k = 0; k < (int)(sizeof(sizes) / sizeof(sizes[0])) && bad < 0; k++)
	{
		size_t	n = sizes[k] >= 3 ? 600 : 200;

		if (!sortsRecords(buf, n, sizes[k], &seed))
			bad = k;
	}
	EXPECT_EQ(bad, -1);
}

/* An array at an odd address, elements of 3 bytes: no alignment assumed. */
TEST_OPT(unaligned_array_and_odd_size, .ref = "C11 7.22.5.2p2")
{
	static unsigned char	buf[3 * 500 + 8];
	unsigned				seed = 5;

	EXPECT_TRUE(sortsRecords(buf + 1, 500, 3, &seed));
	EXPECT_TRUE(sortsRecords(buf + 3, 31, 7, &seed));
}

/* ----- An adversary ----- */

/* McIlroy's adversary: values are frozen as late as possible ("gas"). */
static struct
{
	int		*val;
	int		gas;
	int		solid;
	int		candidate;
}	g_adv;

static int	cmpAdversary(const void *a, const void *b)
{
	int	x = *(const int *)a;
	int	y = *(const int *)b;

	g_seen.calls++;
	see(a);
	see(b);
	if (g_adv.val[x] == g_adv.gas && g_adv.val[y] == g_adv.gas)
		g_adv.val[x == g_adv.candidate ? x : y] = g_adv.solid++;
	if (g_adv.val[x] == g_adv.gas)
		g_adv.candidate = x;
	else if (g_adv.val[y] == g_adv.gas)
		g_adv.candidate = y;
	return ((g_adv.val[x] > g_adv.val[y]) - (g_adv.val[x] < g_adv.val[y]));
}

/*
 * Against the adversary (a total order, decided late to make every pivot
 * a bad one): the array still ends up sorted by the values it decided.
 */
TEST_OPT(sorts_against_an_adversarial_comparator, .timeout = 60, .ref = "C11 7.22.5.2p3")
{
	static int	idx[20000];
	static int	val[20000];
	int			bad = -1;

	for (int n = 1; n <= 20000 && bad < 0; n = n < 100 ? n + 1 : n * 3)
	{
		for (int i = 0; i < n; i++)
		{
			idx[i] = i;
			val[i] = n;
		}
		g_adv.val = val;
		g_adv.gas = n;
		g_adv.solid = 0;
		g_adv.candidate = 0;
		watch(idx, (size_t)n, sizeof(int));
		qsort(idx, (size_t)n, sizeof(int), cmpAdversary);
		for (int i = 1; i < n && bad < 0; i++)
			if (val[idx[i - 1]] > val[idx[i]])
				bad = n;
		if (g_seen.outside != 0)
			bad = n;
	}
	EXPECT_EQ(bad, -1);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	int	a[4] = {4, 2, 3, 1};
	int	b[4] = {4, 2, 3, 1};

	watch(a, 4, sizeof(int));
	qsort(a, 4, sizeof(int), cmpInt);
	watch(b, 4, sizeof(int));
	(qsort)(b, 4, sizeof(int), cmpInt);
	EXPECT_TRUE(ascending(a, 4) && ascending(b, 4));
}
