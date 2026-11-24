/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file intmax_t.h
 * @brief Cases of intmax_t, for every header that defines it.
 * @Created: 2026/11/24 12:10:55 by Moutig
 * @Updated: 2026/11/24 12:11:05 by Moutig
 *
 * intmax_t "designates a signed integer type capable of representing any
 * value of any signed integer type", at least 64 bits (C99 7.18.1.5,
 * 7.18.2.5; C11 / C17 7.20; C23 7.22): <stdint.h>, and <inttypes.h>, which
 * includes it. Only intmax_t is used (INTMAX_MAX: the header's defs.c).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_intmax_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDINT_INTMAX_T_H
# define BITS_STDINT_INTMAX_T_H

# include "framework/lc.h"

# if defined(LC_REQ_intmax_t) || defined(LC_DEFS_ALL)

TEST_SYM(intmax_t__declared, "intmax_t", declared_by_the_header, .ref = "C11 7.20.1.5p1")
{
	intmax_t	v = 0;

	EXPECT_TRUE(v == 0);
}

TEST_SYM(intmax_t__signed, "intmax_t", is_signed, .ref = "C11 7.20.1.5p1")
{
	EXPECT_TRUE((intmax_t)-1 < 0);
}

/* Holds every long long value: at least 64 bits (7.20.2.5). */
TEST_SYM(intmax_t__width, "intmax_t", holds_every_long_long, .ref = "C11 7.20.1.5p1, 7.20.2.5p1")
{
	intmax_t	hi = (intmax_t)9223372036854775807LL;
	intmax_t	lo = -hi - 1;

	EXPECT_TRUE(hi == 9223372036854775807LL);
	EXPECT_TRUE(lo < 0 && lo + hi == -1);
	EXPECT_GE(sizeof(intmax_t), sizeof(long long));
}

# endif

#endif
