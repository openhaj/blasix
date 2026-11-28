/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file owner.h
 * @brief Cases of F_OWNER_PID and F_OWNER_PGRP (POSIX.1-2024).
 * @Created: 2026/11/28 11:11:58 by Moutig
 * @Updated: 2026/11/28 11:58:56 by Moutig
 *
 * The values of the type member of struct f_owner_ex: unique (XBD
 * <fcntl.h>, POSIX.1-2024).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_FCNTL_OWNER_H
# define BITS_FCNTL_OWNER_H

# include "framework/lc.h"

# if defined(LC_REQ_F_OWNER_PID) || defined(LC_DEFS_ALL)
TEST_SYM(F_OWNER_PID__int, "F_OWNER_PID", is_an_int_value, .ref = "XBD <fcntl.h> (Issue 8)")
{
	int	t = F_OWNER_PID;

	EXPECT_TRUE(t == F_OWNER_PID);
}
# endif

# if defined(LC_REQ_F_OWNER_PGRP) || defined(LC_DEFS_ALL)
TEST_SYM(F_OWNER_PGRP__int, "F_OWNER_PGRP", is_an_int_value, .ref = "XBD <fcntl.h> (Issue 8)")
{
	int	t = F_OWNER_PGRP;

	EXPECT_TRUE(t == F_OWNER_PGRP);
}
# endif

# if (defined(LC_REQ_F_OWNER_PID) && defined(LC_REQ_F_OWNER_PGRP)) || defined(LC_DEFS_ALL)
TEST_SYM(F_OWNER_PID__distinct, "F_OWNER_PID", distinct_from_F_OWNER_PGRP, .ref = "XBD <fcntl.h> (Issue 8)")
{
	const long	v[2] = {F_OWNER_PID, F_OWNER_PGRP};
	const int	n = 2;
	int			bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += v[i] == v[j];
	EXPECT_EQ(bad, 0);
}
# endif

#endif
