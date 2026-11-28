/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file fd_flags.h
 * @brief Cases of FD_CLOEXEC and FD_CLOFORK.
 * @Created: 2026/11/28 11:11:37 by Moutig
 * @Updated: 2026/11/28 11:58:48 by Moutig
 *
 * The descriptor flags of fcntl(): bitwise-distinct and suitable for use in
 * #if (XBD <fcntl.h>); FD_CLOFORK is POSIX.1-2024.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_FCNTL_FD_FLAGS_H
# define BITS_FCNTL_FD_FLAGS_H

# include "framework/lc.h"

# if defined(LC_REQ_FD_CLOEXEC) || defined(LC_DEFS_ALL)
TEST_SYM(FD_CLOEXEC__if, "FD_CLOEXEC", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if FD_CLOEXEC == 0
	EXPECT_EQ(FD_CLOEXEC, 0);
#  else
	EXPECT_NE(FD_CLOEXEC, 0);
#  endif
}
# endif

# if defined(LC_REQ_FD_CLOFORK) || defined(LC_DEFS_ALL)
TEST_SYM(FD_CLOFORK__if, "FD_CLOFORK", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if FD_CLOFORK == 0
	EXPECT_EQ(FD_CLOFORK, 0);
#  else
	EXPECT_NE(FD_CLOFORK, 0);
#  endif
}
# endif

# if defined(LC_REQ_FD_CLOEXEC) || defined(LC_DEFS_ALL)
TEST_SYM(FD_CLOEXEC__bit, "FD_CLOEXEC", is_a_nonzero_flag, .ref = "XBD <fcntl.h>")
{
	EXPECT_NE(FD_CLOEXEC, 0);
}
# endif

# if (defined(LC_REQ_FD_CLOEXEC) && defined(LC_REQ_FD_CLOFORK)) || defined(LC_DEFS_ALL)
TEST_SYM(FD_CLOFORK__distinct, "FD_CLOFORK", bitwise_distinct_from_FD_CLOEXEC, .ref = "XBD <fcntl.h> (Issue 8)")
{
	const long	v[2] = {FD_CLOEXEC, FD_CLOFORK};
	const int	n = 2;
	int			bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += (v[i] & v[j]) != 0;
	for (int i = 0; i < n; i++)
		bad += v[i] == 0;
	EXPECT_EQ(bad, 0);
}
# endif

#endif
