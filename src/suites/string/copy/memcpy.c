/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memcpy.c
 * @brief Tests for memcpy() (<string.h>).
 * @Created: 2026/11/24 05:17:11 by Moutig
 * @Updated: 2026/11/25 12:10:24 by Moutig
 *
 * memcpy() copies n characters from s2 to s1 and returns s1; overlapping
 * objects are undefined (not tested). Every character is taken as an
 * unsigned char, NUL included (C 7.24.1p3, C23 7.26.1p4); n may be 0.
 *
 * Clauses: C99 7.21.2.1; C11, C17 7.24.2.1; C23 7.26.2.1. POSIX: XSH
 * memcpy(); POSIX.1-2024 also forbids changing errno on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.2.1p1")
{
	EXPECT_TYPE(&memcpy, void *(*)(void *restrict, const void *restrict, size_t));
}

TEST_OPT(copies_n_bytes_and_returns_s1, .ref = "C11 7.24.2.1p2-3")
{
	char		dst[8] = "xxxxxxx";
	const char	src[5] = {'a', '\0', 'b', '\xff', 'c'};

	EXPECT_PTREQ(memcpy(dst, src, 5), dst);
	EXPECT_MEMEQ(dst, "a\0b\xff" "cxx", 8);
}

TEST_OPT(zero_length_copies_nothing, .ref = "C11 7.24.1p2")
{
	char	dst[4] = "abc";

	EXPECT_PTREQ(memcpy(dst, "xyz", 0), dst);
	EXPECT_STREQ(dst, "abc");
}

/* Every length 0 ... 64 at every alignment 0 ... 7, nothing written past. */
TEST_OPT(every_size_and_alignment, .ref = "C11 7.24.2.1p2")
{
	unsigned char	src[80];
	unsigned char	dst[96];
	int				bad = -1;

	for (int i = 0; i < 80; i++)
		src[i] = (unsigned char)(i * 37 + 1);
	for (int len = 0; len <= 64 && bad < 0; len++)
		for (int off = 0; off < 8 && bad < 0; off++)
		{
			lc_fill(dst, 0xee, sizeof(dst));
			if (memcpy(dst + off, src + (7 - off), (size_t)len) != dst + off)
				bad = len * 8 + off;
			for (int i = 0; i < 96 && bad < 0; i++)
			{
				int	in = i >= off && i < off + len;

				if (dst[i] != (in ? src[7 - off + i - off] : 0xee))
					bad = len * 8 + off;
			}
		}
	EXPECT_EQ(bad, -1);
}

/*
 * Byte i of the test pattern: no short period, so a copy from the wrong
 * offset or a block stored at the wrong place shows.
 */
static unsigned char	pattern(unsigned long i)
{
	return ((unsigned char)(i * 131u + (i >> 8) + 1u));
}

/* Alignments of the source and destination, mod a 64-byte vector. */
static const int	g_offs[] = {0, 1, 7, 8, 15, 16, 31, 32, 33, 63};

# define MC_NOFF	((int)(sizeof(g_offs) / sizeof(g_offs[0])))
# define MC_MAXN	520
# define MC_PAD		64

/*
 * Every length 0 ... 520 (below half a vector, up to 2 and 4 vectors,
 * several turns of a 4-vector loop for vectors of 8 to 64 bytes) with
 * every pair of source and destination alignments: the bytes copied are
 * the source's, nothing before or after the destination is written.
 */
TEST_OPT(every_size_class_and_alignment_pair, .ref = "C11 7.24.2.1p2")
{
	static unsigned char	src[MC_PAD + MC_MAXN];
	static unsigned char	dst[MC_PAD + MC_MAXN + MC_PAD];
	int						bad = -1;

	for (int i = 0; i < MC_PAD + MC_MAXN; i++)
		src[i] = pattern((unsigned long)i);
	for (int n = 0; n <= MC_MAXN && bad < 0; n++)
		for (int a = 0; a < MC_NOFF && bad < 0; a++)
			for (int b = 0; b < MC_NOFF && bad < 0; b++)
			{
				int	so = g_offs[a];
				int	d0 = g_offs[b];

				lc_fill(dst, 0xee, sizeof(dst));
				if (memcpy(dst + d0, src + so, (size_t)n) != dst + d0)
					bad = (n * MC_NOFF + a) * MC_NOFF + b;
				for (int i = 0; i < (int)sizeof(dst) && bad < 0; i++)
				{
					int	in = i >= d0 && i < d0 + n;

					if (dst[i] != (in ? src[so + i - d0] : 0xee))
						bad = (n * MC_NOFF + a) * MC_NOFF + b;
				}
			}
	EXPECT_EQ(bad, -1);
}

/*
 * Source and destination both end right before an inaccessible page:
 * no byte past the n of either may be read or written.
 */
TEST_OPT(copy_ending_at_page_end, .ref = "C11 7.24.2.1p2")
{
	unsigned char	*s = lc_guarded(MC_MAXN);
	unsigned char	*d = lc_guarded(MC_MAXN);
	int				bad = -1;

	if (s == NULL || d == NULL)
		SKIP("no guarded mapping");
	for (int i = 0; i < MC_MAXN; i++)
		s[i] = pattern((unsigned long)i);
	for (int n = 0; n <= MC_MAXN && bad < 0; n++)
	{
		unsigned char	*ds = d + MC_MAXN - n;
		unsigned char	*ss = s + MC_MAXN - n;

		lc_fill(d, 0xee, MC_MAXN);
		(void)memcpy(ds, ss, (size_t)n);
		for (int i = 0; i < n && bad < 0; i++)
			if (ds[i] != ss[i])
				bad = n;
		for (int i = 0; i < MC_MAXN - n && bad < 0; i++)
			if (d[i] != 0xee)
				bad = n;
	}
	EXPECT_EQ(bad, -1);
}

/*
 * Large copies between mappings (pages to megabytes, odd sizes and
 * alignments), source and destination ending within 64 bytes of a page
 * end.
 */
TEST_OPT(large_copies_between_mappings, .ref = "C11 7.24.2.1p2")
{
	static const unsigned long	sizes[] = {4095, 4096, 4097, 65536 + 3,
		(1ul << 20) + 17, (8ul << 20) + 5};
	unsigned long				bad = 0;

	for (int k = 0; k < (int)(sizeof(sizes) / sizeof(sizes[0])) && !bad; k++)
	{
		unsigned long	n = sizes[k];
		unsigned char	*s = lc_guarded(n + 64);
		unsigned char	*d = lc_guarded(n + 64);
		unsigned char	*ss = s + 64 - (k * 7) % 64;
		unsigned char	*dd = d + 64 - (k * 13) % 64;

		if (s == NULL || d == NULL)
			SKIP("no guarded mapping");
		for (unsigned long i = 0; i < n + 64; i++)
			s[i] = pattern(i);
		lc_fill(d, 0xee, n + 64);
		if (memcpy(dd, ss, n) != dd)
			bad = n;
		for (unsigned long i = 0; i < n && !bad; i++)
			if (dd[i] != ss[i])
				bad = n;
		for (unsigned char *p = d; p < dd && !bad; p++)
			if (*p != 0xee)
				bad = n;
		for (unsigned char *p = dd + n; p < d + n + 64 && !bad; p++)
			if (*p != 0xee)
				bad = n;
	}
	EXPECT_UEQ(bad, 0);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[4] = "xxx";
	char	b[4] = "xxx";

	(void)memcpy(a, "ab", 2);
	(void)(memcpy)(b, "ab", 2);
	EXPECT_MEMEQ(a, b, 4);
}

ST_ERRNO_KEPT("POSIX memcpy()",
	char d[4];
	(void)memcpy(d, "abc", 4);)
