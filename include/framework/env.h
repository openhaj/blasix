/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file env.h
 * @brief The environment variables lcrun gives every test.
 * @Created: 2026/11/27 13:22:53 by Moutig
 * @Updated: 2026/11/27 13:22:59 by Moutig
 *
 * A test inherits the environment of lcrun, which nobody controls. So
 * that getenv() and its family can be tested on known data, lcrun takes
 * out every inherited BLASIX_ENV_* variable and adds these, LC_ENV_FIRST
 * first and LC_ENV_LAST last (the other variables stay in between, in
 * their order). Shared by lcrun (which builds them) and the units (which
 * expect them): no libc header here.
 *
 * LC_ENV_VARS(X) calls X(name, value) for each fixed variable; the long
 * one, LC_ENV_LONG_NAME, is LC_ENV_LONG_LEN times LC_ENV_LONG_CHAR, built
 * at run time.
 */

#ifndef LC_ENV_H
# define LC_ENV_H

/** Prefix of every variable lcrun sets (and removes when inherited). */
# define LC_ENV_PREFIX		"BLASIX_ENV_"

/** The first and the last entries of the environment. */
# define LC_ENV_FIRST		"BLASIX_ENV_FIRST"
# define LC_ENV_FIRST_VAL	"first"
# define LC_ENV_LAST		"BLASIX_ENV_LAST"
# define LC_ENV_LAST_VAL	"last"

/** A value of LC_ENV_LONG_LEN bytes LC_ENV_LONG_CHAR. */
# define LC_ENV_LONG_NAME	"BLASIX_ENV_LONG"
# define LC_ENV_LONG_LEN	4000
# define LC_ENV_LONG_CHAR	'L'

/*
 * The other fixed variables: a plain value, an empty one, spaces kept,
 * '=' inside the value, high bytes, a name that is the prefix of another,
 * and two names that differ only by case.
 */
# define LC_ENV_VARS(X)													\
	X("BLASIX_ENV_PLAIN", "plain")										\
	X("BLASIX_ENV_EMPTY", "")											\
	X("BLASIX_ENV_SPACES", "  two  spaces ")							\
	X("BLASIX_ENV_EQUALS", "x=y==z")									\
	X("BLASIX_ENV_HIGH", "\xc3\xa9\xff\x7f")							\
	X("BLASIX_ENV_PREFIX", "short")									\
	X("BLASIX_ENV_PREFIXED", "long")									\
	X("BLASIX_ENV_CASE", "upper")										\
	X("blasix_env_case", "lower")

#endif
