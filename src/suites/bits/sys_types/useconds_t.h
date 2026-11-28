/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file useconds_t.h
 * @brief Cases of useconds_t, for every header that defines it.
 * @Created: 2026/11/28 09:32:43 by Moutig
 * @Updated: 2026/11/28 09:32:49 by Moutig
 *
 * useconds_t is an unsigned integer type that holds [0, 1000000] (XBD <sys/types.h>, XSI; POSIX.1-2001).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_USECONDS_T_H
# define BITS_SYS_TYPES_USECONDS_T_H

# include "framework/lc.h"

# if defined(LC_REQ_useconds_t) || defined(LC_DEFS_ALL)
TEST_SYM(useconds_t__declared, "useconds_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	useconds_t	v = (useconds_t)1;

	EXPECT_TYPE(v, useconds_t);
}

TEST_SYM(useconds_t__unsigned, "useconds_t", is_unsigned_and_holds_one_million, .ref = "XBD <sys/types.h>")
{
	useconds_t	m = 1000000;

	EXPECT_TRUE((useconds_t)-1 > 0);
	EXPECT_TRUE(m == 1000000);
}
# endif

#endif
