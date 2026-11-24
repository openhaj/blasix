/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file wchar_t.h
 * @brief Cases of wchar_t, for every header that defines it.
 * @Created: 2026/11/24 07:37:21 by Moutig
 * @Updated: 2026/11/24 07:37:21 by Moutig
 *
 * wchar_t is an integer type whose range can represent distinct codes for
 * all members of the largest extended character set; the null character
 * has the code 0 and each member of the basic character set the value it
 * has as the lone character of an integer character constant, unless
 * __STDC_MB_MIGHT_NEQ_WC__ (C99 7.17, C11 / C17 7.19, C23 7.21):
 * <stddef.h>, <stdlib.h>, <wchar.h>; POSIX.1-2017 on, <inttypes.h>.
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_wchar_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDDEF_WCHAR_T_H
# define BITS_STDDEF_WCHAR_T_H

# include "framework/lc.h"

# if defined(LC_REQ_wchar_t) || defined(LC_DEFS_ALL)

# ifdef __STDC_MB_MIGHT_NEQ_WC__
#  define BITS_WC_EQ_MB	0
# else
#  define BITS_WC_EQ_MB	1
# endif

TEST_SYM(wchar_t__declared, "wchar_t", declared_by_the_header, .ref = "C11 7.19p2")
{
	wchar_t	c = L'a';

	EXPECT_TYPE(c, wchar_t);
}

TEST_SYM(wchar_t__integer, "wchar_t", is_an_integer_type, .ref = "C11 7.19p2")
{
	wchar_t	c = (wchar_t)65;

	EXPECT_TRUE(c + 1 == 66);
	EXPECT_TRUE((wchar_t)7 / 2 == 3);
}

TEST_SYM(wchar_t__nul, "wchar_t", null_character_is_zero, .ref = "C11 7.19p2")
{
	EXPECT_TRUE(L'\0' == 0);
	EXPECT_TRUE(L""[0] == 0);
}

/* Wide string literals have elements of type wchar_t. */
TEST_SYM(wchar_t__literals, "wchar_t", is_the_type_of_wide_literals, .ref = "C11 6.4.5p6")
{
	EXPECT_TYPE(L"ab"[0], wchar_t);
	EXPECT_UEQ(sizeof(L"ab") / sizeof(wchar_t), 3);
}

TEST_SYM(wchar_t__basic, "wchar_t", basic_characters_keep_their_codes, .ref = "C11 7.19p2")
{
	static const char		n[] = "AZaz09 +-*/\n";
	static const wchar_t	w[] = L"AZaz09 +-*/\n";
	int						bad = -1;

	if (!BITS_WC_EQ_MB)
		SKIP("__STDC_MB_MIGHT_NEQ_WC__ is defined");
	for (int i = 0; n[i] && bad < 0; i++)
		if (w[i] != (wchar_t)n[i])
			bad = i;
	EXPECT_EQ(bad, -1);
}

# endif

#endif
