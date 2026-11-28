/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file setenv.c
 * @brief Tests for setenv() (<stdlib.h>, POSIX).
 * @Created: 2026/11/27 13:29:05 by Moutig
 * @Updated: 2026/11/27 13:30:06 by Moutig
 *
 * setenv(envname, envval, overwrite) adds the variable envname with the
 * value envval, or, when it exists, sets it if overwrite is non-zero and
 * leaves it if overwrite is zero (success both times); the strings are
 * copied; environ is updated. 0 on success; -1, errno set and the
 * environment unchanged on failure: EINVAL for a name that is empty or
 * contains '=' (and a null pointer, POSIX.1-2001 to 2017), ENOMEM.
 *
 * Clauses: POSIX: XSH setenv() (CX, POSIX.1-2001 on); XBD 8.1.
 */
#include "stdlib/env_cases.h"

TEST_OPT(prototype, .ref = "XSH setenv()")
{
	EXPECT_TYPE(&setenv, int (*)(const char *, const char *, int));
}

TEST_OPT(adds_a_new_variable, .ref = "XSH setenv()")
{
	EXPECT_NULL(getenv("BLASIX_SET_NEW"));
	EXPECT_EQ(setenv("BLASIX_SET_NEW", "fresh", 0), 0);
	EXPECT_TRUE(ev_is("BLASIX_SET_NEW", "fresh"));
	EXPECT_EQ(setenv("BLASIX_SET_OTHER", "two", 1), 0);
	EXPECT_TRUE(ev_is("BLASIX_SET_OTHER", "two"));
	EXPECT_TRUE(ev_othersIntact(""));
}

/* An existing variable with overwrite non-zero (1, -1, 42): updated. */
TEST_OPT(overwrite_replaces_the_value, .ref = "XSH setenv()")
{
	EXPECT_EQ(setenv("BLASIX_ENV_PLAIN", "new", 1), 0);
	EXPECT_TRUE(ev_is("BLASIX_ENV_PLAIN", "new"));
	EXPECT_EQ(setenv("BLASIX_ENV_PLAIN", "newer", -1), 0);
	EXPECT_TRUE(ev_is("BLASIX_ENV_PLAIN", "newer"));
	EXPECT_EQ(setenv("BLASIX_ENV_FIRST", "changed", 42), 0);
	EXPECT_TRUE(ev_is("BLASIX_ENV_FIRST", "changed"));
	EXPECT_TRUE(ev_is(LC_ENV_LAST, LC_ENV_LAST_VAL) && ev_is("BLASIX_ENV_PREFIX", "short"));
}

/* An existing variable with overwrite zero: success, unchanged. */
TEST_OPT(no_overwrite_keeps_the_value, .ref = "XSH setenv()")
{
	EXPECT_EQ(setenv("BLASIX_ENV_PLAIN", "ignored", 0), 0);
	EXPECT_TRUE(ev_is("BLASIX_ENV_PLAIN", "plain"));
	EXPECT_EQ(setenv("BLASIX_ENV_EMPTY", "ignored", 0), 0);
	EXPECT_TRUE(ev_is("BLASIX_ENV_EMPTY", ""));
	EXPECT_TRUE(ev_othersIntact(""));
}

/* The strings are copied: changing them afterwards changes nothing. */
TEST_OPT(strings_are_copied, .ref = "XSH setenv()")
{
	char	name[] = "BLASIX_SET_COPY";
	char	value[] = "original";

	EXPECT_EQ(setenv(name, value, 1), 0);
	value[0] = 'X';
	name[0] = 'X';
	EXPECT_TRUE(ev_is("BLASIX_SET_COPY", "original"));
	EXPECT_NULL(getenv("XLASIX_SET_COPY"));
}

/* Any value: empty, with '=', with spaces, high bytes, 100000 bytes. */
TEST_OPT(any_value, .ref = "XSH setenv()")
{
	static char	big[100001];

	EXPECT_EQ(setenv("BLASIX_SET_EMPTY", "", 1), 0);
	EXPECT_TRUE(ev_is("BLASIX_SET_EMPTY", ""));
	EXPECT_EQ(setenv("BLASIX_SET_EQ", "a=b=", 1), 0);
	EXPECT_TRUE(ev_is("BLASIX_SET_EQ", "a=b="));
	EXPECT_EQ(setenv("BLASIX_SET_HIGH", " \xff\x80 x ", 1), 0);
	EXPECT_TRUE(ev_is("BLASIX_SET_HIGH", " \xff\x80 x "));
	lc_fill(big, 'b', 100000);
	big[100000] = '\0';
	EXPECT_EQ(setenv("BLASIX_SET_BIG", big, 1), 0);
	EXPECT_TRUE(ev_is("BLASIX_SET_BIG", big));
}

/* environ is updated: it has the entry "name=value". */
TEST_OPT(updates_environ, .ref = "XSH setenv()")
{
	char	*e;

	EXPECT_EQ(setenv("BLASIX_SET_ENVIRON", "seen", 1), 0);
	e = ev_entry("BLASIX_SET_ENVIRON");
	ASSERT_NOTNULL(e);
	EXPECT_TRUE(ev_streq(e, "BLASIX_SET_ENVIRON=seen"));
	EXPECT_EQ(setenv("BLASIX_ENV_PLAIN", "replaced", 1), 0);
	e = ev_entry("BLASIX_ENV_PLAIN");
	ASSERT_NOTNULL(e);
	EXPECT_TRUE(ev_streq(e, "BLASIX_ENV_PLAIN=replaced"));
}

/*
 * An empty name, or one with '=': -1, EINVAL, the environment unchanged
 * (no variable added, the one named before the '=' kept).
 */
TEST_OPT(invalid_name_fails_with_einval, .ref = "XSH setenv() ERRORS")
{
	static const char	*bad[] = {"", "=", "A=B", "BLASIX_ENV_PLAIN=x", "BLASIX_SET="};
	int					n = ev_count();

	for (int i = 0; i < 5; i++)
	{
		errno = 0;
		EXPECT_EQ(setenv(bad[i], "value", 1), -1);
		EXPECT_EQ(errno, EINVAL);
	}
	EXPECT_EQ(ev_count(), n);
	EXPECT_TRUE(ev_is("BLASIX_ENV_PLAIN", "plain"));
	EXPECT_NULL(getenv("BLASIX_SET"));
	EXPECT_NULL(getenv("A"));
}

/* POSIX.1-2001 to 2017: a null name too (undefined from 2024: not tested). */
TEST_OPT(null_name_fails_with_einval, .since = "POSIX.1-2001", .until = "POSIX.1-2024",
	.ref = "XSH setenv() ERRORS")
{
	errno = 0;
	EXPECT_EQ(setenv(NULL, "value", 1), -1);
	EXPECT_EQ(errno, EINVAL);
}

/* 500 new variables, then each one changed: every value as last set. */
TEST_OPT(many_variables, .ref = "XSH setenv()")
{
	char	name[32] = "BLASIX_MANY_";
	char	value[32];
	int		bad = -1;

	for (int round = 0; round < 2; round++)
		for (int i = 0; i < 500; i++)
		{
			name[12] = (char)('A' + i / 26 % 26);
			name[13] = (char)('A' + i % 26);
			name[14] = (char)('0' + i / 676);
			name[15] = '\0';
			value[0] = (char)('a' + round);
			value[1] = name[13];
			value[2] = '\0';
			if (setenv(name, value, 1) != 0)
				bad = i;
		}
	for (int i = 0; i < 500 && bad < 0; i++)
	{
		name[12] = (char)('A' + i / 26 % 26);
		name[13] = (char)('A' + i % 26);
		name[14] = (char)('0' + i / 676);
		value[0] = 'b';
		value[1] = name[13];
		if (!ev_is(name, value))
			bad = 1000 + i;
	}
	EXPECT_EQ(bad, -1);
	EXPECT_TRUE(ev_othersIntact(""));
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ((setenv)("BLASIX_SET_MACRO", "m", 1), 0);
	EXPECT_TRUE(ev_is("BLASIX_SET_MACRO", "m"));
}
