/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file index.c
 * @brief Tests for index() (<strings.h>, POSIX.1-2001 LEGACY).
 * @Created: 2026/11/24 11:09:17 by Moutig
 * @Updated: 2026/11/24 11:14:14 by Moutig
 *
 * index() is equivalent to strchr(): the first c (converted to char) in
 * the string s, the terminating null character included, or a null
 * pointer.
 *
 * LEGACY (XSI) in POSIX.1-2001 and removed in POSIX.1-2008: this unit only
 * runs against a POSIX.1-2001 target with XSI.
 *
 * POSIX: XSH index() (POSIX.1-2001), XSH strchr().
 */
#include "strings/strings_cases.h"

TEST_OPT(prototype, .ref = "POSIX index() SYNOPSIS")
{
	EXPECT_TYPE(&index, char *(*)(const char *, int));
}

TEST_OPT(finds_first_occurrence, .ref = "POSIX strchr() RETURN VALUE")
{
	static const char	s[] = "abcabc";

	EXPECT_PTREQ(index(s, 'b'), s + 1);
}

TEST_OPT(not_found_returns_null, .ref = "POSIX strchr() RETURN VALUE")
{
	EXPECT_NULL(index("abc", 'z'));
	EXPECT_NULL(index("ab\0c", 'c'));
}

TEST_OPT(finds_terminating_nul, .ref = "POSIX strchr() DESCRIPTION")
{
	static const char	s[] = "abc";

	EXPECT_PTREQ(index(s, '\0'), s + 3);
}

TEST_OPT(c_converted_to_char, .ref = "POSIX strchr() DESCRIPTION")
{
	static const char	s[] = "a\xe9" "b";

	EXPECT_PTREQ(index(s, 0xe9), s + 1);
	EXPECT_PTREQ(index(s, 'b' + 256), s + 2);
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX index() SYNOPSIS")
{
	static const char	s[] = "abc";

	EXPECT_PTREQ(index(s, 'c'), (index)(s, 'c'));
}
