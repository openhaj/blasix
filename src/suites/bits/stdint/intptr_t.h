/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file intptr_t.h
 * @brief Cases of intptr_t, for every header that defines it.
 * @Created: 2026/11/28 09:30:11 by Moutig
 * @Updated: 2026/11/28 09:30:22 by Moutig
 *
 * intptr_t is a signed integer type to which a valid pointer to void converts and back, comparing equal (C 7.20.1.4p1, <stdint.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_STDINT_INTPTR_T_H
# define BITS_STDINT_INTPTR_T_H

# include "framework/lc.h"

# if defined(LC_REQ_intptr_t) || defined(LC_DEFS_ALL)
TEST_SYM(intptr_t__declared, "intptr_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	intptr_t	v = (intptr_t)1;

	EXPECT_TYPE(v, intptr_t);
}

TEST_SYM(intptr_t__round_trip, "intptr_t", void_pointer_converts_and_back, .ref = "C11 7.20.1.4p1")
{
	static int	x;
	void		*p = &x;
	intptr_t		i = (intptr_t)p;

	EXPECT_PTREQ((void *)i, p);
	EXPECT_TRUE((intptr_t)-1 < 0);
}
# endif

#endif
