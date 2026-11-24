/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file memmem.c
 * @brief Tests for memmem() (<string.h>).
 * @Created: 2026/11/24 05:17:19 by Moutig
 * @Updated: 2026/11/24 05:44:18 by Moutig
 *
 * memmem() (POSIX.1-2024) locates the first occurrence of the byte
 * sequence needle (needlelen bytes) in haystack (haystacklen bytes). It
 * returns haystack for needlelen 0, a null pointer if haystacklen is less
 * than needlelen or if the sequence is not found.
 *
 * POSIX: XSH memmem() (CX, Issue 8).
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX memmem() SYNOPSIS")
{
	EXPECT_TYPE(&memmem, void *(*)(const void *, size_t, const void *, size_t));
}

TEST_OPT(finds_first_occurrence, .ref = "POSIX memmem() RETURN VALUE")
{
	static const char	h[] = "abcabc";

	EXPECT_PTREQ(memmem(h, 6, "ca", 2), h + 2);
	EXPECT_PTREQ(memmem(h, 6, "abc", 3), h);
}

TEST_OPT(empty_needle_returns_haystack, .ref = "POSIX memmem() RETURN VALUE")
{
	static const char	h[] = "abc";

	EXPECT_PTREQ(memmem(h, 3, "x", 0), h);
	EXPECT_PTREQ(memmem(h, 0, "x", 0), h);
}

TEST_OPT(haystack_shorter_returns_null, .ref = "POSIX memmem() RETURN VALUE")
{
	EXPECT_NULL(memmem("ab", 2, "abc", 3));
	EXPECT_NULL(memmem("abc", 2, "abc", 3));
}

TEST_OPT(not_found_returns_null, .ref = "POSIX memmem() RETURN VALUE")
{
	EXPECT_NULL(memmem("abcdef", 6, "ce", 2));
	EXPECT_NULL(memmem("abcdef", 5, "f", 1));
}

/* Byte sequences: NUL bytes are ordinary. */
TEST_OPT(nul_bytes_are_ordinary, .ref = "POSIX memmem() DESCRIPTION")
{
	static const char	h[6] = {'a', '\0', 'b', '\0', 'c', 'd'};

	EXPECT_PTREQ(memmem(h, 6, "\0c", 2), h + 3);
	EXPECT_PTREQ(memmem(h, 6, "\0", 1), h + 1);
}

TEST_OPT(overlapping_partial_matches, .ref = "POSIX memmem() DESCRIPTION")
{
	static const char	h[] = "aaaaab";

	EXPECT_PTREQ(memmem(h, 6, "aab", 3), h + 3);
}

/* The match ends on the last byte of the haystack. */
TEST_OPT(match_at_the_end, .ref = "POSIX memmem() DESCRIPTION")
{
	char	*h = lc_guarded(5);

	ASSERT_NOTNULL(h);
	h[0] = 'x';
	h[1] = 'x';
	h[2] = 'x';
	h[3] = 'y';
	h[4] = 'z';
	EXPECT_PTREQ(memmem(h, 5, "yz", 2), h + 3);
	EXPECT_NULL(memmem(h, 5, "zz", 2));
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX memmem() SYNOPSIS")
{
	static const char	h[] = "abcabc";

	EXPECT_PTREQ(memmem(h, 6, "ca", 2), (memmem)(h, 6, "ca", 2));
}
