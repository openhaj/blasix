/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file __STDC_VERSION_STRING_H__.h
 * @brief Cases of the __STDC_VERSION_STRING_H__ macro of <string.h>.
 * @Created: 2026/11/24 07:37:21 by Moutig
 * @Updated: 2026/11/24 07:37:21 by Moutig
 *
 * C23 7.26.1p2: <string.h> defines __STDC_VERSION_STRING_H__, an integer constant expression
 * with a value equivalent to 202311L.
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ___STDC_VERSION_STRING_H__, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_VERSION___STDC_VERSION_STRING_H___H
# define BITS_VERSION___STDC_VERSION_STRING_H___H

# include "framework/lc.h"

# if defined(LC_REQ___STDC_VERSION_STRING_H__) || defined(LC_DEFS_ALL)

TEST_SYM(__STDC_VERSION_STRING_H____defined, "__STDC_VERSION_STRING_H__", is_defined, .ref = "C23 7.26.1p2")
{
#  ifdef __STDC_VERSION_STRING_H__
	EXPECT_TRUE(1);
#  else
	EXPECT_TRUE(0);
#  endif
}

TEST_SYM(__STDC_VERSION_STRING_H____value, "__STDC_VERSION_STRING_H__", value_is_202311, .ref = "C23 7.26.1p2")
{
#  ifdef __STDC_VERSION_STRING_H__
	EXPECT_TRUE(__STDC_VERSION_STRING_H__ == 202311L);
#  else
	SKIP("not defined (see is_defined)");
#  endif
}

/* An integer constant expression: usable in #if and in an array size. */
TEST_SYM(__STDC_VERSION_STRING_H____ice, "__STDC_VERSION_STRING_H__", integer_constant_expression, .ref = "C23 7.26.1p2")
{
#  if defined(__STDC_VERSION_STRING_H__) && __STDC_VERSION_STRING_H__ >= 202311L
	static const char	ok[(__STDC_VERSION_STRING_H__ == 202311L) ? 1 : 2];

	EXPECT_UEQ(sizeof(ok), 1);
#  else
	SKIP("not defined (see is_defined)");
#  endif
}

# endif

#endif
