/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file time_t.h
 * @brief Cases of time_t, for every header that defines it.
 * @Created: 2026/11/28 12:19:57 by Moutig
 * @Updated: 2026/11/28 12:20:03 by Moutig
 *
 * time_t is an integer or real-floating type (XBD <sys/types.h>, up to
 * POSIX.1-2017; C11 7.27.1p3: a real type), and from POSIX.1-2024 an
 * integer type of at least 64 bits.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_TIME_T_H
# define BITS_SYS_TYPES_TIME_T_H

# include "framework/lc.h"

# if defined(LC_REQ_time_t) || defined(LC_DEFS_ALL)
TEST_SYM(time_t__declared, "time_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	time_t	v = (time_t)1;

	EXPECT_TYPE(v, time_t);
}

TEST_SYM(time_t__real, "time_t", is_a_real_type, .ref = "XBD <sys/types.h>")
{
	EXPECT_TRUE((time_t)2 + (time_t)3 == (time_t)5);
	EXPECT_TRUE((time_t)1 > (time_t)0);
}

TEST_SYM(time_t__integer, "time_t", is_an_integer_type_of_at_least_64_bits, .since = "POSIX.1-2024",
	.ref = "XBD <sys/types.h>")
{
	EXPECT_TRUE((time_t)0.5 == 0);
	EXPECT_GE((long)(sizeof(time_t) * 8), 64);
}
# endif

#endif
