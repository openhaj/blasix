/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file lldiv_t.h
 * @brief Cases of the lldiv_t type of <stdlib.h>.
 * @Created: 2026/11/25 12:44:44 by Moutig
 * @Updated: 2026/11/25 12:44:50 by Moutig
 *
 * lldiv_t is a structure type with (in either order) the members quot and
 * rem, both long long (C99 7.20p2, 7.20.6.2p3; C11, C17 7.22p3, 7.22.6.2p3;
 * C23 7.24p3, 7.24.6.2p3). That lldiv() returns it is the prototype case
 * of the lldiv unit (a function the libc lacks must not break defs.c).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_lldiv_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDLIB_LLDIV_T_H
# define BITS_STDLIB_LLDIV_T_H

# include "framework/lc.h"

# if defined(LC_REQ_lldiv_t) || defined(LC_DEFS_ALL)

TEST_SYM(lldiv_t__members, "lldiv_t", has_quot_and_rem_of_long_long, .ref = "C11 7.22.6.2p3")
{
	lldiv_t	r;

	r.quot = 0x7fffffffffffffffLL;
	r.rem = -0x7fffffffffffffffLL;
	EXPECT_TYPE(r.quot, long long);
	EXPECT_TYPE(r.rem, long long);
	EXPECT_TRUE(r.quot == 0x7fffffffffffffffLL && r.rem == -0x7fffffffffffffffLL);
}

/* A structure: members by name, any order, copied by assignment. */
TEST_SYM(lldiv_t__struct, "lldiv_t", is_a_structure, .ref = "C11 7.22p3")
{
	lldiv_t	a = {0};
	lldiv_t	b;

	a.quot = 3;
	a.rem = 4;
	b = a;
	EXPECT_TRUE(b.quot == 3 && b.rem == 4);
	EXPECT_GE(sizeof(lldiv_t), 2 * sizeof(long long));
}

# endif

#endif
