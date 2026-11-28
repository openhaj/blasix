/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file getentropy.c
 * @brief Tests for getentropy() (<unistd.h>, POSIX.1-2024).
 * @Created: 2026/11/28 11:00:46 by Moutig
 * @Updated: 2026/11/28 11:04:34 by Moutig
 *
 * getentropy(buffer, length) writes length bytes of unpredictable data
 * from a cryptographically secure generator; a successful call always
 * provides all of them; 0 on success, -1 and errno otherwise: EINVAL for
 * a length greater than {GETENTROPY_MAX} (<limits.h>, at least 256).
 * Randomness itself cannot be proven: the cases check that every byte
 * is written and that two calls differ.
 *
 * Clauses: POSIX: XSH getentropy() (POSIX.1-2024).
 */
#include "unistd/unistd_cases.h"
#include <limits.h>

#ifndef GETENTROPY_MAX
# define GETENTROPY_MAX	256	/* <limits.h> without it: the value POSIX.1-2024 requires at least */
#endif

TEST_OPT(prototype, .ref = "XSH getentropy() (Issue 8)")
{
	EXPECT_TYPE(&getentropy, int (*)(void *, size_t));
}

/* Every length from 0 to GETENTROPY_MAX: 0, and no byte past length is touched. */
TEST_OPT(fills_every_length_up_to_the_maximum, .ref = "XSH getentropy()")
{
	unsigned char	b[GETENTROPY_MAX + 8];
	int				bad = -1;

	for (int n = 0; n <= GETENTROPY_MAX && bad < 0; n++)
	{
		lc_fill(b, 0xa5, sizeof(b));
		if (getentropy(b, (size_t)n) != 0)
			bad = n;
		for (int i = n; i < (int)sizeof(b) && bad < 0; i++)
			if (b[i] != 0xa5)
				bad = n;
	}
	EXPECT_EQ(bad, -1);
}

/* All the bytes are written: over many calls, each position takes other values than its old one. */
TEST_OPT(every_byte_is_written, .ref = "XSH getentropy()")
{
	unsigned char	b[GETENTROPY_MAX];
	unsigned char	seen0[GETENTROPY_MAX];
	unsigned char	seenFF[GETENTROPY_MAX];
	int				stuck = 0;

	lc_fill(seen0, 0, sizeof(seen0));
	lc_fill(seenFF, 0, sizeof(seenFF));
	for (int k = 0; k < 64; k++)
	{
		lc_fill(b, k % 2 ? 0x00 : 0xff, sizeof(b));
		ASSERT_EQ(getentropy(b, sizeof(b)), 0);
		for (int i = 0; i < (int)sizeof(b); i++)
		{
			seen0[i] |= b[i] != 0x00;
			seenFF[i] |= b[i] != 0xff;
		}
	}
	for (int i = 0; i < (int)sizeof(b); i++)
		stuck += !seen0[i] || !seenFF[i];
	EXPECT_EQ(stuck, 0);
}

/* Two calls give different data (2^-2048 to be wrong by chance). */
TEST_OPT(two_calls_differ, .ref = "XSH getentropy()")
{
	unsigned char	a[GETENTROPY_MAX];
	unsigned char	b[GETENTROPY_MAX];
	int				same = 1;

	ASSERT_EQ(getentropy(a, sizeof(a)), 0);
	ASSERT_EQ(getentropy(b, sizeof(b)), 0);
	for (int i = 0; i < (int)sizeof(a); i++)
		same = same && a[i] == b[i];
	EXPECT_FALSE(same);
}

/* More than GETENTROPY_MAX: -1 and EINVAL, the buffer untouched. */
TEST_OPT(too_long_gives_einval, .ref = "XSH getentropy() ERRORS")
{
	unsigned char	b[GETENTROPY_MAX + 1];

	lc_fill(b, 0x5a, sizeof(b));
	errno = 0;
	EXPECT_EQ(getentropy(b, GETENTROPY_MAX + 1), -1);
	EXPECT_EQ(errno, EINVAL);
	errno = 0;
	EXPECT_EQ(getentropy(b, (size_t)-1), -1);
	EXPECT_EQ(errno, EINVAL);
}

/* {GETENTROPY_MAX}: "Minimum Acceptable Value: 256" (XBD <limits.h>). */
TEST_OPT(maximum_is_at_least_256, .ref = "XBD <limits.h> (Issue 8)")
{
	EXPECT_GE(GETENTROPY_MAX, 256);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	unsigned char	b[16];

	EXPECT_EQ((getentropy)(b, sizeof(b)), 0);
}
