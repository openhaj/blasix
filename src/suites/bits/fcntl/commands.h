/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file commands.h
 * @brief Cases of the fcntl() commands F_* of <fcntl.h>.
 * @Created: 2026/11/28 11:11:22 by Moutig
 * @Updated: 2026/11/28 11:58:36 by Moutig
 *
 * The cmd values of fcntl() are unique and suitable for use in #if (XBD
 * <fcntl.h>): F_DUPFD ... F_SETOWN (2001), F_DUPFD_CLOEXEC (2008),
 * F_DUPFD_CLOFORK, F_GETOWN_EX, F_SETOWN_EX, F_OFD_* (2024).
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_FCNTL_COMMANDS_H
# define BITS_FCNTL_COMMANDS_H

# include "framework/lc.h"

# if defined(LC_REQ_F_DUPFD) || defined(LC_DEFS_ALL)
TEST_SYM(F_DUPFD__if, "F_DUPFD", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_DUPFD == 0
	EXPECT_EQ(F_DUPFD, 0);
#  else
	EXPECT_NE(F_DUPFD, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_GETFD) || defined(LC_DEFS_ALL)
TEST_SYM(F_GETFD__if, "F_GETFD", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_GETFD == 0
	EXPECT_EQ(F_GETFD, 0);
#  else
	EXPECT_NE(F_GETFD, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_SETFD) || defined(LC_DEFS_ALL)
TEST_SYM(F_SETFD__if, "F_SETFD", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_SETFD == 0
	EXPECT_EQ(F_SETFD, 0);
#  else
	EXPECT_NE(F_SETFD, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_GETFL) || defined(LC_DEFS_ALL)
TEST_SYM(F_GETFL__if, "F_GETFL", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_GETFL == 0
	EXPECT_EQ(F_GETFL, 0);
#  else
	EXPECT_NE(F_GETFL, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_SETFL) || defined(LC_DEFS_ALL)
TEST_SYM(F_SETFL__if, "F_SETFL", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_SETFL == 0
	EXPECT_EQ(F_SETFL, 0);
#  else
	EXPECT_NE(F_SETFL, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_GETLK) || defined(LC_DEFS_ALL)
TEST_SYM(F_GETLK__if, "F_GETLK", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_GETLK == 0
	EXPECT_EQ(F_GETLK, 0);
#  else
	EXPECT_NE(F_GETLK, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_SETLK) || defined(LC_DEFS_ALL)
TEST_SYM(F_SETLK__if, "F_SETLK", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_SETLK == 0
	EXPECT_EQ(F_SETLK, 0);
#  else
	EXPECT_NE(F_SETLK, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_SETLKW) || defined(LC_DEFS_ALL)
TEST_SYM(F_SETLKW__if, "F_SETLKW", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_SETLKW == 0
	EXPECT_EQ(F_SETLKW, 0);
#  else
	EXPECT_NE(F_SETLKW, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_GETOWN) || defined(LC_DEFS_ALL)
TEST_SYM(F_GETOWN__if, "F_GETOWN", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_GETOWN == 0
	EXPECT_EQ(F_GETOWN, 0);
#  else
	EXPECT_NE(F_GETOWN, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_SETOWN) || defined(LC_DEFS_ALL)
TEST_SYM(F_SETOWN__if, "F_SETOWN", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_SETOWN == 0
	EXPECT_EQ(F_SETOWN, 0);
#  else
	EXPECT_NE(F_SETOWN, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_DUPFD_CLOEXEC) || defined(LC_DEFS_ALL)
TEST_SYM(F_DUPFD_CLOEXEC__if, "F_DUPFD_CLOEXEC", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_DUPFD_CLOEXEC == 0
	EXPECT_EQ(F_DUPFD_CLOEXEC, 0);
#  else
	EXPECT_NE(F_DUPFD_CLOEXEC, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_DUPFD_CLOFORK) || defined(LC_DEFS_ALL)
TEST_SYM(F_DUPFD_CLOFORK__if, "F_DUPFD_CLOFORK", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_DUPFD_CLOFORK == 0
	EXPECT_EQ(F_DUPFD_CLOFORK, 0);
#  else
	EXPECT_NE(F_DUPFD_CLOFORK, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_GETOWN_EX) || defined(LC_DEFS_ALL)
TEST_SYM(F_GETOWN_EX__if, "F_GETOWN_EX", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_GETOWN_EX == 0
	EXPECT_EQ(F_GETOWN_EX, 0);
#  else
	EXPECT_NE(F_GETOWN_EX, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_SETOWN_EX) || defined(LC_DEFS_ALL)
TEST_SYM(F_SETOWN_EX__if, "F_SETOWN_EX", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_SETOWN_EX == 0
	EXPECT_EQ(F_SETOWN_EX, 0);
#  else
	EXPECT_NE(F_SETOWN_EX, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_OFD_GETLK) || defined(LC_DEFS_ALL)
TEST_SYM(F_OFD_GETLK__if, "F_OFD_GETLK", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_OFD_GETLK == 0
	EXPECT_EQ(F_OFD_GETLK, 0);
#  else
	EXPECT_NE(F_OFD_GETLK, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_OFD_SETLK) || defined(LC_DEFS_ALL)
TEST_SYM(F_OFD_SETLK__if, "F_OFD_SETLK", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_OFD_SETLK == 0
	EXPECT_EQ(F_OFD_SETLK, 0);
#  else
	EXPECT_NE(F_OFD_SETLK, 0);
#  endif
}
# endif

# if defined(LC_REQ_F_OFD_SETLKW) || defined(LC_DEFS_ALL)
TEST_SYM(F_OFD_SETLKW__if, "F_OFD_SETLKW", suitable_for_use_in_if, .ref = "XBD <fcntl.h>")
{
#  if F_OFD_SETLKW == 0
	EXPECT_EQ(F_OFD_SETLKW, 0);
#  else
	EXPECT_NE(F_OFD_SETLKW, 0);
#  endif
}
# endif

# if (defined(LC_REQ_F_DUPFD) && defined(LC_REQ_F_GETFD) && defined(LC_REQ_F_SETFD) && defined(LC_REQ_F_GETFL) && defined(LC_REQ_F_SETFL) && defined(LC_REQ_F_GETLK) && defined(LC_REQ_F_SETLK) && defined(LC_REQ_F_SETLKW) && defined(LC_REQ_F_GETOWN) && defined(LC_REQ_F_SETOWN)) || defined(LC_DEFS_ALL)
TEST_SYM(F_DUPFD__distinct, "F_DUPFD", distinct_from_the_other_2001_commands, .ref = "XBD <fcntl.h>")
{
	const long	v[10] = {F_DUPFD, F_GETFD, F_SETFD, F_GETFL, F_SETFL, F_GETLK, F_SETLK, F_SETLKW, F_GETOWN, F_SETOWN};
	const int	n = 10;
	int			bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += v[i] == v[j];
	EXPECT_EQ(bad, 0);
}
# endif

# if (defined(LC_REQ_F_DUPFD) && defined(LC_REQ_F_GETFD) && defined(LC_REQ_F_SETFD) && defined(LC_REQ_F_GETFL) && defined(LC_REQ_F_SETFL) && defined(LC_REQ_F_GETLK) && defined(LC_REQ_F_SETLK) && defined(LC_REQ_F_SETLKW) && defined(LC_REQ_F_GETOWN) && defined(LC_REQ_F_SETOWN) && defined(LC_REQ_F_DUPFD_CLOEXEC)) || defined(LC_DEFS_ALL)
TEST_SYM(F_DUPFD_CLOEXEC__distinct, "F_DUPFD_CLOEXEC", distinct_from_the_other_commands, .ref = "XBD <fcntl.h>")
{
	const long	v[11] = {F_DUPFD, F_GETFD, F_SETFD, F_GETFL, F_SETFL, F_GETLK, F_SETLK, F_SETLKW, F_GETOWN, F_SETOWN, F_DUPFD_CLOEXEC};
	const int	n = 11;
	int			bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += v[i] == v[j];
	EXPECT_EQ(bad, 0);
}
# endif

# if (defined(LC_REQ_F_DUPFD) && defined(LC_REQ_F_GETFD) && defined(LC_REQ_F_SETFD) && defined(LC_REQ_F_GETFL) && defined(LC_REQ_F_SETFL) && defined(LC_REQ_F_GETLK) && defined(LC_REQ_F_SETLK) && defined(LC_REQ_F_SETLKW) && defined(LC_REQ_F_GETOWN) && defined(LC_REQ_F_SETOWN) && defined(LC_REQ_F_DUPFD_CLOEXEC) && defined(LC_REQ_F_DUPFD_CLOFORK) && defined(LC_REQ_F_GETOWN_EX) && defined(LC_REQ_F_SETOWN_EX) && defined(LC_REQ_F_OFD_GETLK) && defined(LC_REQ_F_OFD_SETLK) && defined(LC_REQ_F_OFD_SETLKW)) || defined(LC_DEFS_ALL)
TEST_SYM(F_OFD_GETLK__distinct, "F_OFD_GETLK", distinct_from_the_other_2024_commands, .ref = "XBD <fcntl.h> (Issue 8)")
{
	const long	v[17] = {F_DUPFD, F_GETFD, F_SETFD, F_GETFL, F_SETFL, F_GETLK, F_SETLK, F_SETLKW, F_GETOWN, F_SETOWN, F_DUPFD_CLOEXEC, F_DUPFD_CLOFORK, F_GETOWN_EX, F_SETOWN_EX, F_OFD_GETLK, F_OFD_SETLK, F_OFD_SETLKW};
	const int	n = 17;
	int			bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += v[i] == v[j];
	EXPECT_EQ(bad, 0);
}
# endif

#endif
