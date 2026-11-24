/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file NULL.h
 * @brief Cases of NULL, for every header that defines it.
 * @Created: 2026/11/24 12:10:42 by Moutig
 * @Updated: 2026/11/24 12:10:48 by Moutig
 *
 * NULL "expands to an implementation-defined null pointer constant" (C99
 * 7.17, C11 / C17 7.19, C23 7.21): <stddef.h>, <string.h>, <stdio.h>,
 * <stdlib.h>, <time.h>, <wchar.h>, <locale.h>...
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_NULL, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDDEF_NULL_H
# define BITS_STDDEF_NULL_H

# include "framework/lc.h"

# if defined(LC_REQ_NULL) || defined(LC_DEFS_ALL)

# ifdef NULL
#  define BITS_NULL_DEFINED	1
# else
#  define BITS_NULL_DEFINED	0
# endif

/* A null pointer constant: valid in a static initializer. */
static char	*bits_null_static = NULL;
static int	(*bits_null_fn)(void) = NULL;

TEST_SYM(NULL__macro, "NULL", is_a_macro, .ref = "C11 7.19p3")
{
	EXPECT_TRUE(BITS_NULL_DEFINED);
}

TEST_SYM(NULL__object, "NULL", converts_to_a_null_object_pointer, .ref = "C11 6.3.2.3p3")
{
	char	*p = NULL;
	void	*v = NULL;

	EXPECT_NULL(p);
	EXPECT_NULL(v);
	EXPECT_NULL(bits_null_static);
}

TEST_SYM(NULL__function, "NULL", converts_to_a_null_function_pointer, .ref = "C11 6.3.2.3p3")
{
	int		(*f)(void) = NULL;

	EXPECT_TRUE(f == 0);
	EXPECT_TRUE(bits_null_fn == 0);
}

TEST_SYM(NULL__zero, "NULL", compares_equal_to_zero, .ref = "C11 6.3.2.3p4")
{
	EXPECT_TRUE(NULL == 0);
	EXPECT_TRUE(!NULL);
}

# endif

#endif
