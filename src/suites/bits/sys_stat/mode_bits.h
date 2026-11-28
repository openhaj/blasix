/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file mode_bits.h
 * @brief Cases of the file mode bits S_IRWXU ... S_ISVTX.
 * @Created: 2026/11/28 11:12:33 by Moutig
 * @Updated: 2026/11/28 11:59:08 by Moutig
 *
 * XBD <sys/stat.h>: the permission bits S_IRUSR ... S_IXOTH, S_ISUID,
 * S_ISGID and (XSI) S_ISVTX are unique bits; S_IRWXU, S_IRWXG, S_IRWXO
 * are the OR of the read, write and execute bits of their class. From
 * POSIX.1-2008 they have the numeric values 0400 ... 01, 04000, 02000,
 * 01000 and are usable in #if. <fcntl.h> defines them as described there.
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_SYS_STAT_MODE_BITS_H
# define BITS_SYS_STAT_MODE_BITS_H

# include "framework/lc.h"

# if defined(LC_REQ_S_IRUSR) || defined(LC_DEFS_ALL)
TEST_SYM(S_IRUSR__value, "S_IRUSR", has_the_value_0400, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IRUSR == 0400
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IRUSR, 0400);
#  endif
}
# endif

# if defined(LC_REQ_S_IWUSR) || defined(LC_DEFS_ALL)
TEST_SYM(S_IWUSR__value, "S_IWUSR", has_the_value_0200, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IWUSR == 0200
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IWUSR, 0200);
#  endif
}
# endif

# if defined(LC_REQ_S_IXUSR) || defined(LC_DEFS_ALL)
TEST_SYM(S_IXUSR__value, "S_IXUSR", has_the_value_0100, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IXUSR == 0100
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IXUSR, 0100);
#  endif
}
# endif

# if defined(LC_REQ_S_IRGRP) || defined(LC_DEFS_ALL)
TEST_SYM(S_IRGRP__value, "S_IRGRP", has_the_value_040, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IRGRP == 040
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IRGRP, 040);
#  endif
}
# endif

# if defined(LC_REQ_S_IWGRP) || defined(LC_DEFS_ALL)
TEST_SYM(S_IWGRP__value, "S_IWGRP", has_the_value_020, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IWGRP == 020
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IWGRP, 020);
#  endif
}
# endif

# if defined(LC_REQ_S_IXGRP) || defined(LC_DEFS_ALL)
TEST_SYM(S_IXGRP__value, "S_IXGRP", has_the_value_010, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IXGRP == 010
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IXGRP, 010);
#  endif
}
# endif

# if defined(LC_REQ_S_IROTH) || defined(LC_DEFS_ALL)
TEST_SYM(S_IROTH__value, "S_IROTH", has_the_value_04, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IROTH == 04
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IROTH, 04);
#  endif
}
# endif

# if defined(LC_REQ_S_IWOTH) || defined(LC_DEFS_ALL)
TEST_SYM(S_IWOTH__value, "S_IWOTH", has_the_value_02, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IWOTH == 02
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IWOTH, 02);
#  endif
}
# endif

# if defined(LC_REQ_S_IXOTH) || defined(LC_DEFS_ALL)
TEST_SYM(S_IXOTH__value, "S_IXOTH", has_the_value_01, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IXOTH == 01
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IXOTH, 01);
#  endif
}
# endif

# if defined(LC_REQ_S_ISUID) || defined(LC_DEFS_ALL)
TEST_SYM(S_ISUID__value, "S_ISUID", has_the_value_04000, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_ISUID == 04000
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_ISUID, 04000);
#  endif
}
# endif

# if defined(LC_REQ_S_ISGID) || defined(LC_DEFS_ALL)
TEST_SYM(S_ISGID__value, "S_ISGID", has_the_value_02000, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_ISGID == 02000
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_ISGID, 02000);
#  endif
}
# endif

# if defined(LC_REQ_S_ISVTX) || defined(LC_DEFS_ALL)
TEST_SYM(S_ISVTX__value, "S_ISVTX", has_the_value_01000, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_ISVTX == 01000
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_ISVTX, 01000);
#  endif
}
# endif

# if defined(LC_REQ_S_IRWXU) || defined(LC_DEFS_ALL)
TEST_SYM(S_IRWXU__value, "S_IRWXU", has_the_value_0700, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IRWXU == 0700
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IRWXU, 0700);
#  endif
}
# endif

# if defined(LC_REQ_S_IRWXG) || defined(LC_DEFS_ALL)
TEST_SYM(S_IRWXG__value, "S_IRWXG", has_the_value_070, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IRWXG == 070
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IRWXG, 070);
#  endif
}
# endif

# if defined(LC_REQ_S_IRWXO) || defined(LC_DEFS_ALL)
TEST_SYM(S_IRWXO__value, "S_IRWXO", has_the_value_07, .since = "POSIX.1-2008", .ref = "XBD <sys/stat.h>")
{
#  if S_IRWXO == 07
	EXPECT_TRUE(1);
#  else
	EXPECT_EQ(S_IRWXO, 07);
#  endif
}
# endif

# if (defined(LC_REQ_S_IRUSR) && defined(LC_REQ_S_IWUSR) && defined(LC_REQ_S_IXUSR) && defined(LC_REQ_S_IRGRP) && defined(LC_REQ_S_IWGRP) && defined(LC_REQ_S_IXGRP) && defined(LC_REQ_S_IROTH) && defined(LC_REQ_S_IWOTH) && defined(LC_REQ_S_IXOTH) && defined(LC_REQ_S_ISUID) && defined(LC_REQ_S_ISGID)) || defined(LC_DEFS_ALL)
TEST_SYM(S_IRUSR__distinct, "S_IRUSR", bitwise_distinct_from_the_other_mode_bits, .ref = "XBD <sys/stat.h>")
{
	const long	v[11] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH, S_ISUID, S_ISGID};
	const int	n = 11;
	int			bad = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < i; j++)
			bad += (v[i] & v[j]) != 0;
	for (int i = 0; i < n; i++)
		bad += v[i] == 0;
	EXPECT_EQ(bad, 0);
}
# endif

# if (defined(LC_REQ_S_ISVTX) && defined(LC_REQ_S_IRUSR) && defined(LC_REQ_S_IWUSR) && defined(LC_REQ_S_IXUSR) && defined(LC_REQ_S_IRGRP) && defined(LC_REQ_S_IWGRP) && defined(LC_REQ_S_IXGRP) && defined(LC_REQ_S_IROTH) && defined(LC_REQ_S_IWOTH) && defined(LC_REQ_S_IXOTH) && defined(LC_REQ_S_ISUID) && defined(LC_REQ_S_ISGID)) || defined(LC_DEFS_ALL)
TEST_SYM(S_ISVTX__distinct, "S_ISVTX", bitwise_distinct_from_the_other_mode_bits, .opt = "XSI",
	.ref = "XBD <sys/stat.h>")
{
	EXPECT_NE(S_ISVTX, 0);
	EXPECT_EQ(S_ISVTX & (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH
		| S_IWOTH | S_IXOTH | S_ISUID | S_ISGID), 0);
}
# endif

# if (defined(LC_REQ_S_IRWXU) && defined(LC_REQ_S_IRUSR) && defined(LC_REQ_S_IWUSR) && defined(LC_REQ_S_IXUSR)) || defined(LC_DEFS_ALL)
TEST_SYM(S_IRWXU__or, "S_IRWXU", is_the_or_of_read_write_execute, .ref = "XBD <sys/stat.h>")
{
	EXPECT_EQ(S_IRWXU, S_IRUSR | S_IWUSR | S_IXUSR);
}
# endif

# if (defined(LC_REQ_S_IRWXG) && defined(LC_REQ_S_IRGRP) && defined(LC_REQ_S_IWGRP) && defined(LC_REQ_S_IXGRP)) || defined(LC_DEFS_ALL)
TEST_SYM(S_IRWXG__or, "S_IRWXG", is_the_or_of_read_write_execute, .ref = "XBD <sys/stat.h>")
{
	EXPECT_EQ(S_IRWXG, S_IRGRP | S_IWGRP | S_IXGRP);
}
# endif

# if (defined(LC_REQ_S_IRWXO) && defined(LC_REQ_S_IROTH) && defined(LC_REQ_S_IWOTH) && defined(LC_REQ_S_IXOTH)) || defined(LC_DEFS_ALL)
TEST_SYM(S_IRWXO__or, "S_IRWXO", is_the_or_of_read_write_execute, .ref = "XBD <sys/stat.h>")
{
	EXPECT_EQ(S_IRWXO, S_IROTH | S_IWOTH | S_IXOTH);
}
# endif

#endif
