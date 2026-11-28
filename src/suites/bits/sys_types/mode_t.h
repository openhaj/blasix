/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file mode_t.h
 * @brief Cases of mode_t, for every header that defines it.
 * @Created: 2026/11/28 11:12:52 by Moutig
 * @Updated: 2026/11/28 11:13:24 by Moutig
 *
 * mode_t is an integer type (XBD <sys/types.h>); the file mode bits are
 * values of it.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_MODE_T_H
# define BITS_SYS_TYPES_MODE_T_H

# include "framework/lc.h"

# if defined(LC_REQ_mode_t) || defined(LC_DEFS_ALL)
TEST_SYM(mode_t__declared, "mode_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	mode_t	m = (mode_t)0644;

	EXPECT_TYPE(m, mode_t);
}

TEST_SYM(mode_t__integer, "mode_t", is_an_integer_type, .ref = "XBD <sys/types.h>")
{
	EXPECT_TRUE((mode_t)7 / 2 == 3);
	EXPECT_TRUE((mode_t)07777 == 07777);
}
# endif

#endif
