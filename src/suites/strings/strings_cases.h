/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file strings_cases.h
 * @brief Shared helpers and cases of the <strings.h> units.
 * @Created: 2026/11/24 11:09:48 by Moutig
 * @Updated: 2026/11/24 11:10:29 by Moutig
 *
 * @Created: 2026/11/24 05:24:04 by Moutig
 * @Updated: 2026/11/24 11:10:29 by Moutig
 *
 * What the units of src/suites/strings/ need around the function they
 * test:
 *
 *   - ss_lower, ss_ascii: the oracle of the case-insensitive comparisons.
 *     In the POSIX locale they "behave as if the strings had been
 *     converted to lowercase and then a byte comparison performed": only
 *     A ... Z have a lowercase there (XBD 7.3.1.1), every other byte is
 *     itself. Built on ASCII codes, so the cases using it skip otherwise;
 *   - SS_ERRNO_KEPT: POSIX.1-2024 adds "errno shall not be changed on
 *     valid input" to strcasecmp() and friends;
 *   - SS_POSIX_LOCALE: a "POSIX" locale object for the *_l functions,
 *     through newlocale() of <locale.h>, optional for the libc under test
 *     (__has_include and weak references, as in ctype and string).
 *
 * Only <strings.h> is under test; <errno.h>, <limits.h> and <locale.h>
 * only prepare the data.
 */

#ifndef STRINGS_CASES_H
# define STRINGS_CASES_H

# include "framework/lc.h"
# include <strings.h>
# include <errno.h>
# include <limits.h>

/** Sign of a comparison result: -1, 0 or 1. */
# define ss_sign(x)		LC_SIGN(x)

/** Is the execution character set ASCII? (only checked on letters) */
# define ss_ascii()		('A' == 0x41 && 'Z' == 0x5a && 'a' == 0x61)

/**
 * @brief Lowercase of byte @p c in the POSIX locale (ASCII codes).
 *
 * @param c Byte value, 0 ... UCHAR_MAX.
 * @return c + 0x20 for 'A' ... 'Z', c otherwise.
 */
static inline int	ss_lower(int c)
{
	return ((c >= 0x41 && c <= 0x5a) ? c + 0x20 : c);
}

/* ----- POSIX.1-2024: errno is left alone on valid input ----- */

/** errno before the call: an ISO C value no valid call would set. */
# define SS_ERRNO_MARK	EDOM

# define SS_ERRNO_KEPT(REF, ...)											\
	TEST_OPT(errno_unchanged_on_valid_input, .since = "POSIX.1-2024",		\
		.ref = REF " (Issue 8)")											\
	{																		\
		errno = SS_ERRNO_MARK;												\
		__VA_ARGS__															\
		EXPECT_EQ(errno, SS_ERRNO_MARK);									\
	}

/* ----- Locale objects (POSIX.1-2008 *_l functions) ----- */

# if defined(__has_include)
#  if __has_include(<locale.h>)
#   include <locale.h>
#   define SS_HAVE_LOCALE_H	1
#  endif
# endif

# if defined(SS_HAVE_LOCALE_H) && defined(LC_ALL_MASK)
/* Weak: NULL when the libc does not define them. */
extern locale_t	newlocale(int mask, const char *locale, locale_t base) __attribute__((weak));
extern void		freelocale(locale_t locale) __attribute__((weak));

/**
 * @brief A new "POSIX" locale object, or (locale_t)0 when the libc has
 *        no newlocale() (the caller skips).
 */
static inline locale_t	ss_posixLocale(void)
{
	if (!newlocale || !freelocale)
		return ((locale_t)0);
	return (newlocale(LC_ALL_MASK, "POSIX", (locale_t)0));
}

/* Declare `loc`, a "POSIX" locale object, or skip the case. */
#  define SS_POSIX_LOCALE(loc)												\
	locale_t loc = ss_posixLocale();										\
	if (loc == (locale_t)0)													\
		SKIP("no newlocale() / freelocale() or no \"POSIX\" locale")
#  define SS_FREE_LOCALE(loc)	freelocale(loc)
# else
#  define SS_POSIX_LOCALE(loc)												\
	locale_t loc = (locale_t)0;												\
	if (loc == (locale_t)0)													\
		SKIP("no <locale.h> with newlocale() in this libc")
#  define SS_FREE_LOCALE(loc)	((void)0)
# endif

#endif
