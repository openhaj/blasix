/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file imaxdiv_t.h
 * @brief Cases of the imaxdiv_t type of <inttypes.h>.
 * @Created: 2026/11/24 12:14:40 by Moutig
 * @Updated: 2026/11/24 12:14:46 by Moutig
 *
 * imaxdiv_t is a structure type, the type of the value imaxdiv() returns,
 * with (in either order) the members quot and rem, both intmax_t (C99,
 * C11, C17 7.8p2, 7.8.2.2p3; C23 7.8p3).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_imaxdiv_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_INTTYPES_IMAXDIV_T_H
# define BITS_INTTYPES_IMAXDIV_T_H

# include "framework/lc.h"

# if defined(LC_REQ_imaxdiv_t) || defined(LC_DEFS_ALL)

TEST_SYM(imaxdiv_t__result, "imaxdiv_t", is_the_result_of_imaxdiv, .ref = "C11 7.8p2")
{
	EXPECT_TYPE(imaxdiv(1, 1), imaxdiv_t);
}

TEST_SYM(imaxdiv_t__members, "imaxdiv_t", has_quot_and_rem_of_intmax_t, .ref = "C11 7.8.2.2p3")
{
	imaxdiv_t	r;

	r.quot = INTMAX_MAX;
	r.rem = INTMAX_MIN;
	EXPECT_TYPE(r.quot, intmax_t);
	EXPECT_TYPE(r.rem, intmax_t);
	EXPECT_TRUE(r.quot == INTMAX_MAX && r.rem == INTMAX_MIN);
}

/* A structure: members by name, any order, copied by assignment. */
TEST_SYM(imaxdiv_t__struct, "imaxdiv_t", is_a_structure, .ref = "C11 7.8p2")
{
	imaxdiv_t	a = {0};
	imaxdiv_t	b;

	a.quot = 3;
	a.rem = 4;
	b = a;
	EXPECT_TRUE(b.quot == 3 && b.rem == 4);
	EXPECT_GE(sizeof(imaxdiv_t), 2 * sizeof(intmax_t));
}

# endif

#endif
