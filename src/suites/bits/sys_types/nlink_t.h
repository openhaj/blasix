/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file nlink_t.h
 * @brief Cases of nlink_t, for every header that defines it.
 * @Created: 2026/11/28 12:19:47 by Moutig
 * @Updated: 2026/11/28 12:19:52 by Moutig
 *
 * nlink_t is an integer type (XBD <sys/types.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_NLINK_T_H
# define BITS_SYS_TYPES_NLINK_T_H

# include "framework/lc.h"

# if defined(LC_REQ_nlink_t) || defined(LC_DEFS_ALL)
TEST_SYM(nlink_t__declared, "nlink_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	nlink_t	v = (nlink_t)1;

	EXPECT_TYPE(v, nlink_t);
}

TEST_SYM(nlink_t__integer, "nlink_t", is_an_integer_type, .ref = "XBD <sys/types.h>")
{
	EXPECT_TRUE((nlink_t)7 / 2 == 3);
	EXPECT_TRUE((nlink_t)0.5 == 0);
}
# endif

#endif
