/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file getenv.c
 * @brief Tests for getenv() (<stdlib.h>).
 * @Created: 2026/11/27 13:21:47 by Moutig
 * @Updated: 2026/11/27 13:23:49 by Moutig
 *
 * getenv(name) searches the environment list for a string that matches
 * name and returns a pointer to the string associated with it (its
 * value), or a null pointer when the name cannot be found (7.24.4.6).
 * POSIX: the environment is the array environ of "name=value" strings;
 * names keep the case of their letters (XBD 8.1) and contain no '='
 * (what getenv() does with such a name is not said: not tested). From
 * POSIX.1-2024 (CX), the pointer points into the string of environ.
 *
 * The data: the variables lcrun gives every test (framework/env.h), and
 * the initial environment of the process read from /proc/self/environ
 * (without the libc).
 *
 * Clauses: C99 7.20.4.5; C11, C17 7.22.4.6; C23 7.24.4.6. POSIX: XSH
 * getenv(); XBD 8.1.
 */
#include "stdlib/stdlib_cases.h"
#include "framework/env.h"
#include "framework/sys.h"

TEST_OPT(prototype, .ref = "C11 7.22.4.6p1")
{
	EXPECT_TYPE(&getenv, char *(*)(const char *));
}

/* Are the strings equal (without the libc)? */
static int	geStreq(const char *a, const char *b)
{
	while (*a && *a == *b)
	{
		a++;
		b++;
	}
	return (*a == *b);
}

/* Are the first n bytes of a and b equal? */
static int	geSame(const char *a, const char *b, size_t n)
{
	for (size_t i = 0; i < n; i++)
		if (a[i] != b[i])
			return (0);
	return (1);
}

/* Is getenv(name) a string equal to value? */
static int	hasValue(const char *name, const char *value)
{
	const char	*v = getenv(name);

	return (v != NULL && geStreq(v, value));
}

/* Every fixed variable of lcrun: its value, byte for byte. */
TEST_OPT(finds_each_variable, .ref = "C11 7.22.4.6p4")
{
	int	bad = 0;

#define CHECK_VAR(name, value)	if (!hasValue(name, value)) { LOG(name); bad++; }
	LC_ENV_VARS(CHECK_VAR)
#undef CHECK_VAR
	EXPECT_EQ(bad, 0);
}

TEST_OPT(empty_value_is_an_empty_string, .ref = "C11 7.22.4.6p4")
{
	char	*v = getenv("BLASIX_ENV_EMPTY");

	ASSERT_NOTNULL(v);
	EXPECT_EQ(v[0], '\0');
}

/* The value is everything after the first '=': spaces and '=' kept. */
TEST_OPT(value_is_everything_after_the_first_equals, .ref = "XBD 8.1")
{
	EXPECT_TRUE(hasValue("BLASIX_ENV_EQUALS", "x=y==z"));
	EXPECT_TRUE(hasValue("BLASIX_ENV_SPACES", "  two  spaces "));
	EXPECT_TRUE(hasValue("BLASIX_ENV_HIGH", "\xc3\xa9\xff\x7f"));
}

TEST_OPT(long_value, .ref = "C11 7.22.4.6p4")
{
	const char	*v = getenv(LC_ENV_LONG_NAME);
	size_t		n = 0;

	ASSERT_NOTNULL(v);
	while (v[n] == LC_ENV_LONG_CHAR)
		n++;
	EXPECT_UEQ(n, LC_ENV_LONG_LEN);
	EXPECT_EQ(v[n], '\0');
}

/* The first and the last entries of the environment are found too. */
TEST_OPT(first_and_last_entries, .ref = "C11 7.22.4.6p2")
{
	EXPECT_TRUE(hasValue(LC_ENV_FIRST, LC_ENV_FIRST_VAL));
	EXPECT_TRUE(hasValue(LC_ENV_LAST, LC_ENV_LAST_VAL));
}

TEST_OPT(absent_name_gives_null, .ref = "C11 7.22.4.6p4")
{
	EXPECT_NULL(getenv("BLASIX_ENV_ABSENT"));
	EXPECT_NULL(getenv("BLASIX_ENV_"));
	EXPECT_NULL(getenv(""));
}

/* A whole name matches: neither a prefix of it nor a longer name. */
TEST_OPT(matches_the_whole_name, .ref = "C11 7.22.4.6p2")
{
	EXPECT_TRUE(hasValue("BLASIX_ENV_PREFIX", "short"));
	EXPECT_TRUE(hasValue("BLASIX_ENV_PREFIXED", "long"));
	EXPECT_NULL(getenv("BLASIX_ENV_PREFI"));
	EXPECT_NULL(getenv("BLASIX_ENV_PREFIXE"));
	EXPECT_NULL(getenv("BLASIX_ENV_PLAINX"));
	EXPECT_NULL(getenv("BLASIX_ENV_PLAI"));
}

/* POSIX: upper and lower case letters are not folded together. */
TEST_OPT(names_are_case_sensitive, .since = "POSIX.1-2001", .ref = "XBD 8.1")
{
	EXPECT_TRUE(hasValue("BLASIX_ENV_CASE", "upper"));
	EXPECT_TRUE(hasValue("blasix_env_case", "lower"));
	EXPECT_NULL(getenv("Blasix_Env_Case"));
	EXPECT_NULL(getenv("blasix_env_plain"));
}

/*
 * Every variable of the initial environment (/proc/self/environ, read
 * raw) whose name appears once: getenv() gives its value.
 */
TEST_OPT(agrees_with_the_initial_environment, .ref = "C11 7.22.4.6p2-4")
{
	static char	buf[1 << 17];
	long		fd = lc_sysOpen("/proc/self/environ", LC_O_RDONLY | LC_O_CLOEXEC, 0);
	long		len = 0;
	long		r;
	int			checked = 0;
	int			bad = 0;

	if (fd < 0)
		SKIP("no /proc/self/environ");
	while (len < (long)sizeof(buf) - 1
		&& (r = lc_sysRead((int)fd, buf + len, sizeof(buf) - 1 - (unsigned long)len)) > 0)
		len += r;
	(void)lc_sysClose((int)fd);
	buf[len] = '\0';
	for (long i = 0; i < len; i += (long)lc_strlen(buf + i) + 1)
	{
		char	*e = buf + i;
		char	*eq = e;
		int		twice = 0;

		while (*eq && *eq != '=')
			eq++;
		if (*eq != '=' || eq == e)
			continue ;
		*eq = '\0';
		for (long j = 0; j < len && !twice; j += (long)lc_strlen(buf + j) + 1)
			if (j != i && geSame(buf + j, e, (size_t)(eq - e))
				&& buf[j + (eq - e)] == '=')
				twice = 1;
		if (!twice)
		{
			checked++;
			if (!hasValue(e, eq + 1))
			{
				LOG(e);
				bad++;
			}
		}
		*eq = '=';
	}
	EXPECT_GT(checked, 10);
	EXPECT_EQ(bad, 0);
}

/* Calls in a row: each gives the value of its own name. */
TEST_OPT(successive_calls, .ref = "C11 7.22.4.6p4")
{
	for (int i = 0; i < 100; i++)
	{
		EXPECT_TRUE(hasValue(i % 2 ? "BLASIX_ENV_PLAIN" : "BLASIX_ENV_PREFIX",
			i % 2 ? "plain" : "short"));
	}
}

extern char	**environ;

/* POSIX.1-2024 (CX): the pointer is inside the string of environ itself. */
TEST_OPT(points_into_environ, .since = "POSIX.1-2024", .ref = "XSH getenv() (Issue 8)")
{
	const char	*v = getenv("BLASIX_ENV_PLAIN");
	int			inside = 0;

	ASSERT_NOTNULL(v);
	for (char **e = environ; *e && !inside; e++)
		if (geSame(*e, "BLASIX_ENV_PLAIN=", 17) && v == *e + 17)
			inside = 1;
	EXPECT_TRUE(inside);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_PTREQ(getenv("BLASIX_ENV_PLAIN"), (getenv)("BLASIX_ENV_PLAIN"));
	EXPECT_NULL((getenv)("BLASIX_ENV_ABSENT"));
}
