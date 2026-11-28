/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file ino_t.h
 * @brief Cases of ino_t, for every header that defines it.
 * @Created: 2026/11/28 12:19:35 by Moutig
 * @Updated: 2026/11/28 12:19:41 by Moutig
 *
 * ino_t is an unsigned integer type: XSI up to POSIX.1-2017, every
 * system from POSIX.1-2024 (XBD <sys/types.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_INO_T_H
# define BITS_SYS_TYPES_INO_T_H

# include "framework/lc.h"

# if defined(LC_REQ_ino_t) || defined(LC_DEFS_ALL)
TEST_SYM(ino_t__declared, "ino_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	ino_t	v = (ino_t)1;

	EXPECT_TYPE(v, ino_t);
}

TEST_SYM(ino_t__unsigned_xsi, "ino_t", is_an_unsigned_integer_type, .opt = "XSI",
	.until = "POSIX.1-2024", .ref = "XBD <sys/types.h>")
{
	EXPECT_TRUE((ino_t)-1 > 0);
	EXPECT_TRUE((ino_t)7 / 2 == 3);
}

TEST_SYM(ino_t__unsigned, "ino_t", is_an_unsigned_integer_type_everywhere, .since = "POSIX.1-2024",
	.ref = "XBD <sys/types.h>")
{
	EXPECT_TRUE((ino_t)-1 > 0);
	EXPECT_TRUE((ino_t)7 / 2 == 3);
}
# endif

#endif
