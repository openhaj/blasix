/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file env_cases.h
 * @brief Helpers of the environment units (getenv, setenv, unsetenv, putenv).
 * @Created: 2026/11/27 13:29:22 by Moutig
 * @Updated: 2026/11/27 13:29:39 by Moutig
 *
 * Every test starts with the environment lcrun gives it
 * (framework/env.h) and runs in its own process, so a test may change
 * it freely. These helpers read the environment without the functions
 * under test:
 *
 *   - ev_streq, ev_same: string comparisons without the libc;
 *   - ev_is: getenv(name) is the string value;
 *   - ev_entry: the entry "name=..." of environ (POSIX), or NULL;
 *   - ev_count: the number of entries of environ.
 *
 * environ is declared here (XBD 8.1: "extern char **environ;"): the
 * application may declare it itself.
 */

#ifndef ENV_CASES_H
# define ENV_CASES_H

# include "stdlib/stdlib_cases.h"
# include "framework/env.h"

extern char	**environ;

/* Are the strings equal? */
static inline int	ev_streq(const char *a, const char *b)
{
	while (*a && *a == *b)
	{
		a++;
		b++;
	}
	return (*a == *b);
}

/* Are the first n bytes of a and b equal? */
static inline int	ev_same(const char *a, const char *b, size_t n)
{
	for (size_t i = 0; i < n; i++)
		if (a[i] != b[i])
			return (0);
	return (1);
}

/* Is getenv(name) the string value? */
static inline int	ev_is(const char *name, const char *value)
{
	const char	*v = getenv(name);

	return (v != NULL && ev_streq(v, value));
}

/* The entry of environ for name ("name=..."), or NULL. */
static inline char	*ev_entry(const char *name)
{
	size_t	n = lc_strlen(name);

	for (char **e = environ; e && *e; e++)
		if (ev_same(*e, name, n) && (*e)[n] == '=')
			return (*e);
	return (NULL);
}

/* The number of entries of environ. */
static inline int	ev_count(void)
{
	int	n = 0;

	for (char **e = environ; e && *e; e++)
		n++;
	return (n);
}

/* The fixed variables of lcrun other than name keep their values. */
static inline int	ev_othersIntact(const char *name)
{
	int	ok = 1;

#  define EV_CHECK(n, v)	if (!ev_streq(n, name) && !ev_is(n, v)) ok = 0;
	LC_ENV_VARS(EV_CHECK)
#  undef EV_CHECK
	if (!ev_streq(LC_ENV_FIRST, name) && !ev_is(LC_ENV_FIRST, LC_ENV_FIRST_VAL))
		ok = 0;
	if (!ev_streq(LC_ENV_LAST, name) && !ev_is(LC_ENV_LAST, LC_ENV_LAST_VAL))
		ok = 0;
	return (ok);
}

#endif
