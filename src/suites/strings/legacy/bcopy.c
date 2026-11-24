/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file bcopy.c
 * @brief Tests for bcopy() (<strings.h>, POSIX.1-2001 LEGACY).
 * @Created: 2026/11/24 11:08:58 by Moutig
 * @Updated: 2026/11/24 11:13:39 by Moutig
 *
 * bcopy() copies n bytes from s1 to s2 (source first, unlike memcpy()),
 * correctly even if the areas overlap. It returns no value.
 *
 * LEGACY (XSI) in POSIX.1-2001 and removed in POSIX.1-2008: this unit only
 * runs against a POSIX.1-2001 target with XSI.
 *
 * POSIX: XSH bcopy() (POSIX.1-2001).
 */
#include "strings/strings_cases.h"

TEST_OPT(prototype, .ref = "POSIX bcopy() SYNOPSIS")
{
	EXPECT_TYPE(&bcopy, void (*)(const void *, void *, size_t));
}

TEST_OPT(copies_from_first_to_second, .ref = "POSIX bcopy() DESCRIPTION")
{
	char	dst[6] = "xxxxx";

	bcopy("a\0b", dst, 3);
	EXPECT_MEMEQ(dst, "a\0bxx", 6);
}

TEST_OPT(overlap_destination_after_source, .ref = "POSIX bcopy() DESCRIPTION")
{
	char	buf[11] = "0123456789";

	bcopy(buf, buf + 2, 6);
	EXPECT_STREQ(buf, "0101234589");
}

TEST_OPT(overlap_destination_before_source, .ref = "POSIX bcopy() DESCRIPTION")
{
	char	buf[11] = "0123456789";

	bcopy(buf + 3, buf, 6);
	EXPECT_STREQ(buf, "3456786789");
}

TEST_OPT(zero_length_copies_nothing, .ref = "POSIX bcopy() DESCRIPTION")
{
	char	buf[4] = "abc";

	bcopy("xyz", buf, 0);
	EXPECT_STREQ(buf, "abc");
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX bcopy() SYNOPSIS")
{
	char	a[4] = "xxx";
	char	b[4] = "xxx";

	bcopy("ab", a, 2);
	(bcopy)("ab", b, 2);
	EXPECT_STREQ(a, b);
}
