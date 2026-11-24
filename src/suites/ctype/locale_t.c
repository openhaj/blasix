/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file locale_t.c
 * @brief Tests for the locale_t type of <ctype.h>.
 * @Created: 2026/11/24 03:41:05 by Moutig
 * @Updated: 2026/11/24 03:54:49 by Moutig
 *
 * XBD <ctype.h> (POSIX.1-2008 on): "The <ctype.h> header shall define the
 * locale_t type as described in <locale.h>", which defines it as the type
 * "representing a locale object". It is the second parameter of the 14
 * *_l functions of <ctype.h>.
 *
 * The probe (spec: locale_t type same=locale.h) already checks that
 * <ctype.h> alone declares it and that <locale.h> declares a compatible
 * type. The cases here check what can be observed of it from <ctype.h>,
 * and what <locale.h> says of its values when that header exists.
 * Its representation (pointer, integer) is not specified: not tested.
 */

#include "framework/lc.h"

#include <ctype.h>

#if defined(__has_include)
# if __has_include(<locale.h>)
#  include <locale.h>
#  define LT_HAVE_LOCALE_H	1
# endif
#endif

/* <ctype.h> alone names the type: an object of it can be declared. */
TEST_OPT(declared_by_ctype_h, .ref = "XBD <ctype.h>")
{
	locale_t	loc = (locale_t)0;

	EXPECT_TYPE(loc, locale_t);
}

/*
 * (locale_t)0 is the "no locale object" value of the interfaces that take
 * or return a locale_t (newlocale() base and failure, uselocale() query):
 * the type must accept a conversion from 0 and compare with it.
 */
TEST_OPT(null_object_converts_and_compares, .ref = "XSH newlocale()")
{
	locale_t	loc = (locale_t)0;

	EXPECT_TRUE(loc == (locale_t)0);
	EXPECT_TRUE(!loc);
}

/* It is the locale parameter of the *_l functions of <ctype.h>. */
TEST_OPT(is_the_parameter_of_the_l_functions, .ref = "XBD <ctype.h>")
{
	EXPECT_TYPE(&isalpha_l, int (*)(int, locale_t));
	EXPECT_TYPE(&tolower_l, int (*)(int, locale_t));
}

/*
 * The same type as in <locale.h>: LC_GLOBAL_LOCALE, a locale object
 * descriptor of <locale.h>, is a value of the <ctype.h> locale_t, and it
 * differs from (locale_t)0 (uselocale() tells them apart).
 */
TEST_OPT(same_as_locale_h, .ref = "XBD <locale.h>")
{
#if defined(LT_HAVE_LOCALE_H) && defined(LC_GLOBAL_LOCALE)
	locale_t	global = LC_GLOBAL_LOCALE;

	EXPECT_TYPE(&newlocale, locale_t (*)(int, const char *, locale_t));
	EXPECT_TRUE(global != (locale_t)0);
#else
	SKIP("no <locale.h> or no LC_GLOBAL_LOCALE in this libc");
#endif
}
