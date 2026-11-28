/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file locks.h
 * @brief Cases of F_RDLCK, F_UNLCK and F_WRLCK.
 * @Created: 2026/11/28 11:11:44 by Moutig
 * @Updated: 2026/11/28 11:58:50 by Moutig
 *
 * The l_type values of record locking: unique and suitable for use in #if
 * (XBD <fcntl.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_FCNTL_LOCKS_H
# define BITS_FCNTL_LOCKS_H

# include "framework/lc.h"

# if defined(LC_REQ_F_RDLCK) || defined(LC_DEFS_ALL)
TEST_SYM(F_RDLCK__if, "F_RDLCK", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_RDLCK == 0
	EXPECT_EQ(F_RDLCK, 0);
#  else
	EXPECT_NE(F_RDLCK, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_UNLCK) || defined(LC_DEFS_ALL)
TEST_SYM(F_UNLCK__if, "F_UNLCK", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_UNLCK == 0
	EXPECT_EQ(F_UNLCK, 0);
#  else
	EXPECT_NE(F_UNLCK, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_WRLCK) || defined(LC_DEFS_ALL)
TEST_SYM(F_WRLCK__if, "F_WRLCK", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_WRLCK == 0
	EXPECT_EQ(F_WRLCK, 0);
#  else
	EXPECT_NE(F_WRLCK, 0);
#  endif
}
# endif

# if (defined(LC_REQ_F_RDLCK) && defined(LC_REQ_F_UNLCK) && defined(LC_REQ_F_WRLCK)) || defined(LC_DEFS_ALL)
TEST_SYM(F_RDLCK__distinct, "F_RDLCK", distinct_from_the_other_lock_types, .ref = "XBD <fcntl.h>")
{
	const long	v[3] = {F_RDLCK, F_UNLCK, F_WRLCK};
	const int	n = 3;
	int			bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += v[i] == v[j];
	EXPECT_EQ(bad, 0);
}
# endif

#endif
