/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file ldiv_t.h
 * @brief Cases of the ldiv_t type of <stdlib.h>.
 * @Created: 2026/11/25 12:44:21 by Moutig
 * @Updated: 2026/11/25 12:44:30 by Moutig
 *
 * ldiv_t is a structure type with (in either order) the members quot and
 * rem, both long (C99 7.20p2, 7.20.6.2p3; C11, C17 7.22p3, 7.22.6.2p3;
 * C23 7.24p3, 7.24.6.2p3). That ldiv() returns it is the prototype case
 * of the ldiv unit (a function the libc lacks must not break defs.c).
 * Include it from <category>/defs.c after the header under test; its
 * cases are compiled only when the target requires the symbol
 * (-DLC_REQ_ldiv_t, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDLIB_LDIV_T_H
# define BITS_STDLIB_LDIV_T_H

# include "framework/lc.h"

# if defined(LC_REQ_ldiv_t) || defined(LC_DEFS_ALL)

TEST_SYM(ldiv_t__members, "ldiv_t", has_quot_and_rem_of_long, .ref = "C11 7.22.6.2p3")
{
	ldiv_t	r;

	r.quot = 0x7fffffffL;
	r.rem = -0x7fffffffL;
	EXPECT_TYPE(r.quot, long);
	EXPECT_TYPE(r.rem, long);
	EXPECT_TRUE(r.quot == 0x7fffffffL && r.rem == -0x7fffffffL);
}

/* A structure: members by name, any order, copied by assignment. */
TEST_SYM(ldiv_t__struct, "ldiv_t", is_a_structure, .ref = "C11 7.22p3")
{
	ldiv_t	a = {0};
	ldiv_t	b;

	a.quot = 3;
	a.rem = 4;
	b = a;
	EXPECT_TRUE(b.quot == 3 && b.rem == 4);
	EXPECT_GE(sizeof(ldiv_t), 2 * sizeof(long));
}

# endif

#endif
