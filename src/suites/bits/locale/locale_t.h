/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file locale_t.h
 * @brief Cases of locale_t, for every header that defines it.
 * @Created: 2026/11/24 07:37:35 by Moutig
 * @Updated: 2026/11/24 08:32:18 by Moutig
 *
 * POSIX.1-2008 on: <locale.h> defines locale_t, "representing a locale
 * object"; <ctype.h>, <string.h>, <strings.h>, <wctype.h>, <time.h>, ...
 * "shall define the locale_t type as described in <locale.h>". Its
 * representation is not specified (not tested). That it is the type of
 * <locale.h> is checked by locale/locale_t_same.h, included last.
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_locale_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_LOCALE_LOCALE_T_H
# define BITS_LOCALE_LOCALE_T_H

# include "framework/lc.h"

# if defined(LC_REQ_locale_t) || defined(LC_DEFS_ALL)

TEST_SYM(locale_t__declared, "locale_t", declared_by_the_header, .ref = "XBD <locale.h>")
{
	locale_t	loc = (locale_t)0;

	EXPECT_TYPE(loc, locale_t);
}

/* (locale_t)0 is the "no locale object" of newlocale(), uselocale(). */
TEST_SYM(locale_t__null, "locale_t", null_object_converts_and_compares, .ref = "XSH newlocale()")
{
	locale_t	loc = (locale_t)0;

	EXPECT_TRUE(loc == (locale_t)0);
	EXPECT_TRUE(!loc);
}

/* Passed and returned by value: a complete object type. */
TEST_SYM(locale_t__complete, "locale_t", is_a_complete_object_type, .ref = "XBD <locale.h>")
{
	locale_t	a = (locale_t)0;
	locale_t	b;

	b = a;
	EXPECT_GT(sizeof(locale_t), 0);
	EXPECT_TRUE(b == a);
}

# endif

#endif
