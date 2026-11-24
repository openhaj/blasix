/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file rindex.c
 * @brief Tests for rindex() (<strings.h>, POSIX.1-2001 LEGACY).
 * @Created: 2026/11/24 11:09:25 by Moutig
 * @Updated: 2026/11/24 11:14:35 by Moutig
 *
 * rindex() is equivalent to strrchr(): the last c (converted to char) in
 * the string s, the terminating null character included, or a null
 * pointer.
 *
 * LEGACY (XSI) in POSIX.1-2001 and removed in POSIX.1-2008: this unit only
 * runs against a POSIX.1-2001 target with XSI.
 *
 * POSIX: XSH rindex() (POSIX.1-2001), XSH strrchr().
 */
#include "strings/strings_cases.h"

TEST_OPT(prototype, .ref = "POSIX rindex() SYNOPSIS")
{
	EXPECT_TYPE(&rindex, char *(*)(const char *, int));
}

TEST_OPT(finds_last_occurrence, .ref = "POSIX strrchr() RETURN VALUE")
{
	static const char	s[] = "abcabc";

	EXPECT_PTREQ(rindex(s, 'b'), s + 4);
}

TEST_OPT(not_found_returns_null, .ref = "POSIX strrchr() RETURN VALUE")
{
	EXPECT_NULL(rindex("abc", 'z'));
}

TEST_OPT(finds_terminating_nul, .ref = "POSIX strrchr() DESCRIPTION")
{
	static const char	s[] = "abc";

	EXPECT_PTREQ(rindex(s, '\0'), s + 3);
}

TEST_OPT(does_not_search_past_nul, .ref = "POSIX strrchr() DESCRIPTION")
{
	static const char	s[] = "ab\0b";

	EXPECT_PTREQ(rindex(s, 'b'), s + 1);
}

TEST_OPT(c_converted_to_char, .ref = "POSIX strrchr() DESCRIPTION")
{
	static const char	s[] = "\xe9" "a\xe9";

	EXPECT_PTREQ(rindex(s, 0xe9), s + 2);
	EXPECT_PTREQ(rindex(s, 'a' + 256), s + 1);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX rindex() SYNOPSIS")
{
	static const char	s[] = "abca";

	EXPECT_PTREQ(rindex(s, 'a'), (rindex)(s, 'a'));
}
