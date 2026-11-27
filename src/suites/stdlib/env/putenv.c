/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file putenv.c
 * @brief Tests for putenv() (<stdlib.h>, XSI).
 * @Created: 2026/11/27 13:28:55 by Moutig
 * @Updated: 2026/11/27 13:29:52 by Moutig
 *
 * putenv(string), with string of the form "name=value", sets the
 * variable name to value, altering it or creating it; the string itself
 * becomes part of the environment, so altering it alters the environment,
 * until a new string for that name is passed. 0 on success, non-zero and
 * errno otherwise (ENOMEM may). POSIX.1-2024: a string that is a valid
 * name without '=' removes that variable or fails with EINVAL.
 *
 * The strings are static: they stay part of the environment.
 *
 * Clauses: POSIX: XSH putenv() (XSI, POSIX.1-2001 on).
 */
#include "stdlib/env_cases.h"

TEST_OPT(prototype, .ref = "XSH putenv()")
{
	EXPECT_TYPE(&putenv, int (*)(char *));
}

TEST_OPT(creates_a_variable, .ref = "XSH putenv()")
{
	static char	s[] = "BLASIX_PUT_NEW=put";

	EXPECT_EQ(putenv(s), 0);
	EXPECT_TRUE(ev_is("BLASIX_PUT_NEW", "put"));
	EXPECT_TRUE(ev_othersIntact(""));
}

TEST_OPT(alters_an_existing_variable, .ref = "XSH putenv()")
{
	static char	s[] = "BLASIX_ENV_PLAIN=altered";

	EXPECT_EQ(putenv(s), 0);
	EXPECT_TRUE(ev_is("BLASIX_ENV_PLAIN", "altered"));
	EXPECT_TRUE(ev_othersIntact("BLASIX_ENV_PLAIN"));
}

/* The string becomes part of the environment: environ holds it, getenv() points in it. */
TEST_OPT(string_becomes_part_of_the_environment, .ref = "XSH putenv()")
{
	static char	s[] = "BLASIX_PUT_SELF=mine";

	ASSERT_EQ(putenv(s), 0);
	EXPECT_PTREQ(ev_entry("BLASIX_PUT_SELF"), s);
	EXPECT_PTREQ(getenv("BLASIX_PUT_SELF"), s + 16);
}

/* Altering the string afterwards alters the environment. */
TEST_OPT(altering_the_string_alters_the_environment, .ref = "XSH putenv()")
{
	static char	s[] = "BLASIX_PUT_LIVE=aaaa";

	ASSERT_EQ(putenv(s), 0);
	s[16] = 'z';
	s[18] = '\0';
	EXPECT_TRUE(ev_is("BLASIX_PUT_LIVE", "za"));
}

/* A new string for the same name takes over: the old one is no longer used. */
TEST_OPT(new_string_for_the_name_takes_over, .ref = "XSH putenv()")
{
	static char	a[] = "BLASIX_PUT_TWICE=first";
	static char	b[] = "BLASIX_PUT_TWICE=second";

	ASSERT_EQ(putenv(a), 0);
	ASSERT_EQ(putenv(b), 0);
	EXPECT_TRUE(ev_is("BLASIX_PUT_TWICE", "second"));
	a[17] = 'X';
	EXPECT_TRUE(ev_is("BLASIX_PUT_TWICE", "second"));
}

/* An empty value and a value with '='. */
TEST_OPT(any_value, .ref = "XSH putenv()")
{
	static char	e[] = "BLASIX_PUT_EMPTY=";
	static char	q[] = "BLASIX_PUT_EQ=a=b";

	EXPECT_EQ(putenv(e), 0);
	EXPECT_EQ(putenv(q), 0);
	EXPECT_TRUE(ev_is("BLASIX_PUT_EMPTY", ""));
	EXPECT_TRUE(ev_is("BLASIX_PUT_EQ", "a=b"));
}

/* POSIX.1-2024: a valid name without '=': removed, or EINVAL and kept. */
TEST_OPT(name_alone_removes_or_fails, .since = "POSIX.1-2024", .ref = "XSH putenv() (Issue 8)")
{
	static char	s[] = "BLASIX_ENV_PLAIN";
	int			r;

	errno = 0;
	r = putenv(s);
	if (r == 0)
	{
		EXPECT_NULL(getenv("BLASIX_ENV_PLAIN"));
	}
	else
	{
		EXPECT_EQ(errno, EINVAL);
		EXPECT_TRUE(ev_is("BLASIX_ENV_PLAIN", "plain"));
	}
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	static char	s[] = "BLASIX_PUT_MACRO=m";

	EXPECT_EQ((putenv)(s), 0);
	EXPECT_TRUE(ev_is("BLASIX_PUT_MACRO", "m"));
}
