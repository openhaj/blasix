/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file fadv.h
 * @brief Cases of the POSIX_FADV_* advices (ADV).
 * @Created: 2026/11/28 11:11:32 by Moutig
 * @Updated: 2026/11/28 11:58:40 by Moutig
 *
 * The advice values of posix_fadvise() (Advisory Information): six
 * different values (XBD <fcntl.h>).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_FCNTL_FADV_H
# define BITS_FCNTL_FADV_H

# include "framework/lc.h"

# if defined(LC_REQ_POSIX_FADV_DONTNEED) || defined(LC_DEFS_ALL)
TEST_SYM(POSIX_FADV_DONTNEED__int, "POSIX_FADV_DONTNEED", is_an_int_value, .opt = "ADV", .ref = "XBD <fcntl.h>")
{
	int	v = POSIX_FADV_DONTNEED;

	EXPECT_TRUE(v == POSIX_FADV_DONTNEED);
}
# endif

# if defined(LC_REQ_POSIX_FADV_NOREUSE) || defined(LC_DEFS_ALL)
TEST_SYM(POSIX_FADV_NOREUSE__int, "POSIX_FADV_NOREUSE", is_an_int_value, .opt = "ADV", .ref = "XBD <fcntl.h>")
{
	int	v = POSIX_FADV_NOREUSE;

	EXPECT_TRUE(v == POSIX_FADV_NOREUSE);
}
# endif

# if defined(LC_REQ_POSIX_FADV_NORMAL) || defined(LC_DEFS_ALL)
TEST_SYM(POSIX_FADV_NORMAL__int, "POSIX_FADV_NORMAL", is_an_int_value, .opt = "ADV", .ref = "XBD <fcntl.h>")
{
	int	v = POSIX_FADV_NORMAL;

	EXPECT_TRUE(v == POSIX_FADV_NORMAL);
}
# endif

# if defined(LC_REQ_POSIX_FADV_RANDOM) || defined(LC_DEFS_ALL)
TEST_SYM(POSIX_FADV_RANDOM__int, "POSIX_FADV_RANDOM", is_an_int_value, .opt = "ADV", .ref = "XBD <fcntl.h>")
{
	int	v = POSIX_FADV_RANDOM;

	EXPECT_TRUE(v == POSIX_FADV_RANDOM);
}
# endif

# if defined(LC_REQ_POSIX_FADV_SEQUENTIAL) || defined(LC_DEFS_ALL)
TEST_SYM(POSIX_FADV_SEQUENTIAL__int, "POSIX_FADV_SEQUENTIAL", is_an_int_value, .opt = "ADV", .ref = "XBD <fcntl.h>")
{
	int	v = POSIX_FADV_SEQUENTIAL;

	EXPECT_TRUE(v == POSIX_FADV_SEQUENTIAL);
}
# endif

# if defined(LC_REQ_POSIX_FADV_WILLNEED) || defined(LC_DEFS_ALL)
TEST_SYM(POSIX_FADV_WILLNEED__int, "POSIX_FADV_WILLNEED", is_an_int_value, .opt = "ADV", .ref = "XBD <fcntl.h>")
{
	int	v = POSIX_FADV_WILLNEED;

	EXPECT_TRUE(v == POSIX_FADV_WILLNEED);
}
# endif

# if (defined(LC_REQ_POSIX_FADV_DONTNEED) && defined(LC_REQ_POSIX_FADV_NOREUSE) && defined(LC_REQ_POSIX_FADV_NORMAL) && defined(LC_REQ_POSIX_FADV_RANDOM) && defined(LC_REQ_POSIX_FADV_SEQUENTIAL) && defined(LC_REQ_POSIX_FADV_WILLNEED)) || defined(LC_DEFS_ALL)
TEST_SYM(POSIX_FADV_NORMAL__distinct, "POSIX_FADV_NORMAL", distinct_from_the_other_advices, .ref = "XBD <fcntl.h>")
{
	const long	v[6] = {POSIX_FADV_DONTNEED, POSIX_FADV_NOREUSE, POSIX_FADV_NORMAL, POSIX_FADV_RANDOM, POSIX_FADV_SEQUENTIAL, POSIX_FADV_WILLNEED};
	const int	n = 6;
	int			bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += v[i] == v[j];
	EXPECT_EQ(bad, 0);
}
# endif

#endif
