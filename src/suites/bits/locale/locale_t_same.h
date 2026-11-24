/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file locale_t_same.h
 * @brief locale_t of a header is the one of <locale.h>.
 * @Created: 2026/11/24 07:37:21 by Moutig
 * @Updated: 2026/11/24 08:32:02 by Moutig
 *
 * Included last by a defs.c (it includes <locale.h> when it exists, which
 * could make visible what the header under test must define itself).
 * LC_GLOBAL_LOCALE, a locale object descriptor of <locale.h>, is a value
 * of that locale_t, different from (locale_t)0 (uselocale() tells them
 * apart), and newlocale() uses the same type. The probe (spec:
 * same=locale.h) checks at compile time that the types are compatible.
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_locale_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_LOCALE_LOCALE_T_SAME_H
# define BITS_LOCALE_LOCALE_T_SAME_H

# include "framework/lc.h"

/* Includes other headers: after every other bits/ file of a defs.c. */
# define BITS_INCLUDES_OTHER_HEADERS	1

# if defined(LC_REQ_locale_t) || defined(LC_DEFS_ALL)

#  if defined(__has_include)
#   if __has_include(<locale.h>)
#    include <locale.h>
#   endif
#  endif

TEST_SYM(locale_t__same, "locale_t", same_as_locale_h, .ref = "XBD <locale.h>")
{
#  ifdef LC_GLOBAL_LOCALE
	locale_t	global = LC_GLOBAL_LOCALE;

	EXPECT_TYPE(&newlocale, locale_t (*)(int, const char *, locale_t));
	EXPECT_TRUE(global != (locale_t)0);
#  else
	SKIP("no LC_GLOBAL_LOCALE: <locale.h> missing or incomplete");
#  endif
}

# endif

#endif
