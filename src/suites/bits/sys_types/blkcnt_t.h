/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file blkcnt_t.h
 * @brief Cases of blkcnt_t, for every header that defines it.
 * @Created: 2026/11/28 12:19:01 by Moutig
 * @Updated: 2026/11/28 12:19:08 by Moutig
 *
 * blkcnt_t is a signed integer type (XBD <sys/types.h>; XSI in
 * <sys/stat.h> from POSIX.1-2017).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_BLKCNT_T_H
# define BITS_SYS_TYPES_BLKCNT_T_H

# include "framework/lc.h"

# if defined(LC_REQ_blkcnt_t) || defined(LC_DEFS_ALL)
TEST_SYM(blkcnt_t__declared, "blkcnt_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	blkcnt_t	v = (blkcnt_t)1;

	EXPECT_TYPE(v, blkcnt_t);
}

TEST_SYM(blkcnt_t__signed, "blkcnt_t", is_a_signed_integer_type, .ref = "XBD <sys/types.h>")
{
	EXPECT_TRUE((blkcnt_t)-1 < 0);
	EXPECT_TRUE((blkcnt_t)7 / 2 == 3);
}
# endif

#endif
