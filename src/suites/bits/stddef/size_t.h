/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file size_t.h
 * @brief Cases of size_t, for every header that defines it.
 * @Created: 2026/11/24 12:10:29 by Moutig
 * @Updated: 2026/11/24 12:10:36 by Moutig
 *
 * size_t is "the unsigned integer type of the result of the sizeof
 * operator" (C99 7.17, C11 / C17 7.19, C23 7.21): <stddef.h>, <string.h>,
 * <strings.h>, <stdio.h>, <stdlib.h>, <time.h>, <wchar.h>, <sys/types.h>...
 * Only size_t itself is used: nothing else may make it visible.
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_size_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDDEF_SIZE_T_H
# define BITS_STDDEF_SIZE_T_H

# include "framework/lc.h"

# if defined(LC_REQ_size_t) || defined(LC_DEFS_ALL)

TEST_SYM(size_t__declared, "size_t", declared_by_the_header, .ref = "C11 7.19p2")
{
	size_t	n = 0;

	EXPECT_TYPE(n, size_t);
}

TEST_SYM(size_t__sizeof, "size_t", is_the_type_of_sizeof, .ref = "C11 7.19p2")
{
	EXPECT_TYPE(sizeof(int), size_t);
	EXPECT_TYPE(sizeof(char[3]), size_t);
}

TEST_SYM(size_t__unsigned, "size_t", is_unsigned, .ref = "C11 7.19p2")
{
	EXPECT_TRUE((size_t)-1 > 0);
	EXPECT_TRUE((size_t)0 - 1 == (size_t)-1);
}

/* SIZE_MAX is at least 65535 (C11 7.20.3p2). */
TEST_SYM(size_t__16_bits, "size_t", at_least_16_bits, .ref = "C11 7.20.3p2")
{
	EXPECT_TRUE((size_t)-1 >= 65535u);
}

# endif

#endif
