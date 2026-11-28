/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Moutig <moutig-tan@proton.me>
 *
 * This file is part of blasix.
 * See LICENSE for the full license text.
 */

/**
 * @file confstr.h
 * @brief Cases of the confstr() names of <unistd.h> (_CS_*).
 * @Created: 2026/11/28 09:34:00 by Moutig
 * @Updated: 2026/11/28 09:35:02 by Moutig
 *
 * The _CS_* symbolic constants name what confstr() reports (XBD
 * <unistd.h>, "Constants for Functions"): each one must name something
 * of its own, so its value differs from every other _CS_* name the
 * header defines. What confstr() answers is tested with confstr().
 * Include it from <category>/defs.c after the header under test; each
 * block is compiled only when the target requires its symbol
 * (-DLC_REQ_<name>, or -DLC_DEFS_ALL in the editor) and reported under it.
 */

#ifndef BITS_UNISTD_CONFSTR_H
# define BITS_UNISTD_CONFSTR_H

# include "framework/lc.h"

/* The _CS_* names the header defines, to compare them. */
static const long	bitsConfstrNames[] = {
#  ifdef _CS_PATH
	(long)_CS_PATH,
#  endif
#  ifdef _CS_POSIX_V6_ILP32_OFF32_CFLAGS
	(long)_CS_POSIX_V6_ILP32_OFF32_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V6_ILP32_OFF32_LDFLAGS
	(long)_CS_POSIX_V6_ILP32_OFF32_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V6_ILP32_OFF32_LIBS
	(long)_CS_POSIX_V6_ILP32_OFF32_LIBS,
#  endif
#  ifdef _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS
	(long)_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS
	(long)_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V6_ILP32_OFFBIG_LIBS
	(long)_CS_POSIX_V6_ILP32_OFFBIG_LIBS,
#  endif
#  ifdef _CS_POSIX_V6_LP64_OFF64_CFLAGS
	(long)_CS_POSIX_V6_LP64_OFF64_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V6_LP64_OFF64_LDFLAGS
	(long)_CS_POSIX_V6_LP64_OFF64_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V6_LP64_OFF64_LIBS
	(long)_CS_POSIX_V6_LP64_OFF64_LIBS,
#  endif
#  ifdef _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS
	(long)_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS
	(long)_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V6_LPBIG_OFFBIG_LIBS
	(long)_CS_POSIX_V6_LPBIG_OFFBIG_LIBS,
#  endif
#  ifdef _CS_POSIX_V6_WIDTH_RESTRICTED_ENVS
	(long)_CS_POSIX_V6_WIDTH_RESTRICTED_ENVS,
#  endif
#  ifdef _CS_POSIX_V7_ILP32_OFF32_CFLAGS
	(long)_CS_POSIX_V7_ILP32_OFF32_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V7_ILP32_OFF32_LDFLAGS
	(long)_CS_POSIX_V7_ILP32_OFF32_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V7_ILP32_OFF32_LIBS
	(long)_CS_POSIX_V7_ILP32_OFF32_LIBS,
#  endif
#  ifdef _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS
	(long)_CS_POSIX_V7_ILP32_OFFBIG_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS
	(long)_CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V7_ILP32_OFFBIG_LIBS
	(long)_CS_POSIX_V7_ILP32_OFFBIG_LIBS,
#  endif
#  ifdef _CS_POSIX_V7_LP64_OFF64_CFLAGS
	(long)_CS_POSIX_V7_LP64_OFF64_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V7_LP64_OFF64_LDFLAGS
	(long)_CS_POSIX_V7_LP64_OFF64_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V7_LP64_OFF64_LIBS
	(long)_CS_POSIX_V7_LP64_OFF64_LIBS,
#  endif
#  ifdef _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS
	(long)_CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS
	(long)_CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V7_LPBIG_OFFBIG_LIBS
	(long)_CS_POSIX_V7_LPBIG_OFFBIG_LIBS,
#  endif
#  ifdef _CS_POSIX_V7_THREADS_CFLAGS
	(long)_CS_POSIX_V7_THREADS_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V7_THREADS_LDFLAGS
	(long)_CS_POSIX_V7_THREADS_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V7_WIDTH_RESTRICTED_ENVS
	(long)_CS_POSIX_V7_WIDTH_RESTRICTED_ENVS,
#  endif
#  ifdef _CS_POSIX_V8_ILP32_OFF32_CFLAGS
	(long)_CS_POSIX_V8_ILP32_OFF32_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V8_ILP32_OFF32_LDFLAGS
	(long)_CS_POSIX_V8_ILP32_OFF32_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V8_ILP32_OFF32_LIBS
	(long)_CS_POSIX_V8_ILP32_OFF32_LIBS,
#  endif
#  ifdef _CS_POSIX_V8_ILP32_OFFBIG_CFLAGS
	(long)_CS_POSIX_V8_ILP32_OFFBIG_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V8_ILP32_OFFBIG_LDFLAGS
	(long)_CS_POSIX_V8_ILP32_OFFBIG_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V8_ILP32_OFFBIG_LIBS
	(long)_CS_POSIX_V8_ILP32_OFFBIG_LIBS,
#  endif
#  ifdef _CS_POSIX_V8_LP64_OFF64_CFLAGS
	(long)_CS_POSIX_V8_LP64_OFF64_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V8_LP64_OFF64_LDFLAGS
	(long)_CS_POSIX_V8_LP64_OFF64_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V8_LP64_OFF64_LIBS
	(long)_CS_POSIX_V8_LP64_OFF64_LIBS,
#  endif
#  ifdef _CS_POSIX_V8_LPBIG_OFFBIG_CFLAGS
	(long)_CS_POSIX_V8_LPBIG_OFFBIG_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V8_LPBIG_OFFBIG_LDFLAGS
	(long)_CS_POSIX_V8_LPBIG_OFFBIG_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V8_LPBIG_OFFBIG_LIBS
	(long)_CS_POSIX_V8_LPBIG_OFFBIG_LIBS,
#  endif
#  ifdef _CS_POSIX_V8_THREADS_CFLAGS
	(long)_CS_POSIX_V8_THREADS_CFLAGS,
#  endif
#  ifdef _CS_POSIX_V8_THREADS_LDFLAGS
	(long)_CS_POSIX_V8_THREADS_LDFLAGS,
#  endif
#  ifdef _CS_POSIX_V8_WIDTH_RESTRICTED_ENVS
	(long)_CS_POSIX_V8_WIDTH_RESTRICTED_ENVS,
#  endif
#  ifdef _CS_V6_ENV
	(long)_CS_V6_ENV,
#  endif
#  ifdef _CS_V7_ENV
	(long)_CS_V7_ENV,
#  endif
#  ifdef _CS_V8_ENV
	(long)_CS_V8_ENV,
#  endif
	-12345678L
};

/* Does v appear exactly once among the _CS_* names? */
static inline int	bitsConfstrUnique(long v)
{
	int	k = 0;

	for (int i = 0; i < (int)(sizeof(bitsConfstrNames) / sizeof(bitsConfstrNames[0])); i++)
		k += bitsConfstrNames[i] == v;
	return (k == 1);
}

# if defined(LC_REQ__CS_PATH) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_PATH__unique, "_CS_PATH", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_PATH));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_ILP32_OFF32_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_ILP32_OFF32_CFLAGS__unique, "_CS_POSIX_V6_ILP32_OFF32_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_ILP32_OFF32_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_ILP32_OFF32_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_ILP32_OFF32_LDFLAGS__unique, "_CS_POSIX_V6_ILP32_OFF32_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_ILP32_OFF32_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_ILP32_OFF32_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_ILP32_OFF32_LIBS__unique, "_CS_POSIX_V6_ILP32_OFF32_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_ILP32_OFF32_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_ILP32_OFFBIG_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS__unique, "_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS__unique, "_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_ILP32_OFFBIG_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_ILP32_OFFBIG_LIBS__unique, "_CS_POSIX_V6_ILP32_OFFBIG_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_ILP32_OFFBIG_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_LP64_OFF64_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_LP64_OFF64_CFLAGS__unique, "_CS_POSIX_V6_LP64_OFF64_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_LP64_OFF64_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_LP64_OFF64_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_LP64_OFF64_LDFLAGS__unique, "_CS_POSIX_V6_LP64_OFF64_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_LP64_OFF64_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_LP64_OFF64_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_LP64_OFF64_LIBS__unique, "_CS_POSIX_V6_LP64_OFF64_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_LP64_OFF64_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS__unique, "_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS__unique, "_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_LPBIG_OFFBIG_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_LPBIG_OFFBIG_LIBS__unique, "_CS_POSIX_V6_LPBIG_OFFBIG_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_LPBIG_OFFBIG_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V6_WIDTH_RESTRICTED_ENVS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V6_WIDTH_RESTRICTED_ENVS__unique, "_CS_POSIX_V6_WIDTH_RESTRICTED_ENVS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V6_WIDTH_RESTRICTED_ENVS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_ILP32_OFF32_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_ILP32_OFF32_CFLAGS__unique, "_CS_POSIX_V7_ILP32_OFF32_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_ILP32_OFF32_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_ILP32_OFF32_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_ILP32_OFF32_LDFLAGS__unique, "_CS_POSIX_V7_ILP32_OFF32_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_ILP32_OFF32_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_ILP32_OFF32_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_ILP32_OFF32_LIBS__unique, "_CS_POSIX_V7_ILP32_OFF32_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_ILP32_OFF32_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_ILP32_OFFBIG_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_ILP32_OFFBIG_CFLAGS__unique, "_CS_POSIX_V7_ILP32_OFFBIG_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_ILP32_OFFBIG_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS__unique, "_CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_ILP32_OFFBIG_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_ILP32_OFFBIG_LIBS__unique, "_CS_POSIX_V7_ILP32_OFFBIG_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_ILP32_OFFBIG_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_LP64_OFF64_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_LP64_OFF64_CFLAGS__unique, "_CS_POSIX_V7_LP64_OFF64_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_LP64_OFF64_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_LP64_OFF64_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_LP64_OFF64_LDFLAGS__unique, "_CS_POSIX_V7_LP64_OFF64_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_LP64_OFF64_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_LP64_OFF64_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_LP64_OFF64_LIBS__unique, "_CS_POSIX_V7_LP64_OFF64_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_LP64_OFF64_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS__unique, "_CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS__unique, "_CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_LPBIG_OFFBIG_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_LPBIG_OFFBIG_LIBS__unique, "_CS_POSIX_V7_LPBIG_OFFBIG_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_LPBIG_OFFBIG_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_THREADS_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_THREADS_CFLAGS__unique, "_CS_POSIX_V7_THREADS_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_THREADS_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_THREADS_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_THREADS_LDFLAGS__unique, "_CS_POSIX_V7_THREADS_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_THREADS_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V7_WIDTH_RESTRICTED_ENVS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V7_WIDTH_RESTRICTED_ENVS__unique, "_CS_POSIX_V7_WIDTH_RESTRICTED_ENVS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V7_WIDTH_RESTRICTED_ENVS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_ILP32_OFF32_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_ILP32_OFF32_CFLAGS__unique, "_CS_POSIX_V8_ILP32_OFF32_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_ILP32_OFF32_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_ILP32_OFF32_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_ILP32_OFF32_LDFLAGS__unique, "_CS_POSIX_V8_ILP32_OFF32_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_ILP32_OFF32_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_ILP32_OFF32_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_ILP32_OFF32_LIBS__unique, "_CS_POSIX_V8_ILP32_OFF32_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_ILP32_OFF32_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_ILP32_OFFBIG_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_ILP32_OFFBIG_CFLAGS__unique, "_CS_POSIX_V8_ILP32_OFFBIG_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_ILP32_OFFBIG_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_ILP32_OFFBIG_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_ILP32_OFFBIG_LDFLAGS__unique, "_CS_POSIX_V8_ILP32_OFFBIG_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_ILP32_OFFBIG_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_ILP32_OFFBIG_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_ILP32_OFFBIG_LIBS__unique, "_CS_POSIX_V8_ILP32_OFFBIG_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_ILP32_OFFBIG_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_LP64_OFF64_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_LP64_OFF64_CFLAGS__unique, "_CS_POSIX_V8_LP64_OFF64_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_LP64_OFF64_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_LP64_OFF64_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_LP64_OFF64_LDFLAGS__unique, "_CS_POSIX_V8_LP64_OFF64_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_LP64_OFF64_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_LP64_OFF64_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_LP64_OFF64_LIBS__unique, "_CS_POSIX_V8_LP64_OFF64_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_LP64_OFF64_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_LPBIG_OFFBIG_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_LPBIG_OFFBIG_CFLAGS__unique, "_CS_POSIX_V8_LPBIG_OFFBIG_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_LPBIG_OFFBIG_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_LPBIG_OFFBIG_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_LPBIG_OFFBIG_LDFLAGS__unique, "_CS_POSIX_V8_LPBIG_OFFBIG_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_LPBIG_OFFBIG_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_LPBIG_OFFBIG_LIBS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_LPBIG_OFFBIG_LIBS__unique, "_CS_POSIX_V8_LPBIG_OFFBIG_LIBS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_LPBIG_OFFBIG_LIBS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_THREADS_CFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_THREADS_CFLAGS__unique, "_CS_POSIX_V8_THREADS_CFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_THREADS_CFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_THREADS_LDFLAGS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_THREADS_LDFLAGS__unique, "_CS_POSIX_V8_THREADS_LDFLAGS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_THREADS_LDFLAGS));
}
# endif

# if defined(LC_REQ__CS_POSIX_V8_WIDTH_RESTRICTED_ENVS) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_POSIX_V8_WIDTH_RESTRICTED_ENVS__unique, "_CS_POSIX_V8_WIDTH_RESTRICTED_ENVS", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_POSIX_V8_WIDTH_RESTRICTED_ENVS));
}
# endif

# if defined(LC_REQ__CS_V6_ENV) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_V6_ENV__unique, "_CS_V6_ENV", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_V6_ENV));
}
# endif

# if defined(LC_REQ__CS_V7_ENV) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_V7_ENV__unique, "_CS_V7_ENV", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_V7_ENV));
}
# endif

# if defined(LC_REQ__CS_V8_ENV) || defined(LC_DEFS_ALL)
TEST_SYM(_CS_V8_ENV__unique, "_CS_V8_ENV", distinct_from_the_other_cs_names, .ref = "XBD <unistd.h> (confstr())")
{
	EXPECT_TRUE(bitsConfstrUnique((long)_CS_V8_ENV));
}
# endif


#endif
