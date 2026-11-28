/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file dev_t.h
 * @brief Cases of dev_t, for every header that defines it.
 * @Created: 2026/11/28 12:19:23 by Moutig
 * @Updated: 2026/11/28 12:19:30 by Moutig
 *
 * dev_t is an arithmetic type, an integer type from POSIX.1-2024 (XBD
 * <sys/types.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_TYPES_DEV_T_H
# define BITS_SYS_TYPES_DEV_T_H

# include "framework/lc.h"

# if defined(LC_REQ_dev_t) || defined(LC_DEFS_ALL)
TEST_SYM(dev_t__declared, "dev_t", declared_by_the_header, .ref = "XBD <sys/types.h>")
{
	dev_t	v = (dev_t)1;

	EXPECT_TYPE(v, dev_t);
}

TEST_SYM(dev_t__integer, "dev_t", is_an_integer_type, .since = "POSIX.1-2024", .ref = "XBD <sys/types.h>")
{
	EXPECT_TRUE((dev_t)7 / 2 == 3);
	EXPECT_TRUE((dev_t)0.5 == 0);
}
# endif

#endif
