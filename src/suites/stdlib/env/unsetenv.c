/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file unsetenv.c
 * @brief Tests for unsetenv() (<stdlib.h>, POSIX).
 * @Created: 2026/11/27 13:29:13 by Moutig
 * @Updated: 2026/11/27 13:30:14 by Moutig
 *
 * unsetenv(name) removes the variable name from the environment; a name
 * that is not there leaves the environment unchanged and succeeds;
 * environ is updated. 0 on success; -1, errno set and the environment
 * unchanged on failure: EINVAL for a name that is empty or contains '='
 * (and a null pointer, POSIX.1-2001 to 2017).
 *
 * Clauses: POSIX: XSH unsetenv() (CX, POSIX.1-2001 on).
 */
#include "stdlib/env_cases.h"

TEST_OPT(prototype, .ref = "XSH unsetenv()")
{
	EXPECT_TYPE(&unsetenv, int (*)(const char *));
}

TEST_OPT(removes_the_variable, .ref = "XSH unsetenv()")
{
	int	n = ev_count();

	EXPECT_EQ(unsetenv("BLASIX_ENV_PLAIN"), 0);
	EXPECT_NULL(getenv("BLASIX_ENV_PLAIN"));
	EXPECT_NULL(ev_entry("BLASIX_ENV_PLAIN"));
	EXPECT_EQ(ev_count(), n - 1);
	EXPECT_TRUE(ev_othersIntact("BLASIX_ENV_PLAIN"));
}

/* The first and the last entries of environ, and an empty value. */
TEST_OPT(removes_the_first_and_the_last_entries, .ref = "XSH unsetenv()")
{
	EXPECT_EQ(unsetenv(LC_ENV_FIRST), 0);
	EXPECT_EQ(unsetenv(LC_ENV_LAST), 0);
	EXPECT_EQ(unsetenv("BLASIX_ENV_EMPTY"), 0);
	EXPECT_NULL(getenv(LC_ENV_FIRST));
	EXPECT_NULL(getenv(LC_ENV_LAST));
	EXPECT_NULL(getenv("BLASIX_ENV_EMPTY"));
	EXPECT_TRUE(ev_is("BLASIX_ENV_PLAIN", "plain"));
}

/* A name that is not there: success, nothing changes. */
TEST_OPT(absent_name_succeeds, .ref = "XSH unsetenv()")
{
	int	n = ev_count();

	EXPECT_EQ(unsetenv("BLASIX_ENV_ABSENT"), 0);
	EXPECT_EQ(unsetenv("BLASIX_ENV_PLAIN"), 0);
	EXPECT_EQ(unsetenv("BLASIX_ENV_PLAIN"), 0);
	EXPECT_EQ(ev_count(), n - 1);
}

/* Only that name: not a longer one, not one of another case. */
TEST_OPT(removes_only_that_name, .ref = "XSH unsetenv()")
{
	EXPECT_EQ(unsetenv("BLASIX_ENV_PREFIX"), 0);
	EXPECT_NULL(getenv("BLASIX_ENV_PREFIX"));
	EXPECT_TRUE(ev_is("BLASIX_ENV_PREFIXED", "long"));
	EXPECT_EQ(unsetenv("BLASIX_ENV_CASE"), 0);
	EXPECT_TRUE(ev_is("blasix_env_case", "lower"));
	EXPECT_EQ(unsetenv("BLASIX_ENV_PLAI"), 0);
	EXPECT_TRUE(ev_is("BLASIX_ENV_PLAIN", "plain"));
}

/* A variable added by setenv() is removed, and can be added again. */
TEST_OPT(removes_a_variable_set_by_setenv, .ref = "XSH unsetenv()")
{
	ASSERT_EQ(setenv("BLASIX_UNSET_NEW", "v1", 1), 0);
	EXPECT_EQ(unsetenv("BLASIX_UNSET_NEW"), 0);
	EXPECT_NULL(getenv("BLASIX_UNSET_NEW"));
	ASSERT_EQ(setenv("BLASIX_UNSET_NEW", "v2", 0), 0);
	EXPECT_TRUE(ev_is("BLASIX_UNSET_NEW", "v2"));
}

/* Every fixed variable removed one after the other. */
TEST_OPT(removes_every_variable_in_turn, .ref = "XSH unsetenv()")
{
	int	bad = 0;

#define EV_UNSET(n, v)	if (unsetenv(n) != 0 || getenv(n) != NULL) bad++;
	LC_ENV_VARS(EV_UNSET)
#undef EV_UNSET
	EXPECT_EQ(bad, 0);
	EXPECT_TRUE(ev_is(LC_ENV_FIRST, LC_ENV_FIRST_VAL));
	EXPECT_TRUE(ev_is(LC_ENV_LAST, LC_ENV_LAST_VAL));
}

/* An empty name, or one with '=': -1, EINVAL, nothing removed. */
TEST_OPT(invalid_name_fails_with_einval, .ref = "XSH unsetenv() ERRORS")
{
	static const char	*bad[] = {"", "=", "BLASIX_ENV_PLAIN=plain", "BLASIX_ENV_PLAIN="};
	int					n = ev_count();

	for (int i = 0; i < 4; i++)
	{
		errno = 0;
		EXPECT_EQ(unsetenv(bad[i]), -1);
		EXPECT_EQ(errno, EINVAL);
	}
	EXPECT_EQ(ev_count(), n);
	EXPECT_TRUE(ev_is("BLASIX_ENV_PLAIN", "plain"));
}

/* POSIX.1-2001 to 2017: a null name too (undefined from 2024: not tested). */
TEST_OPT(null_name_fails_with_einval, .since = "POSIX.1-2001", .until = "POSIX.1-2024",
	.ref = "XSH unsetenv() ERRORS")
{
	errno = 0;
	EXPECT_EQ(unsetenv(NULL), -1);
	EXPECT_EQ(errno, EINVAL);
}

TEST_OPT(macro_and_function_agree, .ref = "C11 7.1.4p1")
{
	EXPECT_EQ((unsetenv)("BLASIX_ENV_PLAIN"), 0);
	EXPECT_NULL(getenv("BLASIX_ENV_PLAIN"));
}
