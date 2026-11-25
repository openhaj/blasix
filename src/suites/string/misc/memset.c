/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memset.c
 * @brief Tests for memset() (<string.h>).
 * @Created: 2026/11/24 05:17:59 by Moutig
 * @Updated: 2026/11/25 12:10:41 by Moutig
 *
 * memset() copies c, converted to unsigned char, into the first n
 * characters of s and returns s.
 *
 * Clauses: C99 7.21.6.1; C11, C17 7.24.6.1; C23 7.26.6.1. POSIX: XSH
 * memset(); POSIX.1-2024: errno unchanged on valid input.
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.6.1p1")
{
	EXPECT_TYPE(&memset, void *(*)(void *, int, size_t));
}

TEST_OPT(fills_n_bytes_and_returns_s, .ref = "C11 7.24.6.1p2-3")
{
	char	buf[6] = "abcde";

	EXPECT_PTREQ(memset(buf, 'z', 3), buf);
	EXPECT_STREQ(buf, "zzzde");
}

TEST_OPT(value_converted_to_unsigned_char, .ref = "C11 7.24.6.1p2")
{
	unsigned char	buf[3] = {0, 0, 0};

	(void)memset(buf, 0x141, 1);
	(void)memset(buf + 1, -1, 1);
	(void)memset(buf + 2, UCHAR_MAX + 1, 1);
	EXPECT_UEQ(buf[0], 0x41);
	EXPECT_UEQ(buf[1], UCHAR_MAX);
	EXPECT_UEQ(buf[2], 0);
}

TEST_OPT(zero_length_writes_nothing, .ref = "C11 7.24.1p2")
{
	char	buf[4] = "abc";

	EXPECT_PTREQ(memset(buf, 'z', 0), buf);
	EXPECT_STREQ(buf, "abc");
}

TEST_OPT(every_size_and_alignment, .ref = "C11 7.24.6.1p2")
{
	unsigned char	buf[96];
	int				bad = -1;

	for (int len = 0; len <= 64 && bad < 0; len++)
		for (int off = 0; off < 8 && bad < 0; off++)
		{
			lc_fill(buf, 0xee, sizeof(buf));
			(void)memset(buf + off, 0x5a, (size_t)len);
			for (int i = 0; i < 96 && bad < 0; i++)
				if (buf[i] != ((i >= off && i < off + len) ? 0x5a : 0xee))
					bad = len * 8 + off;
		}
	EXPECT_EQ(bad, -1);
}

/* Alignments of the destination, mod a 64-byte vector. */
static const int	g_offs[] = {0, 1, 7, 8, 15, 16, 31, 32, 33, 63};

# define MS_NOFF	((int)(sizeof(g_offs) / sizeof(g_offs[0])))
# define MS_MAXN	520
# define MS_PAD		64

/*
 * Every length 0 ... 520 (below half a vector, up to 2 and 4 vectors,
 * several turns of a 4-vector loop for vectors of 8 to 64 bytes) at every
 * alignment, with values whose splat must not be sign-extended: exactly
 * the n bytes hold the value, nothing around them is written.
 */
TEST_OPT(every_size_class_alignment_and_value, .ref = "C11 7.24.6.1p2")
{
	static const int		vals[] = {0x00, 0x5a, 0x80, 0xff, -2};
	static unsigned char	buf[MS_PAD + MS_MAXN + MS_PAD];
	int						bad = -1;

	for (int v = 0; v < (int)(sizeof(vals) / sizeof(vals[0])) && bad < 0; v++)
		for (int n = 0; n <= MS_MAXN && bad < 0; n++)
			for (int a = 0; a < MS_NOFF && bad < 0; a++)
			{
				int				o = g_offs[a];
				unsigned char	c = (unsigned char)vals[v];
				unsigned char	fill = (unsigned char)(c ^ 0xa5);

				lc_fill(buf, fill, sizeof(buf));
				if (memset(buf + o, vals[v], (size_t)n) != buf + o)
					bad = (v * (MS_MAXN + 1) + n) * MS_NOFF + a;
				for (int i = 0; i < (int)sizeof(buf) && bad < 0; i++)
					if (buf[i] != ((i >= o && i < o + n) ? c : fill))
						bad = (v * (MS_MAXN + 1) + n) * MS_NOFF + a;
			}
	EXPECT_EQ(bad, -1);
}

/* The n bytes end right before an inaccessible page: none past is written. */
TEST_OPT(fill_ending_at_page_end, .ref = "C11 7.24.6.1p2")
{
	unsigned char	*g = lc_guarded(MS_MAXN);
	int				bad = -1;

	if (g == NULL)
		SKIP("no guarded mapping");
	for (int n = 0; n <= MS_MAXN && bad < 0; n++)
	{
		lc_fill(g, 0xee, MS_MAXN);
		(void)memset(g + MS_MAXN - n, 0x11, (size_t)n);
		for (int i = 0; i < MS_MAXN && bad < 0; i++)
			if (g[i] != (i >= MS_MAXN - n ? 0x11 : 0xee))
				bad = n;
	}
	EXPECT_EQ(bad, -1);
}

/*
 * Large fills of a mapping (pages to megabytes, odd sizes and alignments),
 * ending within 64 bytes of a page end.
 */
TEST_OPT(large_fills_of_a_mapping, .ref = "C11 7.24.6.1p2")
{
	static const unsigned long	sizes[] = {4095, 4096, 4097, 65536 + 3,
		(1ul << 20) + 17, (8ul << 20) + 5};
	unsigned long				bad = 0;

	for (int k = 0; k < (int)(sizeof(sizes) / sizeof(sizes[0])) && !bad; k++)
	{
		unsigned long	n = sizes[k];
		unsigned char	*g = lc_guarded(n + 64);
		unsigned char	*d = g + 64 - (k * 13) % 64;

		if (g == NULL)
			SKIP("no guarded mapping");
		lc_fill(g, 0xee, n + 64);
		if (memset(d, 0xc3, n) != d)
			bad = n;
		for (unsigned char *p = g; p < g + n + 64 && !bad; p++)
			if (*p != ((p >= d && p < d + n) ? 0xc3 : 0xee))
				bad = n;
	}
	EXPECT_UEQ(bad, 0);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[4] = "abc";
	char	b[4] = "abc";

	(void)memset(a, 'q', 2);
	(void)(memset)(b, 'q', 2);
	EXPECT_STREQ(a, b);
}

ST_ERRNO_KEPT("POSIX memset()",
	char d[4];
	(void)memset(d, 0, sizeof(d));)
