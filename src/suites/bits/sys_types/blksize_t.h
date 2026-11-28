/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file blksize_t.h
 * @brief Cases of blksize_t, for every header that defines it.
 * @Created: 2026/11/28 12:19:13 by Moutig
 * @Updated: 2026/11/28 12:19:19 by Moutig
 *
 * blksize_t is a signed integer type (XBD <sys/types.h>; XSI in
 * <sys/stat.h> from POSIX.1-2017).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_BLKSIZE_T_H
# define BITS_SYS_TYPES_BLKSIZE_T_H

# include "framework/lc.h"

# if defined(LC_REQ_blksize_t) || defined(LC_DEFS_ALL)
TEST_SYM(blksize_t__declared, "blksize_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	blksize_t	v = (blksize_t)1;

	EXPECT_TYPE(v, blksize_t);
}

TEST_SYM(blksize_t__signed, "blksize_t", is_a_signed_integer_type, .ref = "XBD <sys/types.h>")
{
	EXPECT_TRUE((blksize_t)-1 < 0);
	EXPECT_TRUE((blksize_t)7 / 2 == 3);
}
# endif

#endif
