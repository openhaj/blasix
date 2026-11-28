/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file off_t.h
 * @brief Cases of off_t, for every header that defines it.
 * @Created: 2026/11/28 09:31:31 by Moutig
 * @Updated: 2026/11/28 09:31:41 by Moutig
 *
 * off_t is a signed integer type (XBD <sys/types.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_OFF_T_H
# define BITS_SYS_TYPES_OFF_T_H

# include "framework/lc.h"

# if defined(LC_REQ_off_t) || defined(LC_DEFS_ALL)
TEST_SYM(off_t__declared, "off_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	off_t	v = (off_t)1;

	EXPECT_TYPE(v, off_t);
}

TEST_SYM(off_t__signed, "off_t", is_a_signed_integer_type, .ref = "XBD <sys/types.h>")
{
	off_t	m = (off_t)-1;

	EXPECT_TRUE(m < 0);
	EXPECT_TRUE((off_t)7 / 2 == 3);
}
# endif

#endif
