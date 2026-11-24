/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file string_cases.h
 * @brief Shared helpers and cases of the <string.h> units.
 * @Created: 2026/11/24 05:21:31 by Moutig
 * @Updated: 2026/11/24 05:35:48 by Moutig
 *
 * What every unit of src/suites/string/ needs around the function it
 * tests:
 *
 *   - ST_ERRNO_KEPT: POSIX.1-2024 adds to most functions of the header
 *     "shall not change the setting of errno on valid input" (XSH
 *     memchr() ... strxfrm(), new in Issue 8): one case per unit;
 *   - ST_POSIX_LOCALE: a "POSIX" locale object for the *_l functions,
 *     through newlocale() of <locale.h>, which is optional for the libc
 *     under test (__has_include and weak references, as in ctype);
 *   - bits/lib_ext1.h (BITS_LIB_EXT1_TESTS): an Annex K unit also checks
 *     __STDC_LIB_EXT1__;
 *   - ST_EXT1_IGNORE: runtime-constraint violations of Annex K must
 *     return instead of calling the default handler (which may abort),
 *     through set_constraint_handler_s() of <stdlib.h>;
 *   - st_sign, st_ascii: comparison sign, ASCII execution character set.
 *
 * Only <string.h> is under test. <errno.h>, <limits.h> (and <locale.h>,
 * <stdlib.h> where said) only prepare the data.
 */

#ifndef STRING_CASES_H
# define STRING_CASES_H

# include "framework/lc.h"
# include <string.h>
# include <errno.h>
# include <limits.h>

/** Sign of a comparison result: -1, 0 or 1. */
# define st_sign(x)		LC_SIGN(x)

/** Is the execution character set ASCII? (only checked on letters) */
# define st_ascii()		('A' == 0x41 && 'a' == 0x61 && '0' == 0x30)

/* ----- POSIX.1-2024: errno is left alone on valid input ----- */

/** errno before the call: an ISO C value no valid call would set. */
# define ST_ERRNO_MARK	EDOM

/*
 * One case: errno set to ST_ERRNO_MARK, the calls (statements, valid
 * input) are made, errno must still hold the mark.
 *
 *     ST_ERRNO_KEPT("POSIX memchr()", (void)memchr(s, 'a', 3);)
 */
# define ST_ERRNO_KEPT(REF, ...)											\
	TEST_OPT(errno_unchanged_on_valid_input, .since = "POSIX.1-2024",		\
		.ref = REF " (Issue 8)")											\
	{																		\
		errno = ST_ERRNO_MARK;												\
		__VA_ARGS__															\
		EXPECT_EQ(errno, ST_ERRNO_MARK);									\
	}

/* ----- Locale objects (POSIX.1-2008 *_l functions) ----- */

# if defined(__has_include)
#  if __has_include(<locale.h>)
#   include <locale.h>
#   define ST_HAVE_LOCALE_H	1
#  endif
# endif

# if defined(ST_HAVE_LOCALE_H) && defined(LC_ALL_MASK)
#  define ST_HAVE_NEWLOCALE	1
/* Weak: NULL when the libc does not define them. */
extern locale_t	newlocale(int mask, const char *locale, locale_t base) __attribute__((weak));
extern void		freelocale(locale_t locale) __attribute__((weak));

/**
 * @brief A new "POSIX" locale object, or (locale_t)0 when the libc has
 *        no newlocale() (the caller skips).
 */
static inline locale_t	st_posixLocale(void)
{
	if (!newlocale || !freelocale)
		return ((locale_t)0);
	return (newlocale(LC_ALL_MASK, "POSIX", (locale_t)0));
}

/*
 * Declare `loc`, a "POSIX" locale object, or skip the case. Released by
 * ST_FREE_LOCALE (the process ends after the case anyway).
 */
#  define ST_POSIX_LOCALE(loc)												\
	locale_t loc = st_posixLocale();										\
	if (loc == (locale_t)0)													\
		SKIP("no newlocale() / freelocale() or no \"POSIX\" locale")
#  define ST_FREE_LOCALE(loc)	freelocale(loc)
# else
#  define ST_POSIX_LOCALE(loc)												\
	locale_t loc = (locale_t)0;												\
	if (loc == (locale_t)0)													\
		SKIP("no <locale.h> with newlocale() in this libc")
#  define ST_FREE_LOCALE(loc)	((void)0)
# endif

/* ----- Annex K: runtime-constraint violations return ----- */

# if defined(__STDC_WANT_LIB_EXT1__)
#  include <stdlib.h>
#  include <stdint.h>
/* The default handler is implementation-defined (K.3.6.1.1p3). */
#  define ST_EXT1_IGNORE()	((void)set_constraint_handler_s(ignore_handler_s))
# endif

/* __STDC_LIB_EXT1__ checks of every Annex K unit: BITS_LIB_EXT1_TESTS. */
# include "bits/annex_k/lib_ext1.h"

#endif
