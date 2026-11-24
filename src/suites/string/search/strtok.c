/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strtok.c
 * @brief Tests for strtok() (<string.h>).
 * @Created: 2026/11/24 05:25:20 by Moutig
 * @Updated: 2026/11/24 06:13:49 by Moutig
 *
 * A sequence of strtok() calls splits s1 into tokens delimited by
 * characters of s2, which may change from call to call. The first call
 * takes s1, the next ones a null pointer. Leading delimiters are skipped,
 * the character ending a token is overwritten with a null character, and
 * a null pointer is returned when no token remains (then on every later
 * call).
 *
 * Clauses: C99 7.21.5.8; C11, C17 7.24.5.8; C23 7.26.5.9. POSIX: XSH
 * strtok().
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "C11 7.24.5.8p1")
{
	EXPECT_TYPE(&strtok, char *(*)(char *restrict, const char *restrict));
}

TEST_OPT(splits_into_tokens, .ref = "C11 7.24.5.8p3-4")
{
	char	s[] = "ab,cd,,ef";

	EXPECT_PTREQ(strtok(s, ","), s);
	EXPECT_STREQ(s, "ab");
	EXPECT_PTREQ(strtok(NULL, ","), s + 3);
	EXPECT_PTREQ(strtok(NULL, ","), s + 7);
	EXPECT_NULL(strtok(NULL, ","));
}

TEST_OPT(leading_delimiters_skipped, .ref = "C11 7.24.5.8p3")
{
	char	s[] = "  ;ab";

	EXPECT_PTREQ(strtok(s, " ;"), s + 3);
	EXPECT_NULL(strtok(NULL, " ;"));
}

TEST_OPT(no_token_returns_null, .ref = "C11 7.24.5.8p3")
{
	char	s[] = ",,,";
	char	e[] = "";

	EXPECT_NULL(strtok(s, ","));
	EXPECT_NULL(strtok(e, ","));
}

TEST_OPT(delimiter_overwritten_with_nul, .ref = "C11 7.24.5.8p4")
{
	char	s[] = "ab;;cd";

	(void)strtok(s, ";");
	EXPECT_MEMEQ(s, "ab\0;cd", 7);
}

TEST_OPT(separators_may_change, .ref = "C11 7.24.5.8p2 (example)")
{
	static char	str[] = "?a???b,,,#c";

	EXPECT_PTREQ(strtok(str, "?"), str + 1);
	EXPECT_PTREQ(strtok(NULL, ","), str + 3);
	EXPECT_STREQ(str + 3, "??b");
	EXPECT_PTREQ(strtok(NULL, "#,"), str + 10);
	EXPECT_NULL(strtok(NULL, "?"));
}

TEST_OPT(null_after_last_token, .ref = "C11 7.24.5.8p3")
{
	char	s[] = "ab";

	EXPECT_PTREQ(strtok(s, ","), s);
	EXPECT_NULL(strtok(NULL, ","));
	EXPECT_NULL(strtok(NULL, ","));
}

TEST_OPT(empty_delimiter_set, .ref = "C11 7.24.5.8p3")
{
	char	s[] = "a b";

	EXPECT_PTREQ(strtok(s, ""), s);
	EXPECT_STREQ(s, "a b");
	EXPECT_NULL(strtok(NULL, ""));
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	char	a[] = "x,y";
	char	b[] = "x,y";

	EXPECT_PTREQ(strtok(a, ","), a);
	EXPECT_PTREQ((strtok)(b, ","), b);
	EXPECT_STREQ(a, b);
}
