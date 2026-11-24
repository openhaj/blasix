/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strtok_r.c
 * @brief Tests for strtok_r() (<string.h>).
 * @Created: 2026/11/24 05:24:59 by Moutig
 * @Updated: 2026/11/24 06:13:37 by Moutig
 *
 * strtok_r() splits like strtok(), but keeps its state in the pointer
 * that state points to (need not be initialized), so several strings can
 * be scanned at once and strtok()'s own state is left alone.
 *
 * POSIX: XSH strtok() (strtok_r: TSF in POSIX.1-2001, base from
 * POSIX.1-2008; POSIX.1-2017 restates it as strtok() with a state).
 */
#include "string/string_cases.h"

TEST_OPT(prototype, .ref = "POSIX strtok_r() SYNOPSIS")
{
	EXPECT_TYPE(&strtok_r, char *(*)(char *restrict, const char *restrict, char **restrict));
}

TEST_OPT(splits_into_tokens, .ref = "POSIX strtok_r() DESCRIPTION")
{
	char	s[] = ",ab,,cd";
	char	*st;

	EXPECT_PTREQ(strtok_r(s, ",", &st), s + 1);
	EXPECT_STREQ(s + 1, "ab");
	EXPECT_PTREQ(strtok_r(NULL, ",", &st), s + 5);
	EXPECT_NULL(strtok_r(NULL, ",", &st));
}

TEST_OPT(no_token_returns_null, .ref = "POSIX strtok_r() RETURN VALUE")
{
	char	s[] = ";;";
	char	*st;

	EXPECT_NULL(strtok_r(s, ";", &st));
}

/* The state pointer need not be initialized before the first call. */
TEST_OPT(state_need_not_be_initialized, .ref = "POSIX strtok_r() DESCRIPTION")
{
	char	s[] = "a b";
	char	*st = (char *)1;

	EXPECT_PTREQ(strtok_r(s, " ", &st), s);
	EXPECT_PTREQ(strtok_r(NULL, " ", &st), s + 2);
}

TEST_OPT(two_strings_interleaved, .ref = "POSIX strtok_r() DESCRIPTION")
{
	char	a[] = "1 2 3";
	char	b[] = "x,y";
	char	*sa;
	char	*sb;

	EXPECT_PTREQ(strtok_r(a, " ", &sa), a);
	EXPECT_PTREQ(strtok_r(b, ",", &sb), b);
	EXPECT_PTREQ(strtok_r(NULL, " ", &sa), a + 2);
	EXPECT_PTREQ(strtok_r(NULL, ",", &sb), b + 2);
	EXPECT_PTREQ(strtok_r(NULL, " ", &sa), a + 4);
	EXPECT_NULL(strtok_r(NULL, ",", &sb));
}

TEST_OPT(does_not_disturb_strtok, .ref = "POSIX strtok() DESCRIPTION")
{
	char	a[] = "1 2";
	char	b[] = "x y";
	char	*st;

	EXPECT_PTREQ(strtok(a, " "), a);
	EXPECT_PTREQ(strtok_r(b, " ", &st), b);
	EXPECT_PTREQ(strtok_r(NULL, " ", &st), b + 2);
	EXPECT_PTREQ(strtok(NULL, " "), a + 2);
}

TEST_OPT(separators_may_change, .ref = "POSIX strtok_r() DESCRIPTION")
{
	char	s[] = "?a???b,,,#c";
	char	*st;

	EXPECT_PTREQ(strtok_r(s, "?", &st), s + 1);
	EXPECT_PTREQ(strtok_r(NULL, ",", &st), s + 3);
	EXPECT_PTREQ(strtok_r(NULL, "#,", &st), s + 10);
	EXPECT_NULL(strtok_r(NULL, "?", &st));
}

TEST_OPT(macro_and_function_agree, .ref = "POSIX strtok_r() SYNOPSIS")
{
	char	a[] = "x,y";
	char	b[] = "x,y";
	char	*sa;
	char	*sb;

	EXPECT_PTREQ(strtok_r(a, ",", &sa), a);
	EXPECT_PTREQ((strtok_r)(b, ",", &sb), b);
}
