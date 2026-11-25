/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file div_t.h
 * @brief Cases of the div_t type of <stdlib.h>.
 * @Created: 2026/11/25 12:42:57 by Moutig
 * @Updated: 2026/11/25 12:43:05 by Moutig
 *
 * div_t is a structure type with (in either order) the members quot and
 * rem, both int (C99 7.20p2, 7.20.6.2p3; C11, C17 7.22p3, 7.22.6.2p3;
 * C23 7.24p3, 7.24.6.2p3). That div() returns it is the prototype case
 * of the div unit (a function the libc lacks must not break defs.c).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_div_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDLIB_DIV_T_H
# define BITS_STDLIB_DIV_T_H

# include "framework/lc.h"

# if defined(LC_REQ_div_t) || defined(LC_DEFS_ALL)

TEST_SYM(div_t__members, "div_t", has_quot_and_rem_of_int, .ref = "C11 7.22.6.2p3")
{
	div_t	r;

	r.quot = 0x7fff;
	r.rem = -0x7fff;
	EXPECT_TYPE(r.quot, int);
	EXPECT_TYPE(r.rem, int);
	EXPECT_TRUE(r.quot == 0x7fff && r.rem == -0x7fff);
}

/* A structure: members by name, any order, copied by assignment. */
TEST_SYM(div_t__struct, "div_t", is_a_structure, .ref = "C11 7.22p3")
{
	div_t	a = {0};
	div_t	b;

	a.quot = 3;
	a.rem = 4;
	b = a;
	EXPECT_TRUE(b.quot == 3 && b.rem == 4);
	EXPECT_GE(sizeof(div_t), 2 * sizeof(int));
}

# endif

#endif
