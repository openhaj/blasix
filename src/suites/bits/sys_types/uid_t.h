/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file uid_t.h
 * @brief Cases of uid_t, for every header that defines it.
 * @Created: 2026/11/28 09:32:23 by Moutig
 * @Updated: 2026/11/28 09:32:37 by Moutig
 *
 * uid_t is an integer type (XBD <sys/types.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_UID_T_H
# define BITS_SYS_TYPES_UID_T_H

# include "framework/lc.h"

# if defined(LC_REQ_uid_t) || defined(LC_DEFS_ALL)
TEST_SYM(uid_t__declared, "uid_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	uid_t	v = (uid_t)1;

	EXPECT_TYPE(v, uid_t);
}

TEST_SYM(uid_t__integer, "uid_t", is_an_integer_type, .ref = "XBD <sys/types.h>")
{
	EXPECT_TRUE((uid_t)7 / 2 == 3);
	EXPECT_TRUE((uid_t)0.5 == 0);
}
# endif

#endif
