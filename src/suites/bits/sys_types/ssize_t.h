/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file ssize_t.h
 * @brief Cases of ssize_t, for every header that defines it.
 * @Created: 2026/11/28 09:32:04 by Moutig
 * @Updated: 2026/11/28 09:32:10 by Moutig
 *
 * ssize_t is a signed integer type that holds -1 (XBD <sys/types.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_SSIZE_T_H
# define BITS_SYS_TYPES_SSIZE_T_H

# include "framework/lc.h"

# if defined(LC_REQ_ssize_t) || defined(LC_DEFS_ALL)
TEST_SYM(ssize_t__declared, "ssize_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	ssize_t	v = (ssize_t)1;

	EXPECT_TYPE(v, ssize_t);
}

TEST_SYM(ssize_t__signed, "ssize_t", is_a_signed_integer_type, .ref = "XBD <sys/types.h>")
{
	ssize_t	m = (ssize_t)-1;

	EXPECT_TRUE(m < 0);
	EXPECT_TRUE((ssize_t)7 / 2 == 3);
}
# endif

#endif
