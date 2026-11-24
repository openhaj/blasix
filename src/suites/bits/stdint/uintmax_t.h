/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file uintmax_t.h
 * @brief Cases of uintmax_t, for every header that defines it.
 * @Created: 2026/11/24 12:11:11 by Moutig
 * @Updated: 2026/11/24 12:11:18 by Moutig
 *
 * uintmax_t "designates an unsigned integer type capable of representing
 * any value of any unsigned integer type", at least 64 bits (C99
 * 7.18.1.5, 7.18.2.5; C11 / C17 7.20; C23 7.22): <stdint.h>, and
 * <inttypes.h>, which includes it.
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_uintmax_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDINT_UINTMAX_T_H
# define BITS_STDINT_UINTMAX_T_H

# include "framework/lc.h"

# if defined(LC_REQ_uintmax_t) || defined(LC_DEFS_ALL)

TEST_SYM(uintmax_t__declared, "uintmax_t", declared_by_the_header, .ref = "C11 7.20.1.5p1")
{
	uintmax_t	v = 0;

	EXPECT_TRUE(v == 0);
}

TEST_SYM(uintmax_t__unsigned, "uintmax_t", is_unsigned, .ref = "C11 7.20.1.5p1")
{
	EXPECT_TRUE((uintmax_t)-1 > 0);
}

/* Holds every unsigned long long value: at least 64 bits. */
TEST_SYM(uintmax_t__width, "uintmax_t", holds_every_unsigned_long_long, .ref = "C11 7.20.1.5p1, 7.20.2.5p1")
{
	uintmax_t	m = (uintmax_t)18446744073709551615ULL;

	EXPECT_TRUE(m == 18446744073709551615ULL);
	EXPECT_TRUE((uintmax_t)-1 >= m);
	EXPECT_GE(sizeof(uintmax_t), sizeof(unsigned long long));
}

# endif

#endif
